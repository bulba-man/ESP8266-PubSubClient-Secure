#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include <time.h>
#include <ArduinoJson.h>
#include "certificates.h"

//-------- Customise these values -----------
const char* ssid = "<yourSSID>";
const char* password = "<yourPassword>";

#define ORG "quickstart" // "quickstart" or use your organisation
#define DEVICE_ID "Test5"      
#define DEVICE_TYPE "ESP8266" // your device type or not used for "quickstart"
#define TOKEN "xxxxxxxxxxxxx" // your device token or not used for "quickstart"
//-------- Customise the above values --------

char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

const char publishTopic[] = "iot-2/evt/status/fmt/json";
const char responseTopic[] = "iotdm-1/response";
const char manageTopic[] = "iotdevice-1/mgmt/manage";
const char updateTopic[] = "iotdm-1/device/update";
const char rebootTopic[] = "iotdm-1/mgmt/initiate/device/reboot";

void callback(char* topic, byte* payload, unsigned int payloadLength);

BearSSL::WiFiClientSecure wifiClient;
PubSubClient client(server, 8883, callback, wifiClient);

int publishInterval = 3000; // 30 seconds
long lastPublishMillis;

void setup() {
  Serial.begin(115200); Serial.println();

  wifiConnect();
  mqttConnect();
  initManagedDevice();
}

void loop() {
  if (millis() - lastPublishMillis > publishInterval) {
    publishData();
    lastPublishMillis = millis();
  }

  if (!client.loop()) {
    mqttConnect();
    initManagedDevice();
  }
}

void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected, IP address: "); Serial.println(WiFi.localIP());

  setClock();

  BearSSL::X509List *client_cert = new BearSSL::X509List(CLIENT_CERT);
  BearSSL::PrivateKey *client_key = new BearSSL::PrivateKey(CLIENT_KEY);
  wifiClient.setClientRSACert(client_cert, client_key);

  BearSSL::X509List *ca_cert = new BearSSL::X509List(CA_CERT);
  wifiClient.setTrustAnchors(ca_cert);
}

void mqttConnect() {
  if (!!!client.connected()) {
    
    //you can tell BearSSL not to check the certificate of the server
    //wifiClient.setInsecure();

    Serial.print("Connecting MQTT client to "); Serial.println(server);
    while (!!!client.connect(clientId, authMethod, token)) {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");

      Serial.printf("SSLError: ");
      Serial.println (wifiClient.getLastSSLError());

      // Wait 5 seconds before retrying
      delay(5000);
    }

    Serial.println();
    Serial.println("MQTT connected!");
  }
}

void subscribeTopic(char* topic) {
  if (client.subscribe(topic)) {
    Serial.print("Subscribed to topic: "); Serial.println(topic);
  } else {
    Serial.print("ERROR subscribing to topic: "); Serial.println(topic);
  }
}

void initManagedDevice() {
  
  subscribeTopic((char*) responseTopic);
  subscribeTopic((char*) rebootTopic);
  subscribeTopic((char*) updateTopic);

  StaticJsonDocument<300> jsonBuffer;

  jsonBuffer["d"] = "";
  jsonBuffer["d"]["metadata"] = "";
  jsonBuffer["d"]["metadata"]["publishInterval"] = publishInterval;
  jsonBuffer["d"]["supports"] = "";
  jsonBuffer["d"]["supports"]["deviceActions"] = true;

  char buff[300];
  serializeJson(jsonBuffer, buff);

  Serial.println("publishing device metadata:"); Serial.println(buff);

  if (client.publish(manageTopic, buff)) {
    Serial.println("device Publish ok");
  } else {
    Serial.print("device Publish failed:");
  }
}

void publishData() {

  float temperature = 25.3;

  String payload = "{\"d\":{\"temperature\":";
  payload += temperature;
  payload += "}}";

  Serial.print("Sending payload: "); Serial.println(payload);

  if (client.publish(publishTopic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  Serial.print("callback invoked for topic: "); Serial.println(topic);

  if (strcmp (responseTopic, topic) == 0) {
    return; // just print of response for now
  }

  if (strcmp (rebootTopic, topic) == 0) {
    Serial.println("Rebooting...");
    ESP.restart();
  }

  if (strcmp (updateTopic, topic) == 0) {
    handleUpdate(payload);
  }
}

void handleUpdate(byte* payload) {
  StaticJsonDocument<300> root;
  
  auto error = deserializeJson(root, (char*)payload);
  if (error) {
    Serial.print("handleUpdate: payload parse FAILED with code: ");
    Serial.println(error.c_str());
    return;
  }
  
  Serial.println("handleUpdate payload:"); serializeJsonPretty(root, Serial); Serial.println();

  JsonObject d = root["d"];
  JsonArray fields = d["fields"];
  for (JsonArray::iterator it = fields.begin(); it != fields.end(); ++it) {
    JsonObject field = *it;
    const char* fieldName = field["field"];
    if (strcmp (fieldName, "metadata") == 0) {
      JsonObject fieldValue = field["value"];
      if (fieldValue.containsKey("publishInterval")) {
        publishInterval = fieldValue["publishInterval"];
        Serial.print("publishInterval:"); Serial.println(publishInterval);
      }
    }
  }
}

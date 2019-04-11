#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include <time.h>
#include "certs.h"
#include "helper.h"

//-------- Customise these values -----------
//___________________________________________
#define WIFI_SSID       "<yourSSID>"
#define WIFI_PASSWORD   "<yourPassword>"

#define MQTT_ORG        "<iot hub name>"
#define MQTT_DEVICE_ID  "<device id>"
#define MQTT_PASSWORD   "<SAS token>"
//___________________________________________
//-------- Customise the above values --------

const char* wifi_ssid = WIFI_SSID;
const char* wifi_password = WIFI_PASSWORD;

const char* mqtt_host = MQTT_ORG ".azure-devices.net";
const uint16_t mqtt_port = 8883;
const char* mqtt_client_id = MQTT_DEVICE_ID;
const char* mqtt_user = MQTT_ORG ".azure-devices.net/" MQTT_DEVICE_ID "/?api-version=2018-06-30";
const char* mqtt_password = MQTT_PASSWORD;

const char* mqtt_subscribe_topic = "devices/" MQTT_DEVICE_ID "/messages/devicebound/#";
const char* mqtt_publish_topic = "devices/" MQTT_DEVICE_ID "/messages/events/";

int publishInterval = 3000; // 3 seconds
long lastPublishMillis;
byte addr[8];

void callback(char* topic, byte* payload, unsigned int payloadLength);

#define MQTT_MAX_PACKET_SIZE 512

BearSSL::WiFiClientSecure wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  delay(1000);
  Serial.begin(115200); Serial.println();

  wifiConnect();
  setClock();
  mqttConnect();
  mqttManage();
}

void loop() {
  if (millis() - lastPublishMillis > publishInterval) {
    publishData();
    lastPublishMillis = millis();
  }

  if (!mqttClient.loop()) {
    mqttConnect();
    mqttManage();
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
  Serial.print("Connecting to "); Serial.print(WIFI_SSID);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!!!mqttClient.connected()) {
    BearSSL::X509List *ca_cert = new BearSSL::X509List(CA_CERT);
    wifiClient.setTrustAnchors(ca_cert);

//    BearSSL::X509List *client_cert = new BearSSL::X509List(CLIENT_CERT);
//    BearSSL::PrivateKey *client_key = new BearSSL::PrivateKey(CLIENT_KEY);
//    wifiClient.setClientRSACert(client_cert, client_key);

    mqttClient.setClient(wifiClient);
    mqttClient.setServer(mqtt_host, mqtt_port);
    mqttClient.setCallback(callback);

    Serial.print("Connecting MQTT client to "); Serial.println(mqtt_host);

    while (!!!mqttClient.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
      int state = mqttClient.state();
      if (state) {
        Serial.print("failed, rc = "); Serial.println(state);
        Serial.print("Error: "); Serial.println(pub_sub_error(state));
      }

      uint8_t ssl_error = wifiClient.getLastSSLError();
      if (ssl_error) {
        Serial.print("SSL Error: "); Serial.print(ssl_error); Serial.print(" - "); Serial.println(SSL_ERRORS[ssl_error]);
      }

      Serial.println();
      Serial.println("try again in 5 seconds");
      delay(5000);
    }

    Serial.println();
    Serial.println("MQTT connected!");
  }
}


void mqttManage() {
  subscribeTopic((char*) mqtt_subscribe_topic);
}


void subscribeTopic(char* topic) {
  if (mqttClient.subscribe(topic)) {
    Serial.print("Subscribed to topic: "); Serial.println(topic);
  } else {
    Serial.print("ERROR subscribing to topic: "); Serial.println(topic);
  }
}

void publishData() {

  String payload = "{\"deviceId\":\"" MQTT_DEVICE_ID "\", \"temp\":12}";

  Serial.print("Sending payload: "); Serial.println(payload);

  if (mqttClient.publish(mqtt_publish_topic, (char*) payload.c_str())) {
    Serial.println("Publish OK");
  } else {
    Serial.println("Publish FAILED");
  }
}

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  Serial.println();
  Serial.println('----------');
  Serial.print("callback invoked for topic: "); Serial.println(topic);

  Serial.print("Message arrived: ");
  for (int i = 0; i < payloadLength; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println('----------');
  Serial.println();
}

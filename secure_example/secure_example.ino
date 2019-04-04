#include <ESP8266WiFi.h>
#include "PubSubClient.h"
#include <time.h>
#include "certificates.h"

const char* ssid = "<yourSSID>";
const char* password = "<yourPassword>";

char server[] = "<your secure server>";
uint16_t port = 8883;

void callback(char* topic, byte* payload, unsigned int payloadLength);

BearSSL::WiFiClientSecure wifiClient;
PubSubClient client(server, 8883, wifiClient);

void setup() {
  Serial.begin(115200); Serial.println();

  wifiConnect();
  mqttConnect();
}

void loop() {
  
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

  //if your server requires a CA
//  BearSSL::X509List *ca_cert = new BearSSL::X509List(CA_CERT);
//  wifiClient.setTrustAnchors(ca_cert);
}

void mqttConnect() {
  if (!!!client.connected()) {
    
    //you can tell BearSSL not to check the certificate of the server
    //wifiClient.setInsecure();

    Serial.print("Connecting MQTT client to "); Serial.println(server);

    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    
    while (!!!client.connect(clientId.c_str())) {
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

void callback(char* topic, byte* payload, unsigned int payloadLength) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < payloadLength; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

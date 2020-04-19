#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#ifndef STASSID
#define STASSID "ssid"
#define STAPSK  "password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;


#define MAX_SRV_CLIENTS 1
WiFiServer server(9999);
WiFiClient serverClient;

void setup() {
  Serial.begin(9600);
  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  IPAddress ip(192, 168, 0, 15);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress dns(192, 168, 0, 1);
  IPAddress gateway(192, 168, 0, 1);
  WiFi.config(ip, gateway, subnet, dns);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("onstep");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Serveur Telnet
  server.begin();
  server.setNoDelay(true);
}

void loop() {
  ArduinoOTA.handle();
  if (server.hasClient())
    AcceptConnection();
  else if (serverClient && serverClient.connected())
    ManageConnected();

  //  httpServer.handleClient();
}

void AcceptConnection()
{
  if (serverClient && serverClient.connected())
    serverClient.stop();

  serverClient = server.available();
  //serverClient.write("ESP8266 Connected!\n");
}

void ManageConnected()
{
  String Rep = serverClient.readStringUntil(35);
  Serial.print(Rep); Serial.println('#');
  unsigned long debut = millis();
  while (! Serial.available()  && (millis()-debut) < 1000) {

  }
  serverClient.print(Serial.readStringUntil(35));
  serverClient.print('#');
  serverClient.stop();

}

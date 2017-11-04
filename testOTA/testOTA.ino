#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Mega";
const char* password = "inter1017net";
const char* usr = "dev3";
const char* pwd = "dev3dev3";
int currentValweState = -1;

void setup() {
  Serial.begin(19200);
  Serial.println("Booting");
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  ArduinoOTA.setPort(8266);


  String hostName = "esp8266:";
  hostName += usr;
  ArduinoOTA.setHostname(hostName.c_str());

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
  pinMode(16, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(5, INPUT_PULLUP);
  valweSet(0);
}

void valweSet(int state) {
  
  Serial.printf("valweSet(%d)\n", state);
  Serial.printf("currentValweState =  %d \n", currentValweState);

  Serial.write(0xA0);
  Serial.write(0x04);
  Serial.write(state == 1 ? 0x06 : 0x05);
  Serial.write(0xA1);
  Serial.flush();
}

void loop() {

  delay(5000);
  ArduinoOTA.handle();
}

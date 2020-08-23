#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>


const char* ssid = "Mega";
const char* password = "inter1017net";
const char* usr = "dev11";
const char* pwd = "dev11dev11";
boolean buttonPressed = false;
void ICACHE_RAM_ATTR buttonPress();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Booting");

  WiFi.begin(ssid, password);
  String hostName = "esp8266:";
  hostName += usr;
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());



  
  pinMode(12, OUTPUT);//blue Led + relay
  pinMode(13, OUTPUT);//green led (inverse)
  pinMode(0, INPUT_PULLUP);//button
  setDefaultPortValues();

  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname(hostName.c_str());ArduinoOTA.onStart([]() {
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

  attachInterrupt(0, buttonPress, FALLING);
  sei();
  
  Serial.println("Setup Ok");
}

void setDefaultPortValues() {
  digitalWrite(13, 1);
}

void buttonPress () {
  buttonPressed = true;
} 

void loop() {
  ArduinoOTA.handle();

  Serial.println(buttonPressed);
  if(buttonPressed == 1) {
    digitalWrite(12, digitalRead(12) == 0 ? 1 : 0);
    buttonPressed = false;
  }


  digitalWrite(13, !digitalRead(13));
  delay(100);
  digitalWrite(3, !digitalRead(13));
  delay(100);
  digitalWrite(13, !digitalRead(13));
  delay(100);
  digitalWrite(13, !digitalRead(13));
  delay(100);
  digitalWrite(13, !digitalRead(13));
  delay(100);
  digitalWrite(13, !digitalRead(13));
  // wait for WiFi connection
  if((WiFi.status() == WL_CONNECTED)) {

    HTTPClient http;
    Serial.println("[HTTP] start...");
    String url = "http://iot.lyolek.dp.ua/services/device.php";
    http.begin(url);
    http.setAuthorization(usr, pwd);

    int httpCode = http.GET();
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if(httpCode == 200) {
      String resp = http.getString();
      Serial.println(resp);
//      char JSONMessage[resp.length() + 1];
//      resp.toCharArray(JSONMessage, resp.length() + 1);
//      Serial.println(JSONMessage);
//      StaticJsonBuffer<300> JSONBuffer;   //Memory pool
//      JsonObject& parsed = JSONBuffer.parseObject(JSONMessage); //Parse message
//      if (!parsed.success()) {   //Check for errors in parsing
//        Serial.println("Parsing failed");
//        delay(5000);
//        return;
//      }
      
      DynamicJsonDocument parsed(JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(3) + resp.length());
      DeserializationError error = deserializeJson(parsed, resp);
      if (error) {   //Check for errors in parsing
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        delay(5000);
        return;
      }
      int GPIO12 = parsed["GPIO12"];
      Serial.printf("GPIO12=%d\n", GPIO12);

      
      digitalWrite(12, GPIO12);
      digitalWrite(13, 0);
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        setDefaultPortValues();
    }
    http.end();
  } else {
    setDefaultPortValues();
  }
  delay(5000);
}

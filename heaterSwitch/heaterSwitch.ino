#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Mega";
const char* password = "inter1017net";
const char* usr = "dev2";
const char* pwd = "dev2dev2";

unsigned long lastRun;
float setTemp;

#define DHTPIN 14

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println("Booting");

  WiFi.begin(ssid, password);
  String hostName = "esp8266:";
  hostName += usr;
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  dht.begin();
  
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  setDefaultPortValues();
}

void setDefaultPortValues() {
  digitalWrite(12, 1);
  digitalWrite(13, 1);
}

void loop() {
   delay(2000);
  
    float t = dht.readTemperature();
    Serial.println("t=" + String(t));
  
    if (isnan(t)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  
    // wait for WiFi connection
    if((WiFi.status() == WL_CONNECTED)) {
  
      HTTPClient http;
      Serial.println("[HTTP] start...");
      String url = "http://iot.lyolek.dp.ua/services/device.php?";
      url += "GPIO14=" + String(t) + "&";
      url += "GPIO100=" + String(digitalRead(12)) + "&";
      url += "GPIO101=" + String(setTemp);
      Serial.println(url);
      http.begin(url);
      http.setAuthorization(usr, pwd);
  
      int httpCode = http.GET();
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if(httpCode == 200) {
        String resp = http.getString();
        char JSONMessage[resp.length() + 1];
        resp.toCharArray(JSONMessage, resp.length() + 1);
        Serial.println(JSONMessage);
        StaticJsonBuffer<300> JSONBuffer;   //Memory pool
        JsonObject& parsed = JSONBuffer.parseObject(JSONMessage); //Parse message
        if (!parsed.success()) {   //Check for errors in parsing
  
          Serial.println("Parsing failed");
          delay(5000);
          return;
        }
        setTemp = parsed["GPIO12"];
        Serial.printf("GPIO12=%d\n", setTemp);
  
        if(t < (setTemp - 0.5)) {
          digitalWrite(12, 1);
          Serial.println("Set ON");
        } else if(t > (setTemp + 0.5)) {
          digitalWrite(12, 0);
          Serial.println("Set OFF");
        }
        digitalWrite(13, 0);
      } else {
          Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
          setDefaultPortValues();
      }
      http.end();
    } else {
      setDefaultPortValues();
    }

    
  while(millis() - lastRun < 10000) {
    delay(500);
  }
  lastRun = millis();

}

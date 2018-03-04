#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "TP-LINK-158";
const char* password = "inter158net";
const char* usr = "dev3";
const char* pwd = "dev3dev3";

unsigned long lastRun;
float setTemp = 10;
float currAirTemp = setTemp;
float currFlorTemp = setTemp;

#define ONE_WIRE_BUS 14

OneWire oneWire(ONE_WIRE_BUS);
DeviceAddress florThermometer = {0x28, 0xFF, 0x69, 0xE1, 0x02, 0x17, 0x03, 0x43};//пол 28 FF 69 E1 02 17 03 43
DeviceAddress airThermometer = {0x28, 0xFF, 0xF4, 0x35, 0xA1, 0x16, 0x04, 0x42};//воздух 28 FF F4 35 A1 16 04 42
DallasTemperature sensors(&oneWire);

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

  sensors.begin();
  sensors.setResolution(airThermometer, 12);
  sensors.setResolution(florThermometer, 12);
  
  pinMode(12, OUTPUT);//RELAY+LED RED
  pinMode(13, OUTPUT);//LED BLUE
  setDefaultPortValues();
}

void setDefaultPortValues() {
  setRelay();
  digitalWrite(13, 1);
}

void setRelay() {
  if(currFlorTemp < (setTemp - 0.3)) {
    digitalWrite(12, 1);
    Serial.println("Set ON");
  } else if(currFlorTemp > (setTemp + 0.3)) {
    digitalWrite(12, 0);
    Serial.println("Set OFF");
  }
}

void loop() {
    Serial.println("New Loop");


    sensors.requestTemperatures();
    currAirTemp = sensors.getTempC(airThermometer);
    currFlorTemp = sensors.getTempC(florThermometer);
    Serial.println("currAirTemp=" + String(currAirTemp));
    Serial.println("currFlorTemp=" + String(currFlorTemp));
  
    // wait for WiFi connection
    if((WiFi.status() == WL_CONNECTED)) {
  
      HTTPClient http;
      Serial.println("[HTTP] start...");
      String url = "http://iot.lyolek.dp.ua/services/device.php?";
      url += "GPIO01=" + String(currAirTemp) + "&";
      url += "GPIO03=" + String(currFlorTemp) + "&";
      url += "GPIO100=" + String(digitalRead(12));
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
          setDefaultPortValues();
          return;
        }
        setTemp = parsed["GPIO12"];
        Serial.printf("GPIO12=%d\n", setTemp);
        setRelay();
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

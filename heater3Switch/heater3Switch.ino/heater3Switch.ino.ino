#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

const char* ssid = "TP-LINK-158";
const char* password = "inter158net";
const char* usr = "dev10";
const char* pwd = "dev10dev10";

unsigned long currentTime;
unsigned long prewTime;
unsigned long lastSwitch;
float setTemp = 10;
float currAirTemp = setTemp;


#define ledPin 15
#define relayPin 12

#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DeviceAddress airThermometer = {0x28, 0xFF, 0x96, 0x1B, 0x01, 0x17, 0x04, 0x87};//воздух
//ROM = 28 FF 96 1B 1 17 4 87


DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println("Booting");

  WiFi.begin(ssid, password);
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  sensors.begin();
  sensors.setResolution(airThermometer, 12);
  
  pinMode(relayPin, OUTPUT);//RELAY+LED RED
  pinMode(ledPin, OUTPUT);//LED BLUE
  setDefaultPortValues();

  ArduinoOTA.setPort(8266);
  String hostName = "esp8266:";
  hostName += usr;
  ArduinoOTA.setHostname(hostName.c_str());
  ArduinoOTA.begin();
}

void setDefaultPortValues() {
  setRelay();
  digitalWrite(ledPin, 0);
}

void setRelay() {
  if(!digitalRead(relayPin) && millis() - lastSwitch < 5*60*1000) {
    return;
  }
  if(digitalRead(relayPin) && millis() - lastSwitch > 25*60*1000) {
    lastSwitch = millis();
    digitalWrite(relayPin, 0);
    Serial.println("Set OFF");
    return;
  }
  if(currAirTemp < (setTemp - 0.3)) {
    if(!digitalRead(relayPin)) {
      lastSwitch = millis();
      digitalWrite(relayPin, 1);
      Serial.println("Set ON");
    }
  } else if(currAirTemp > (setTemp + 0.3)) {
    if(digitalRead(relayPin)) {
      lastSwitch = millis();
      digitalWrite(relayPin, 0);
      Serial.println("Set OFF");
    }
  }
}

void loop() {
  ArduinoOTA.handle();

  currentTime = millis();
  if(currentTime < (prewTime + 20000)) {
    return;
  }
  prewTime = currentTime;

  sensors.requestTemperatures();
  currAirTemp = sensors.getTempC(airThermometer);
  Serial.println("currAirTemp=" + String(currAirTemp));


  digitalWrite(ledPin, !digitalRead(ledPin));
  delay(100);
  digitalWrite(ledPin, !digitalRead(ledPin));
  delay(100);
  digitalWrite(ledPin, !digitalRead(ledPin));
  delay(100);
  digitalWrite(ledPin, !digitalRead(ledPin));
  delay(100);
  digitalWrite(ledPin, !digitalRead(ledPin));
  delay(100);
  digitalWrite(ledPin, !digitalRead(ledPin));
  
  // wait for WiFi connection
  if((WiFi.status() == WL_CONNECTED)) {

    HTTPClient http;
    Serial.println("[HTTP] start...");
    String url = "http://iot.lyolek.dp.ua/services/device.php?";
    url += "GPIO01=" + String(currAirTemp) + "&";
    url += "GPIO100=" + String(digitalRead(relayPin));
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
      digitalWrite(ledPin, 1);
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        setDefaultPortValues();
    }
    http.end();
  } else {
    setDefaultPortValues();
  }
}

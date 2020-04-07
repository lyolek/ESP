#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"
#include <SoftwareSerial.h>


Adafruit_MCP23017 mcp;
SoftwareSerial RFIDSerial(12, 13); // RX, TX

const char* ssid = "TP-LINK-158";
const char* password = "inter158net";
const char* usr = "dev8";
const char* pwd = "dev8dev8";

#define SENSOR1_IR_PIN_MCP 14
#define SENSOR1_DOPL_PIN_MCP 15
#define SENSOR2_IR_PIN_MCP 13
#define SENSOR2_DOPL_PIN_MCP 12
#define SENSOR_DOOR_PIN_MCP 11

#define LED_NET_PIN_MCP 6
#define LED_ARM_PIN_MCP 7
#define BOOZER_PIN_MCP 4
#define ALARM_PIN_MCP 0


int rfidInput;
String rfidS;
bool isDOPL1 = false;
bool isIR1 = false;
bool isDOPL2 = false;
bool isIR2 = false;
bool isArmed = false;
bool isArmedToggle = false;
bool isArmedToggleSent = false;
bool isNeedBeep = false;
unsigned long currentTime;
unsigned long prewTime;
unsigned long rfidPrewTime;
unsigned long rfidLastRead;



void setup() {
  Serial.flush();
  Serial.begin(9600);
  RFIDSerial.begin(9600);
  rfidS="";
  Serial.println("Setup...");

  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
  mcp.begin();      // use default address 0
  
  // On interrupt, polariy is set HIGH/LOW (last parameter).
  mcp.setupInterrupts(true, false, LOW);

  mcp.pinMode(LED_ARM_PIN_MCP, OUTPUT);
  mcp.pinMode(LED_NET_PIN_MCP, OUTPUT);
  mcp.pinMode(BOOZER_PIN_MCP, OUTPUT);
  mcp.pinMode(ALARM_PIN_MCP, OUTPUT);
  mcp.pinMode(SENSOR1_IR_PIN_MCP, INPUT);
  mcp.pinMode(SENSOR1_DOPL_PIN_MCP, INPUT);
  mcp.pinMode(SENSOR2_IR_PIN_MCP, INPUT);
  mcp.pinMode(SENSOR2_DOPL_PIN_MCP, INPUT);
  mcp.pinMode(SENSOR_DOOR_PIN_MCP, INPUT);
  
  mcp.pullUp(SENSOR1_IR_PIN_MCP, HIGH);
  mcp.pullUp(SENSOR1_DOPL_PIN_MCP, HIGH);
  mcp.pullUp(SENSOR2_IR_PIN_MCP, HIGH);
  mcp.pullUp(SENSOR2_DOPL_PIN_MCP, HIGH);
  mcp.pullUp(SENSOR_DOOR_PIN_MCP, HIGH);
  
  mcp.digitalWrite(LED_NET_PIN_MCP, 0);
  mcp.digitalWrite(LED_ARM_PIN_MCP, isArmed);
  mcp.digitalWrite(BOOZER_PIN_MCP, 0);
  mcp.digitalWrite(ALARM_PIN_MCP, 0);

  mcp.setupInterruptPin(SENSOR1_IR_PIN_MCP, CHANGE);
  mcp.setupInterruptPin(SENSOR1_DOPL_PIN_MCP, CHANGE);
  mcp.setupInterruptPin(SENSOR2_IR_PIN_MCP, CHANGE);
  mcp.setupInterruptPin(SENSOR2_DOPL_PIN_MCP, CHANGE);
  mcp.setupInterruptPin(SENSOR_DOOR_PIN_MCP, CHANGE);

  attachInterrupt(2, detected, CHANGE); // Setup Interrupt
  mcp.readGPIOAB();


  currentTime = millis();
  prewTime = currentTime;
  rfidPrewTime = millis();
  rfidLastRead = millis();

  
  ArduinoOTA.setPort(8266);
  String hostName = "esp8266:";
  hostName += usr;
  ArduinoOTA.setHostname(hostName.c_str());
  ArduinoOTA.begin();
}

void setDefaultPortValues() {
  isArmedToggleSent = false;
}

void detected () {
  int pin = mcp.getLastInterruptPin();
  int val = mcp.getLastInterruptPinValue();
  Serial.println("int on " + String(pin) + "=" + String(val));
  if(pin != 255 && val == 1) {
    if(isArmed) {
//      isNeedBeep = true;
      if(pin == SENSOR1_IR_PIN_MCP){
        isIR1 = true;
      }
      if(pin == SENSOR1_DOPL_PIN_MCP){
        isDOPL1 = true;
      }
    }
//    flow_frequency[pin-8]++;
  }
}

void setArmed(bool flag) {
  isArmed = flag;
  mcp.digitalWrite(LED_ARM_PIN_MCP, isArmed);
  mcp.digitalWrite(BOOZER_PIN_MCP, 1);
  delay(100);
  mcp.digitalWrite(BOOZER_PIN_MCP, 0);
  delay(100);
  if(isArmed) {
    mcp.digitalWrite(BOOZER_PIN_MCP, 1);
    delay(100);
    mcp.digitalWrite(BOOZER_PIN_MCP, 0);
    delay(100);
  }
}

void toggleArmed() {
  setArmed(!isArmed);
  isArmedToggle = true;
}

void loop() {
  ArduinoOTA.handle();
  // если есть данные, то
  if (RFIDSerial.available() > 0) {
    rfidLastRead = millis();
    // читаем блок данных с модуля RDM630 и заносим их в переменную input
    rfidInput = RFIDSerial.read();
    // присваиваем все считанные в переменную input значения переменной s, так как за 1 цикл loop мы получаем 1 значение, а их 12
    rfidS += rfidInput;
    if (rfidS.length() == 26) {
      Serial.println("=-" + rfidS);
      if(millis() - rfidPrewTime > 2000) {
Serial.println("rfidTouch");
        rfidPrewTime = millis();
        mcp.digitalWrite(BOOZER_PIN_MCP, 1);
        delay(100);
        mcp.digitalWrite(BOOZER_PIN_MCP, 0);
        delay(100);
        if(rfidS == "25056484856547051565168693") {
          toggleArmed();
        }
      }
      rfidS = "";
      RFIDSerial.flush();
    }
  }
  if(isNeedBeep) {
    mcp.digitalWrite(BOOZER_PIN_MCP, 1);
    delay(50);
    mcp.digitalWrite(BOOZER_PIN_MCP, 0);
    delay(50);
    mcp.digitalWrite(BOOZER_PIN_MCP, 1);
    delay(50);
    mcp.digitalWrite(BOOZER_PIN_MCP, 0);
    delay(50);
    mcp.digitalWrite(BOOZER_PIN_MCP, 1);
    delay(100);
    mcp.digitalWrite(BOOZER_PIN_MCP, 0);
    delay(50);
    isNeedBeep = false;
  }
  currentTime = millis();
  if(currentTime < (prewTime + 10000)) {
    return;
  }
  prewTime = currentTime;
  
  Serial.println("loop");

  if((WiFi.status() == WL_CONNECTED)) {
    HTTPClient http;
    String url = "http://iot.lyolek.dp.ua/services/device.php";
    url += "?GPIO12=" + String(isDOPL1 ? "1" : "0");
    url += "&GPIO14=" + String(isIR1 ? "1" : "0");
    url += "&GPIO100=" + String(isArmedToggle ? String(isArmed) : "");
    isArmedToggleSent = isArmedToggle;
    Serial.println(url);
    http.begin(url);
    http.setAuthorization(usr, pwd);
    
    int httpCode = http.GET();
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if(httpCode == 200) {
      String resp = http.getString();
      
      Serial.println(resp);
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
      if(!isArmedToggle && isArmed != parsed["GPIO100"]) {
        setArmed(parsed["GPIO100"]);
      }
      
      isDOPL1 = false;
      isIR1 = false;
      isDOPL2 = false;
      isIR2 = false;
      if(isArmedToggleSent) {
        isArmedToggle = false;
      }
      isArmedToggleSent = false;
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        setDefaultPortValues();
    }
  } else {
    setDefaultPortValues();
  }
}

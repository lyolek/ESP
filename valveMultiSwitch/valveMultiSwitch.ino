#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include "Adafruit_MCP23017.h"


const char* ssid = "Mega";
const char* password = "inter1017net";
const char* usr = "dev9";
const char* pwd = "dev9dev9";

unsigned char ledNet = 15;
unsigned char flowmeterINT = 13;

int faultCounter = 0;
unsigned int flow_frequency[] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned int valweSwitchDelay = 1500;

unsigned long currentTime;
unsigned long prewTime;

Adafruit_MCP23017 mcp;

void setup() {
  Serial.begin(19200);
  Serial.println("setup");
  mcp.begin();      // use default address 0
  
  // On interrupt, polariy is set HIGH/LOW (last parameter).
  mcp.setupInterrupts(true, false, LOW);
  
  mcp.pinMode(0, OUTPUT);
  mcp.pinMode(1, OUTPUT);
  mcp.pinMode(2, OUTPUT);
  mcp.pinMode(3, OUTPUT);
  mcp.pinMode(4, OUTPUT);
  mcp.pinMode(5, OUTPUT);
  mcp.pinMode(6, OUTPUT);
  mcp.pinMode(7, OUTPUT);
  
  mcp.pinMode(8, INPUT);
  mcp.pinMode(9, INPUT);
  mcp.pinMode(10, INPUT);
  mcp.pinMode(11, INPUT);
  mcp.pinMode(12, INPUT);
  mcp.pinMode(13, INPUT);
  mcp.pinMode(14, INPUT);
  mcp.pinMode(15, INPUT);
  mcp.pullUp(8, HIGH);
  mcp.pullUp(9, HIGH);
  mcp.pullUp(10, HIGH);
  mcp.pullUp(11, HIGH);
  mcp.pullUp(12, HIGH);
  mcp.pullUp(13, HIGH);
  mcp.pullUp(14, HIGH);
  mcp.pullUp(15, HIGH);

  mcp.setupInterruptPin(8,CHANGE);
  mcp.setupInterruptPin(9,CHANGE);
  mcp.setupInterruptPin(10,CHANGE);
  mcp.setupInterruptPin(11,CHANGE);
  mcp.setupInterruptPin(12,CHANGE);
  mcp.setupInterruptPin(13,CHANGE);
  mcp.setupInterruptPin(14,CHANGE);
  mcp.setupInterruptPin(15,CHANGE);

  attachInterrupt(flowmeterINT, flow, CHANGE); // Setup Interrupt
  mcp.readGPIOAB();

  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


  pinMode(ledNet, OUTPUT);
  pinMode(flowmeterINT, INPUT);

  setRelays(0);
    
  currentTime = millis();
  prewTime = currentTime;
}


void flow () { 
  int pin = mcp.getLastInterruptPin();
  int val = mcp.getLastInterruptPinValue();
  Serial.println("int on " + String(pin) + "=" + String(val));
  if(pin != 255 && val == 1) {
    flow_frequency[pin-8]++;
  }
}


void handleFault() {
Serial.println("handleFault" + String(faultCounter));
  if(faultCounter >= 10) {
    faultCounter = 0;
    setRelays(0);
  } else {
    faultCounter++;
  }
  digitalWrite(ledNet, 0);
}

void handleSuccess() {
Serial.println("handleSuccess" + String(faultCounter));
  faultCounter = 0;
  digitalWrite(ledNet, 1);
}

void setRelays(unsigned char state) {
  state = !state;
  for(int i = 0; i < 8; i++) {
    if(mcp.digitalRead(i) != state) {
Serial.println("go");
      mcp.digitalWrite(i, state);
    }
  }
  delay(valweSwitchDelay);
}

void setRelay(unsigned char relay, unsigned char state) {
Serial.println("setRelay(" + String(relay) + ")=" + String(state));
  state = !state;
  if(mcp.digitalRead(relay) != state) {
Serial.println("go");
    mcp.digitalWrite(relay, state);
    delay(valweSwitchDelay);
  }
}

void loop() {
  

  currentTime = millis();
  if(currentTime < (prewTime + 10000)) {
    return;
  }
  prewTime = currentTime;
    
  // wait for WiFi connection
  if((WiFi.status() == WL_CONNECTED)) {

Serial.println("blink");  
    digitalWrite(ledNet, !digitalRead(ledNet));
    delay(100);
    digitalWrite(ledNet, !digitalRead(ledNet));
    delay(100);
    digitalWrite(ledNet, !digitalRead(ledNet));
    delay(100);
    digitalWrite(ledNet, !digitalRead(ledNet));
    delay(100);
    digitalWrite(ledNet, !digitalRead(ledNet));
    delay(100);
    digitalWrite(ledNet, !digitalRead(ledNet));
    
    HTTPClient http;
    Serial.println("[HTTP] start...");
    String url = "http://iot.lyolek.dp.ua/services/device.php?";
    for(int i = 0; i < 8; i++) {
      url += "GPIO10" + String(i) + "=" + String(flow_frequency[i] * 6 / 7.5) + "&";
      flow_frequency[i] = 0;
    }
    for(int i = 0; i < 8; i++) {
      url += "GPIO20" + String(i) + "=" + String(!mcp.digitalRead(i)) + "&";
    }
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
        handleFault();
        return;
      }
      
      for(int i = 0; i < 8; i++) {
        char val = parsed["GPIO0" + String(i)];
        setRelay(i, val);
      }
//      int GPIO12 = parsed["GIO12"];
//      Serial.printf("GPIO16=%d\n", GPIO12);
//      setRelay();
      handleSuccess();
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        handleFault();
    }
    http.end();
  } else {
    handleFault();
  }
}

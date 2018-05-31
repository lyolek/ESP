#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "TP-LINK-158";
const char* password = "inter158net";
const char* usr = "dev5";
const char* pwd = "dev5dev5";

unsigned char flowmeter = 4;  // Flow Meter Pin number
unsigned char ledNet = 2;
unsigned char inpON = 14;
unsigned char inpOFF = 16;
unsigned char relayValve = 12;
//unsigned char relay = 13; //currently unused
int faultCounter = 0;

volatile int  flow_frequency;  // Measures flow meter pulses
unsigned long currentTime;
unsigned long prewTime;

void setup() {
//  Serial.begin(115200);
  Serial.begin(19200);
Serial.println("setup");
  attachInterrupt(flowmeter, flow, RISING); // Setup Interrupt 
  sei();
  
  Serial.setDebugOutput(true);
  Serial.println("Booting");

  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


//  http.setReuse(true);

  
  pinMode(ledNet, OUTPUT);
  pinMode(relayValve, OUTPUT);
  pinMode(inpON, INPUT_PULLUP);
  pinMode(inpOFF, INPUT_PULLUP);
  pinMode(flowmeter, INPUT_PULLUP);
  
  setRelay(0);
    
  currentTime = millis();
  prewTime = currentTime;
}


void flow () { 
   flow_frequency++;
}


void handleFault() {
Serial.println("handleFault" + faultCounter);
  if(faultCounter >= 10) {
    faultCounter = 0;
    if(digitalRead(inpON) == 1 && digitalRead(inpOFF) == 1) {
      setRelay(1);
    } else {
      setRelay(digitalRead(inpON));
    }
    digitalWrite(ledNet, 0);
  } else {
    faultCounter++;
  }
}

void handleSuccess(int state) {
Serial.println("handleSuccess" + faultCounter);
  faultCounter = 0;
  setRelay(state);
  digitalWrite(ledNet, 1);
}

//state is inversed
void setRelay(int state) {
Serial.println("setRelay" + state);
  if(digitalRead(relayValve) != state) {
Serial.println("go");
    digitalWrite(relayValve, state);
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

  
    digitalWrite(ledNet, digitalRead(ledNet) == 1 ? 0 : 1);
    delay(100);
    digitalWrite(ledNet, digitalRead(ledNet) == 1 ? 0 : 1);
    delay(100);
    digitalWrite(ledNet, digitalRead(ledNet) == 1 ? 0 : 1);
    delay(100);
    digitalWrite(ledNet, digitalRead(ledNet) == 1 ? 0 : 1);
    
    HTTPClient http;
    Serial.println("[HTTP] start...");
    String url = "http://iot.lyolek.dp.ua/services/device.php?GPIO02=";
    url += (flow_frequency * 6 / 7.5);
    url += "&GPIO120=";
    url += (digitalRead(relayValve) == 1 ? 0 : 1);
    flow_frequency = 0;
    Serial.println(url);
    http.begin(url);
    http.setAuthorization(usr, pwd);

    int httpCode = http.GET();
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    
Serial.printf("inpON=%d\n", digitalRead(inpON));
Serial.printf("inpOFF=%d\n", digitalRead(inpOFF));
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
      int GPIO12 = parsed["GPIO12"];
      Serial.printf("GPIO16=%d\n", GPIO12);

      
      if(digitalRead(inpON) == 1 && digitalRead(inpOFF) == 1) {
        handleSuccess(GPIO12 == 1 ? 0 : 1);
      } else {
        handleSuccess(digitalRead(inpON));
      }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        handleFault();
    }
    http.end();
  } else {
    handleFault();
  }
}

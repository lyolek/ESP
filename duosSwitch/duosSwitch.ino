#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "Mega";
const char* password = "inter1017net";
const char* usr = "dev5";
const char* pwd = "dev5dev5";
int currentValweState = -1;

unsigned char flowmeter = 5;  // Flow Meter Pin number
volatile int  flow_frequency;  // Measures flow meter pulses

void setup() {
//  Serial.begin(115200);
  Serial.begin(19200);
  attachInterrupt(flowmeter, flow, RISING); // Setup Interrupt 
  sei();
  
  Serial.setDebugOutput(true);
  Serial.println("Booting");

  WiFi.begin(ssid, password);

  String hostName = "esp8266:";
  hostName += usr;
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


//  http.setReuse(true);

  
  pinMode(13, OUTPUT);
  pinMode(flowmeter, INPUT_PULLUP);
//  pinMode(1, INPUT_PULLUP);
//  pinMode(3, INPUT_PULLUP);
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
void flow () { 
   flow_frequency++;
} 
void loop() {

  // wait for WiFi connection
  if((WiFi.status() == WL_CONNECTED)) {

    HTTPClient http;
    Serial.println("[HTTP] start...");
    String url = "http://iot.lyolek.dp.ua/services/device.php?GPIO01=";
    url += flow_frequency;
    flow_frequency = 0;
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
        digitalWrite(13, 1);
        valweSet(0);
        delay(5000);
        return;
      }
      int GPIO16 = parsed["GPIO16"];
      Serial.printf("GPIO16=%d\n", GPIO16);

      
      digitalWrite(13, 0);
      valweSet(GPIO16);
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        digitalWrite(13, 1);
        valweSet(0);
    }
    http.end();
  } else {
    digitalWrite(13, 1);
    valweSet(0);
  }
  delay(5000);
}

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "Mega";
const char* password = "inter1017net";
const char* usr = "dev6";
const char* pwd = "dev6dev6";
unsigned long button1Last;
unsigned long button2Last;
unsigned long currentTime;
unsigned long prewTime;
boolean button1Pressed = false;
boolean button2Pressed = false;
int relay1 = 12;
int relay2 = 5;
int button1 = 0;
int button2 = 9;
int ledNet = 13;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Booting");


  pinMode(button1, INPUT_PULLUP);
  pinMode(button2, INPUT_PULLUP);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(ledNet, OUTPUT);

  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  String hostName = "esp8266:";
  hostName += usr;
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  setDefaultPortValues();
  
  currentTime = millis();
  prewTime = currentTime;

  attachInterrupt(button1, button1Press, FALLING);
  attachInterrupt(button2, button2Press, FALLING);
  sei();
}

void setDefaultPortValues() {
  digitalWrite(relay1, 0);
  digitalWrite(relay2, 0);
  digitalWrite(ledNet, 1);
}

void button1Press () {
  Serial.println("b1");
  digitalWrite(relay1, digitalRead(relay1) == 1 ? 0 : 1);
  button1Last = millis();
  Serial.println("b1");
}

void button2Press () {
  Serial.println("b2");
  digitalWrite(relay2, digitalRead(relay2) == 1 ? 0 : 1);
  button2Last = millis();
  Serial.println("b2");
}

void loop() {
  currentTime = millis();
  if(currentTime < (prewTime + 5000)) {
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
    String url = "http://iot.lyolek.dp.ua/services/device.php";
    url += "?GPIO12=" + String(digitalRead(relay1));
    url += "&GPIO05=" + String(digitalRead(relay2));
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
      int relay1State = parsed["GPIO12"];
      Serial.printf("relay1State=%d\n", relay1State);
      int relay2State = parsed["GPIO05"];
      Serial.printf("relay2State=%d\n", relay2State);
      
      if(currentTime - button1Last > 30*60*1000 || button1Last == 0) {
        digitalWrite(relay1, relay1State);
      }
      if(currentTime - button2Last > 30*60*1000 || button2Last == 0) {
        digitalWrite(relay2, relay2State);
      }
      digitalWrite(ledNet, 0);
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        setDefaultPortValues();
    }
    http.end();
  } else {
    setDefaultPortValues();
  }
}

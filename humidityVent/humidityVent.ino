#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "Mega";
const char* password = "inter1017net";
const char* usr = "dev7";
const char* pwd = "dev7dev7";

unsigned long lastRun;
unsigned long buttonLast;
unsigned long currentTime = millis();
unsigned long prewTime;
float setHumidity = 10;
float currTemp;
float currHumidity = setHumidity;

int ledNet = 13;
int relay = 12;
int sensor = 14;
int button = 0;

#define DHTPIN sensor

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Serial.setDebugOutput(true);
  Serial.println("Booting");

  WiFi.begin(ssid, password);
  WiFi.mode(WIFI_STA);
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  dht.begin();
  
  pinMode(relay, OUTPUT);
  pinMode(ledNet, OUTPUT);
  digitalWrite(relay, 0);
  digitalWrite(ledNet, 1);
  attachInterrupt(button, , HIGH);
  sei();
}

void setDefaultPortValues() {
  digitalWrite(ledNet, 1);
}

void setRelay() {
  digitalWrite(ledNet, 0);
  if(currentTime - buttonLast > 30*60*1000 || buttonLast == 0) {
    if(currHumidity >= (setHumidity + 5)) {
      digitalWrite(relay, 1);
      Serial.println("Set ON");
    } else if(currHumidity < (setHumidity - 5)) {
      digitalWrite(relay, 0);
      Serial.println("Set OFF");
    }
  }
}

void buttonPress () {
  Serial.println("b1");
  digitalWrite(relay, digitalRead(relay) == 1 ? 0 : 1);
  buttonLast = millis();
  Serial.println("b1");
}

void loop() {
  currentTime = millis();
  if(currentTime < (prewTime + 20000)) {
    return;
  }
  prewTime = currentTime;
  
  currHumidity = dht.readHumidity();
  currTemp = dht.readTemperature();
  Serial.println("currHumidity=" + String(currHumidity));
  Serial.println("currTemp=" + String(currTemp));

  if (isnan(currHumidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

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
    String url = "http://iot.lyolek.dp.ua/services/device.php?";
    url += "GPIO14=" + String(currHumidity) + "&";
    url += "GPIO15=" + String(currTemp) + "&";
    url += "GPIO100=" + String(digitalRead(relay));
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
      setHumidity = parsed["GPIO12"];
      Serial.printf("GPIO12=%d\n", setHumidity);
      setRelay();
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

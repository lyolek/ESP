#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "Mega";
const char* password = "inter1017net";
const char* usr = "dev2";
const char* pwd = "dev2dev2";

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



  
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(14, INPUT_PULLUP);
  setDefaultPortValues();
}

void setDefaultPortValues() {
  digitalWrite(12, 0);
  digitalWrite(13, 0);
}

void loop() {

  // wait for WiFi connection
  if((WiFi.status() == WL_CONNECTED)) {

    HTTPClient http;
    Serial.println("[HTTP] start...");
    String url = "http://iot.lyolek.dp.ua/services/device.php?GPIO14=";
    url += digitalRead(14);
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
      int GPIO12 = parsed["GPIO12"];
//      int GPIO13 = parsed["GPIO13"];
      Serial.printf("GPIO12=%d\n", GPIO12);
//      Serial.printf("GPIO13=%d\n", GPIO13);
//      Serial.printf("GPIO14=%d\n", digitalRead(14));

      
      digitalWrite(12, GPIO12);
      digitalWrite(13, GPIO12);
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

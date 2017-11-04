#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "Mega";
const char* password = "inter1017net";
const char* usr = "dev1";
const char* pwd = "dev1dev1";

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


//  http.setReuse(true);

  
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, INPUT_PULLUP);
  setDefaultPortValues();
}

void setDefaultPortValues() {
  digitalWrite(0, 0);
  digitalWrite(2, 0);
  digitalWrite(4, 0);
}

void loop() {

  // wait for WiFi connection
  if((WiFi.status() == WL_CONNECTED)) {

    HTTPClient http;
    Serial.println("[HTTP] start...");
    String url = "http://iot.lyolek.dp.ua/services/device.php?GPIO05=";
    url += digitalRead(5);
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
      int GPIO02 = parsed["GPIO02"];
      int GPIO04 = parsed["GPIO04"];
      Serial.printf("GPIO02=%d\n", GPIO02);
      Serial.printf("GPIO04=%d\n", GPIO04);

      
      digitalWrite(0, 1);
      digitalWrite(2, GPIO02);
      digitalWrite(4, GPIO04);
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

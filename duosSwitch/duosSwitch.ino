#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "Mega";
const char* password = "inter1017net";
const char* usr = "dev4";
const char* pwd = "dev4dev4";
int currentValweState = -1;

void setup() {
//  Serial.begin(115200);
  Serial.begin(19200);
  Serial.setDebugOutput(true);
  Serial.println("Booting");

  WiFi.begin(ssid, password);

  String hostName = "esp8266:";
  hostName += usr;
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


//  http.setReuse(true);

  
  pinMode(16, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(1, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
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

void loop() {

  // wait for WiFi connection
  if((WiFi.status() == WL_CONNECTED)) {

    HTTPClient http;
    Serial.println("[HTTP] start...");
    String url = "http://iot.lyolek.dp.ua/services/device.php?GPIO01=";
    url += digitalRead(1);
    url += "&GPIO03=";
    url += digitalRead(3);
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
        valweSet(0);
        delay(5000);
        return;
      }
      int GPIO13 = parsed["GPIO13"];
      int GPIO16 = parsed["GPIO16"];
      Serial.printf("GPIO13=%d\n", GPIO13);
      Serial.printf("GPIO16=%d\n", GPIO16);

      
      digitalWrite(13, GPIO13);
      valweSet(GPIO16);
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        valweSet(0);
    }
    http.end();
  } else {
    valweSet(0);
  }
  delay(5000);
}

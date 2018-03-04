#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


const char* ssid = "Mega";
const char* password = "inter1017net";
const char* usr = "dev4";
const char* pwd = "dev4dev4";

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



  
  pinMode(15, OUTPUT);//green led (inverse)
}

void failHandler() {
  Serial.println("FAIL!!!");  
}

void loop() {

  // wait for WiFi connection
  if((WiFi.status() == WL_CONNECTED)) {

    HTTPClient http;

    Serial.println("[HTTP] start...");
    String url = "http://iot.lyolek.dp.ua/services/device.php";
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
      int GPIO15 = parsed["GPIO05"];
      Serial.printf("GPIO15=%d\n", GPIO15);

      if(GPIO15 == 1) {
        
        digitalWrite(15, 1);
        delay(10000);
        digitalWrite(15, 0);
        Serial.println("[HTTP] start...");
        String url = "http://iot.lyolek.dp.ua/services/setPort.php";
        http.begin(url);
    
        int httpCode = http.sendRequest("GET", "{\"devId\":\"2\", \"GPIO\":\"05\", \"state\":\"0\"}");
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        if(httpCode == 200) {
          Serial.println("[HTTP] done");
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            failHandler();
        }
      }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        failHandler();
    }



    
    http.end();
  } else {
    failHandler();
  }
  delay(1000);
}

/**************************************************************
 *
 * This sketch connects to a website and downloads a page.
 * It can be used to perform HTTP/RESTful API calls.
 *
 * For this example, you need to install ArduinoHttpClient library:
 *   https://github.com/arduino-libraries/ArduinoHttpClient
 *   or from http://librarymanager/all#ArduinoHttpClient
 *
 * TinyGSM Getting Started guide:
 *   http://tiny.cc/tiny-gsm-readme
 *
 **************************************************************/

// Select your modem:
#define TINY_GSM_MODEM_SIM800

// Increase RX buffer
#define TINY_GSM_RX_BUFFER 512

#include <SoftwareSerial.h>
SoftwareSerial SerialAT(5, 4); // RX, TX


// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "www.ab.kyivstar.net";
const char user[] = "";
const char pass[] = "";

// Name of the server we want to connect to
const char server[] = "iot.lyolek.dp.ua";
const int  port     = 80;
// Path to download (this is the bit after the hostname in the URL)
const char resource[] = "/services/device.php";

#include <TinyGsmClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

TinyGsm modem(SerialAT);

TinyGsmClient client(modem);

const char* usr = "dev4";
const char* pwd = "dev4dev4";

void setup() {
  // Set console baud rate
  Serial.begin(115200);
  delay(10);

  // Set GSM module baud rate
  
  Serial.println("Initializing modem...");
  SerialAT.begin(115200);
  delay(3000);


//  pinMode(13, OUTPUT);
//  pinMode(12, INPUT_PULLUP);
  setDefaultPortValues();
}

void setDefaultPortValues() {
//  digitalWrite(13, 0);
}

void loop() {

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.restart();

  String modemInfo = modem.getModemInfo();
  Serial.print("Modem: ");
  Serial.println(modemInfo);

  
  Serial.print(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");

  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail");
    delay(10000);
    return;
  }
  Serial.println(" OK");

/*
  HTTPClient http;
  Serial.println("[HTTP] start...");
  String url = "http://iot.lyolek.dp.ua/services/device.php?GPIO12=";
  url += digitalRead(12);
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
      int GPIO13 = parsed["GPIO13"];
      Serial.printf("GPIO13=%d\n", GPIO13);

      
      digitalWrite(13, GPIO13);
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        setDefaultPortValues();
    }
    http.end();

*/
  modem.gprsDisconnect();
  Serial.println("GPRS disconnected");

  // Do nothing forevermore
  delay(1000);
}


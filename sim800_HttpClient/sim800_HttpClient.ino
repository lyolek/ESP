 /**************************************************************

   This sketch connects to a website and downloads a page.
   It can be used to perform HTTP/RESTful API calls.

   For this example, you need to install ArduinoHttpClient library:
     https://github.com/arduino-libraries/ArduinoHttpClient
     or from http://librarymanager/all#ArduinoHttpClient

   TinyGSM Getting Started guide:
     http://tiny.cc/tiny-gsm-readme

 **************************************************************/
// Increase RX buffer
#define TINY_GSM_RX_BUFFER 512

// Select your modem:
#define TINY_GSM_MODEM_SIM800
// #define TINY_GSM_MODEM_SIM808
// #define TINY_GSM_MODEM_SIM900
// #define TINY_GSM_MODEM_A6
// #define TINY_GSM_MODEM_A7
// #define TINY_GSM_MODEM_M590
// #define TINY_GSM_MODEM_ESP8266

#include <ArduinoJson.h>
#include <TinyGsmClient.h>
#include <SoftwareSerial.h>
#include <ArduinoHttpClient.h>


SoftwareSerial SerialAT(5, 4); // RX, TX


// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "www.ab.kyivstar.net";
//const char apn[]  = "";
//const char apn[]  = "internet";
const char user[] = "";
const char pass[] = "";

const char* usr = "dev4";
const char* pwd = "dev4dev4";

// Name of the server we want to connect to
const char server[] = "iot.lyolek.dp.ua";
const int  port     = 80;

int cntGood = 0;
int cntFail = 0;
int cntFailConst = 0;
boolean isGprsConnected = false;

TinyGsm modem(SerialAT);

TinyGsmClient client(modem);
HttpClient http(client, server, port);

void setup() {
  // Set console baud rate
  Serial.begin(19200);
  delay(10);

  // Set GSM module baud rate

  Serial.println("Initializing modem...");
  SerialAT.begin(19200);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.init();




  pinMode(14, OUTPUT);
  pinMode(12, INPUT_PULLUP);
  setDefaultPortValues();
}

void setDefaultPortValues() {
  digitalWrite(14, 0);
}

void handleError(boolean isGprsConnected) {
  setDefaultPortValues();
  cntFail++;
  cntFailConst++;
  Serial.printf("cntFail=%d\n", cntFail);
  Serial.printf("cntFailConst=%d\n", cntFailConst);
  if(cntFailConst < 5) {
    if(isGprsConnected){
      Serial.println("gprsDisconnect()");
      modem.gprsDisconnect();
      Serial.println("done");
    } else {
      Serial.println("gprs is not connected");
    }
  } else {
    Serial.println("restart()");
    modem.restart();
    delay(10000);
    cntFailConst = 0;
  }
}

void loop() {


  Serial.printf("getSignalQuality()=%d\n", modem.getSignalQuality());
  
  Serial.print(F("Waiting for network..."));
  if (!modem.waitForNetwork()) {
    Serial.println(" fail waitForNetwork()");
    delay(1000);
    handleError(false);
    return;
  }
  Serial.println(" OK");
  

  Serial.print(F("Connecting to "));
  Serial.print(apn);
  if (!modem.gprsConnect(apn, user, pass)) {
    Serial.println(" fail gprsConnect()");
    delay(1000);
    handleError(false);
    return;
  }
  Serial.println(" OK");







  String url = "/services/device.php?GPIO12=";
  url += digitalRead(12);

  //  Serial.println("Performing HTTP GET request... ");
  http.beginRequest();
  int err = http.get(url);
  http.sendBasicAuth(usr, pwd); // send the username and password for authentication
  http.endRequest();
  if (err != 0) {
    Serial.println("failed to connect");
    delay(1000);
    handleError(true);
    return;
  }

  int status = http.responseStatusCode();
  Serial.println(status);
  if (status < 0) {
    delay(1000);
    handleError(true);
    return;
  }

  while (http.headerAvailable()) {
    String headerName = http.readHeaderName();
    String headerValue = http.readHeaderValue();
    //Serial.println(headerName + " : " + headerValue);
  }

  int length = http.contentLength();
  if (length >= 0) {
    //    Serial.println(String("Content length is: ") + length);
  }
  if (http.isResponseChunked()) {
    //    Serial.println("This response is chunked");
  }

  String body = http.responseBody();
  //  Serial.println("Response:");
  Serial.println(body);


  char JSONMessage[body.length() + 1];
  body.toCharArray(JSONMessage, body.length() + 1);
  Serial.println(JSONMessage);
  StaticJsonBuffer<300> JSONBuffer;   //Memory pool
  JsonObject& parsed = JSONBuffer.parseObject(JSONMessage); //Parse message
  if (!parsed.success()) {   //Check for errors in parsing

    Serial.println("Parsing failed");
    delay(1000);
    handleError(true);
    return;
  }
  int GPIO14 = parsed["GPIO14"];
  Serial.printf("GPIO14=%d\n", GPIO14);


  digitalWrite(14, GPIO14);
  cntGood++;
  Serial.println(String("Good: ") + cntGood);
  Serial.println(String("Fail: ") + cntFail);

  //  Serial.println(String("Body length is: ") + body.length());

  // Shutdown

  http.stop();

  Serial.println(modem.getLocalIP());
  modem.gprsDisconnect();
  Serial.println("GPRS disconnected");

  // Do nothing forevermore

  delay(60000);
}


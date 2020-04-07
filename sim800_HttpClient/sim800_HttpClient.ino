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

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>


#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
//Adafruit_SSD1306 display();

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

  display.begin(SSD1306_SWITCHCAPVCC);


  // Set GSM module baud rate

  Serial.println("Initializing modem...");
  SerialAT.begin(9600);
  delay(3000);

  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  Serial.println("Initializing modem...");
  modem.init();




  pinMode(15, OUTPUT);
  digitalWrite(15, 1);
  pinMode(14, OUTPUT);
  //  pinMode(12, INPUT_PULLUP);
  setDefaultPortValues();

}

void restartModem() {
  Serial.println(" restartModem()");
  writeString(0, "gprsDisconnect()");
  modem.gprsDisconnect();
  delay(1000);
  Serial.println("Hard modem RST");
  writeString(0, "Hard modem RST");
  digitalWrite(15, 0);
  delay(1000);
  digitalWrite(15, 1);
  delay(1000);
  Serial.println("restart()");
  writeString(0, "restart()");
  modem.restart();
  delay(10000);
}

void setDefaultPortValues() {
  //  digitalWrite(14, 0);
}

void clearString(int i) {
  display.setColor(BLACK);
  display.fillRect(0, i * 10 + 2, 128, 10);
  display.setColor(WHITE);
  display.display();
}
void writeString(int i, String s) {
  clearString(i);
  display.drawString(0, 0 + i * 10, s);
  display.display();
}

void handleError() {
  Serial.println("handleError(" + String(cntFailConst) + ")");
  writeString(0, "Handle error(" + String(cntFailConst) + ")");
  cntFail++;
  cntFailConst++;

  setDefaultPortValues();
  Serial.println("cntFail=");
  Serial.println("cntFailConst=");
  if (cntFailConst < 5
     ) {
    //    if(isGprsConnected){
    //      Serial.println("gprsDisconnect()");
    //      modem.gprsDisconnect();
    //      Serial.println("done");
    //    } else {
    //      Serial.println("gprs is not connected");
    //    }
  } else {
    restartModem();
    cntFailConst = 0;
  }
}

void loop() {
  Serial.println("loop()");

  //  display.clear();
  //  display.display();


  if(!modem.isNetworkConnected()) {
    writeString(0, "Waiting for network(" + String(cntFailConst) + ")...");
    Serial.print("Waiting for network(" + String(cntFailConst) + ")...");
    if (!modem.waitForNetwork()) {
      Serial.println(" fail waitForNetwork()");
      delay(1000);
      handleError();
      return;
    }
  }
  Serial.println(" OK");
  clearString(0);


  String signalQ = String(modem.getSignalQuality());
  writeString(1, String("RSI:") + signalQ);
  Serial.println(signalQ);

  if (!modem.isGprsConnected()) {
    writeString(0, "Connecting...");
    Serial.println("Connecting...");
    if (!modem.gprsConnect(apn, user, pass)) {
      Serial.println(" fail gprsConnect()");
      delay(1000);
      handleError();
      return;
    }
  }
  String localIP = modem.getLocalIP();
  writeString(0, "IP:" + localIP);
  Serial.println("IP:" + localIP);






  String url = "/services/device.php?GPIO140=1";
//  url += digitalRead(12);

  //  Serial.println("Performing HTTP GET request... ");
  http.beginRequest();
  int err = http.get(url);
  http.sendBasicAuth(usr, pwd); // send the username and password for authentication
  http.endRequest();
  if (err != 0) {
    Serial.println("failed to connect");
    delay(1000);
    //    handleError(true);
    return;
  }

  int status = http.responseStatusCode();
  Serial.println(status);
  writeString(1, "Responce code:" + String(status));
  if (status < 0) {
    delay(1000);
    //    handleError(true);
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
    //    handleError(true);
    return;
  }
  int GPIO14 = parsed["GPIO14"];
  Serial.printf("GPIO14=%d\n", GPIO14);

  writeString(2, "GPIO14:" + String(GPIO14));

  digitalWrite(14, GPIO14);
  cntGood++;
  cntFailConst = 0;
  Serial.println(String("Good: ") + cntGood);
  Serial.println(String("Fail: ") + cntFail);

  //  Serial.println(String("Body length is: ") + body.length());

  // Shutdown

  http.stop();



  delay(5000);

  //  writeString(0, "Disconnecting...");
  //  Serial.print("Disconnecting...");
  //  modem.gprsDisconnect();
  //  writeString(0, "Disconnected");
  //  Serial.print("Disconnected");
  //  delay(5000);

}


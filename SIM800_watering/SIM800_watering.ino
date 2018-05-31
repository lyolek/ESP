#include <GSMSim.h>
#include "SSD1306.h"
#include <ESP8266WiFi.h>

#define RX 4
#define TX 5
#define RESET 15
#define BAUD 9600


//GSMSim gsm;
//GSMSim gsm(RX, TX);
GSMSim gsm(RX, TX, RESET);
//GSMSim gsm(RX, TX, RESET, LED_PIN, LED_FLAG);

SSD1306  display(0x3c, 12, 13);

char* MASTER_PHONE = "+380980912097";


String readSerial() {
  uint64_t timeOld = millis();
  while (!gsm.available() && !(millis() > timeOld + 5000)) {
    delay(13);
  }
  delay(100);
  String str = "";
  while (gsm.available()) {
      str += (char) gsm.read();
  }
  return str;
}

void setup() {
  
  Serial.begin(9600);

  Serial.println("GSMSim Library - Call Example");
  Serial.println("");
  delay(10000);

  gsm.start(); // baud default 9600
  //gsm.start(BAUD);

  char* phone_no = MASTER_PHONE;

  Serial.println("Calling the number " + String(phone_no));
  Serial.println(gsm.call(phone_no));
  delay(10000);
  Serial.println("Call hang off");
  gsm.callHangoff();
  delay(1000);
  Serial.println(gsm.callActivateListCurrent(true));
  

  

  WiFi.mode(WIFI_STA);
}

void loop() {
//  int callStatus = gsm.callStatus();
//  delay(3000);
//  Serial.println("-" + String(callStatus));
//  if(callStatus == 3) {
    String resp = readSerial();
    Serial.println("=" + resp);
    if(resp.indexOf("+CLIP:") != -1 && resp.indexOf("\"+") != -1){
      String number = resp.substring(resp.indexOf("\"+") + 1, resp.indexOf("\"+") + 14);
      Serial.println("#" + number);
      gsm.callHangoff();
      delay(3000);
      if(number == String(MASTER_PHONE)) {
        Serial.println("#master");
        char* msg = "Arduino is";
        gsm.smsTextMode(true);
        delay(1000);
        Serial.println(gsm.smsSend(MASTER_PHONE, msg));
      }
//    }
  }
}

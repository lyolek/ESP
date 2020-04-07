#include <SoftwareSerial.h>
#include <Base64.h>
 
//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 3
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 2

SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

void setup() {
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
   
  //Being serial communication witj Arduino and SIM800
  
  serialSIM800.begin(9600);
  delay(1000);
   
  Serial.println("Setup Complete!");

  char inputString[] = "dev4:dev4dev4";
  int inputStringLength = sizeof(inputString);
  int encodedLength = Base64.encodedLength(inputStringLength);
  char encodedString[encodedLength];
  Base64.encode(encodedString, inputString, inputStringLength);
  Serial.println(encodedString);
}

void loop() {
//  delay(50);
  //Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
  while(serialSIM800.available()){
    Serial.write(serialSIM800.read());
  }
  //Read Arduino IDE Serial Monitor inputs (if available) and send them to SIM800
  while(Serial.available()){
    serialSIM800.write(Serial.read());
  }

}

#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <MemoryFree.h>

//unsigned long loopStartTime;
//unsigned long loopWorkTime;

#define SIM800_TX 11
#define SIM800_RX 10
#define RESET_PIN A2
#define SENSOR1_PIN A0
#define SENSOR2_PIN A1
#define NET_PIN 9

SoftwareSerial GSMport(SIM800_TX, SIM800_RX);


String respAT = "";
int cntFailConst = 0;
float setHumidity[] = {450, 450};
unsigned int measuredVal[] = {0, 0};
int wateringDuration = 5;
int MOTOR_PIN[] = {3,7};
unsigned long lastWateringTime[] = {0, 0};
unsigned int wateringSwitchOn[] = {0, 0};
unsigned long unwaterPause = 300000;

void setup() {
  Serial.begin(9600);
  GSMport.begin(9600);
  delay(1000);
  
  pinMode(SENSOR1_PIN, INPUT);
  pinMode(SENSOR2_PIN, INPUT);
  pinMode(RESET_PIN, OUTPUT);
  pinMode(NET_PIN, OUTPUT);
  pinMode(MOTOR_PIN[0], OUTPUT);
  pinMode(MOTOR_PIN[1], OUTPUT);
  
  analogWrite(RESET_PIN, 1024);

  restartModem();
}

void setupModem() {
  sendAT("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  sendAT("AT+SAPBR=3,1,\"APN\",\"www.ab.kyivstar.net\"");
  sendAT("AT+SAPBR=3,1,\"USER\",\"\"");
  sendAT("AT+SAPBR=3,1,\"PWD\",\"\"");
}

void disconnectModem() {
  sendAT("AT+HTTPTERM");
//  printArr(respAT);
  sendAT("AT+SAPBR=0,1");
//  printArr(respAT);
}
String sendAT(String cmd) {
  return sendAT(cmd, "OK");
}
String sendAT(String cmd, String resp) {
  unsigned long startTime;
  startTime = millis();
  GSMport.println(cmd);
  String s;
  String ss;
  int i = 0;
  do {
    s = readATString();
    if(s.length() >0) {
      ss += s + "\n";
//      Serial.println("->" + s);
    }
//    Serial.print(".");
  } while(!s.startsWith(resp) && !s.equals("ERROR") && millis() - startTime < 30000);
  return ss;
}

String readATString(){
  unsigned long startTime;
  startTime = millis();
  char c;
  String v;
  while (true && millis() - startTime < 30000) {  //сохраняем входную строку в переменную v
    if(GSMport.available()){
      c = GSMport.read();
//Serial.println(c);
      if(c == 13 || c == 10){
        return v;
      }
      v += c;
    }
  }
}


void restartModem() {
  Serial.println(" restartModem()");
  disconnectModem();
  delay(1000);
  Serial.print("Hard modem RST...");
  analogWrite(RESET_PIN, 0);
  for(int i = 0; i < 10; i++) {
    digitalWrite(NET_PIN, !digitalRead(NET_PIN));
    delay(500);
  }
  analogWrite(RESET_PIN, 1024);
  Serial.println(" Done");
  delay(5000);
  setupModem();
}

void handleError() {
  digitalWrite(NET_PIN, 0);
  Serial.println("handleError(" + String(cntFailConst) + ")");
  cntFailConst++;

  if (cntFailConst >= 5) {
    restartModem();
    cntFailConst = 0;
  }
  delay(5000);
}

String getHTTPResponce (String s){
//  Serial.println(s);

  String body = "";
  if(s.indexOf("+HTTPREAD: ") >= 0) {
    int hdr = s.indexOf(char(10), s.indexOf("+HTTPREAD: "));
    int bodyLength = s.substring(s.indexOf("+HTTPREAD: ") + 11, hdr).toInt();
//    Serial.println("=" + String(bodyLength) + "=");
    body = s.substring(hdr + 1, hdr + 1 + bodyLength);
  } else {
    return "ERROR";
  }
  return body;
}


void setRelay(int n) {
  Serial.println(millis() - lastWateringTime[n]);
  if(measuredVal[n] > setHumidity[n] && millis() - lastWateringTime[n] > unwaterPause) {
    Serial.println("Watering " + String(n));
    digitalWrite(MOTOR_PIN[n], 1);
    delay(wateringDuration*1000);
    digitalWrite(MOTOR_PIN[n], 0);
    lastWateringTime[n] = millis();
    wateringSwitchOn[n]++;
  }
}

void loop() {
  unsigned long loopStartTime = millis();
  
  Serial.println("loop()");
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
  int numTrys = 50;
  measuredVal[0] = 0;
  measuredVal[1] = 0;
  for(int i = 0; i < numTrys; i++) {
    measuredVal[0] += analogRead(SENSOR1_PIN);
    measuredVal[1] += analogRead(SENSOR2_PIN);
    digitalWrite(NET_PIN, !digitalRead(NET_PIN));
    delay(100);
  }
  measuredVal[0] = measuredVal[0]/numTrys;
  measuredVal[1] = measuredVal[1]/numTrys;
  Serial.println("measuredVal1=" + String(measuredVal[0]));
  Serial.println("measuredVal2=" + String(measuredVal[1]));
  
  sendAT("AT+SAPBR=1,1");
  sendAT("AT+HTTPINIT");
  sendAT("AT+HTTPPARA=\"CID\",1");
  sendAT("AT+HTTPPARA=\"URL\",\"http://iot.lyolek.dp.ua/services/device.php?GPIO140=" + String(measuredVal[0]) + "&GPIO150=" + String(measuredVal[1]) + "&GPIO141=" + String(wateringSwitchOn[0]) + "&GPIO151=" + String(wateringSwitchOn[1]) + "\"");
  Serial.println("----------");
  sendAT("AT+HTTPPARA=\"USERDATA\",\"Authorization: Basic ZGV2NDpkZXY0ZGV2NAA=\"");
  Serial.println("----------");
  sendAT("AT+HTTPACTION=0", "+HTTPACTION:");
  Serial.println("----------");
  respAT = sendAT("AT+HTTPREAD");
  Serial.println("----------");
  respAT = getHTTPResponce(respAT);
  Serial.println(respAT);
  char JSONMessage[respAT.length() + 1];
  respAT.toCharArray(JSONMessage, respAT.length() + 1);
//  Serial.println(JSONMessage);
  StaticJsonBuffer<200> JSONBuffer;   //Memory pool
  JsonObject& parsed = JSONBuffer.parseObject(JSONMessage); //Parse message
  if (!parsed.success()) {   //Check for errors in parsing
    Serial.println("Parsing failed");
    handleError();
    return;
  }

  
  digitalWrite(NET_PIN, 1);
  cntFailConst = 0;
  wateringSwitchOn[0] = 0;
  wateringSwitchOn[1] = 0;

  setHumidity[0] = parsed["GPIO14"];
  setHumidity[1] = parsed["GPIO15"];
  wateringDuration = parsed["GPIO16"];
  Serial.println("setHumidity1=" + String(setHumidity[0]));
  Serial.println("setHumidity2=" + String(setHumidity[1]));
  Serial.println("wateringDuration=" + String(wateringDuration));
  disconnectModem();
  setRelay(0);
  setRelay(1);
  

  unsigned long loopWorkTime = millis() - loopStartTime;
  if(loopWorkTime < 20000) {
    delay(20000 - loopWorkTime);
  }
}


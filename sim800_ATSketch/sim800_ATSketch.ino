#include <SoftwareSerial.h>
SoftwareSerial GSMport(3, 2); // RX, TX

void setup() {
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
   
  //Being serial communication witj Arduino and SIM800
  
  GSMport.begin(9600);
  delay(1000);
   
  Serial.println("Setup Complete!");

  sendAT("AT");
  sendAT("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"");
  sendAT("AT+SAPBR=3,1,\"APN\",\"www.ab.kyivstar.net\"");
  sendAT("AT+SAPBR=3,1,\"USER\",\"\"");
  sendAT("AT+SAPBR=3,1,\"PWD\",\"\"");

}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(100);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(100);   
  Serial.println("loop()");
  sendAT("AT+SAPBR=1,1");
  sendAT("AT+HTTPINIT");
  sendAT("AT+HTTPPARA=\"CID\",1");
  sendAT("AT+HTTPPARA=\"URL\",\"http://iot.lyolek.dp.ua/services/device.php?GPIO140=1\"");
  sendAT("AT+HTTPPARA=\"USERDATA\",\"Authorization: Basic ZGV2NDpkZXY0ZGV2NAA=\"");
  sendAT("AT+HTTPACTION=0");
  delay(5000);
  sendAT("AT+HTTPREAD");
  sendAT("AT+HTTPTERM");
  sendAT("AT+SAPBR=0,1");
  
  delay(600000);
}


String sendAT(String cmd) {
  return sendAT(cmd, "OK");
}
String sendAT(String cmd, String resp) {
  unsigned long startTime;
  startTime = millis();
  Serial.println(cmd);
  GSMport.println(cmd);
  String s;
  do {
    s = readATString();
    if(s.length() >0) {
      Serial.println("->" + s);
    }
    Serial.print(".");
  } while(!s.equals(resp) && !s.equals("ERROR") && millis() - startTime < 10000);
  Serial.println("finAT");
  return "";
}

String readATString(){
    int c;
    String v;
    while (true) {  //сохраняем входную строку в переменную v
      if(GSMport.available()){
        c = GSMport.read();
//        Serial.println(c);
        if(c == 13 || c == 10){
          return v;
        }
        v += char(c);
      }
    }
}
/*
void gprs_init() {  //Процедура начальной инициализации GSM модуля
  int d = 500;
  int ATsCount = 7;
  String ATs[] = {  //массив АТ команд
    "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"",  //Установка настроек подключения
    "AT+SAPBR=3,1,\"APN\",\"www.ab.kyivstar.net\"",
    "AT+SAPBR=3,1,\"USER\",\"\"",
    "AT+SAPBR=3,1,\"PWD\",\"\"",
    "AT+SAPBR=1,1",  //Устанавливаем GPRS соединение
    "AT+HTTPINIT",  //Инициализация http сервиса
    "AT+HTTPPARA=\"CID\",1"  //Установка CID параметра для http сессии
  };
  int ATsDelays[] = {6, 1, 1, 1, 3, 3, 1}; //массив задержек
  Serial.println("GPRG init start");
  for (int i = 0; i < ATsCount; i++) {
    Serial.println(ATs[i]);  //посылаем в монитор порта
    GSMport.println(ATs[i]);  //посылаем в GSM модуль
    delay(d * ATsDelays[i]);
    Serial.println(ReadGSM());  //показываем ответ от GSM модуля
    delay(d);
  }
  Serial.println("GPRG init complete");
}

void gprs_send(String data) {  //Процедура отправки данных на сервер
  //отправка данных на сайт
  int d = 400;
  Serial.println("Send start");
  Serial.println("setup url");
  GSMport.println("AT+HTTPPARA=\"URL\",\"http://www.google.com\"");
  delay(d * 2);
  Serial.println(ReadGSM());
  delay(d);
  Serial.println("GET url");
  GSMport.println("AT+HTTPACTION=0");
  delay(d * 2);
  Serial.println(ReadGSM());
  delay(d);
  Serial.println("Send done");
}
*/


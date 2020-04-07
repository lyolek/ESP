#include <SoftwareSerial.h>

int input;
String s;

SoftwareSerial RFIDSerial(12, 13); // RX, TX

void setup() {
  // Очищаем буфер
  Serial.flush();
  // Устанавливаем скорость работы с портом
  Serial.begin(9600);
  // Устанавливаем скорость работы с модулем RDM630
  RFIDSerial.begin(9600);
  s="";
}

void loop() {
  // если есть данные, то
  if (RFIDSerial.available() > 0) {
    // читаем блок данных с модуля RDM630 и заносим их в переменную input
    input = RFIDSerial.read();
    // присваиваем все считанные в переменную input значения переменной s, так как за 1 цикл loop мы получаем 1 значение, а их 12
    s+=input;  // то же самое, что и s=s+input;
    // если длинна кода равна 26 символам (в памяти чипа можно разместить 26 байт информации), то
    if (s.length()==26) {
      // выводим данные
      Serial.println(s);
      // очищаем переменную
      s="";
    }
  }
}

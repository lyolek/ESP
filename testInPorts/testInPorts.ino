
void setup() {
  Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY);
  Serial.setDebugOutput(true);
  Serial.println("Booting");
  delay(1000);


//  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT);
//  pinMode(2, INPUT);
//  pinMode(3, INPUT);
//  pinMode(4, INPUT);
//  pinMode(5, INPUT);
//  pinMode(6, OUTPUT);
//  pinMode(7, OUTPUT);
//  pinMode(8, OUTPUT);
  pinMode(9, INPUT);
//  pinMode(10, OUTPUT);
//  pinMode(11, OUTPUT);
//  pinMode(12, INPUT);
//  pinMode(13, INPUT);
//  pinMode(14, INPUT);
//  pinMode(15, INPUT);
//  pinMode(16, INPUT);

}

void loop() {
  Serial.println("New loop");
    Serial.print(9);
    Serial.print("=");
    Serial.println(digitalRead(9));
    Serial.print(1);
    Serial.print("=");
    Serial.println(digitalRead(1));
/*
  for(int i = 0; i <= 16; i++) {
    Serial.print(i);
    Serial.print("=");
    Serial.println(digitalRead(i));
  }
*/
  delay(1000);
}

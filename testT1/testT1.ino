
void setup() {
  Serial.begin(115200);
//  Serial.begin(115200,SERIAL_8N1,SERIAL_TX_ONLY);
  Serial.setDebugOutput(true);
  Serial.println("Booting");
  delay(1000);


  pinMode(0, INPUT_PULLUP);
//  pinMode(1, INPUT);
//  pinMode(2, INPUT);
//  pinMode(3, INPUT);
//  pinMode(4, INPUT);
  pinMode(5, OUTPUT);
//  pinMode(6, OUTPUT);
//  pinMode(7, OUTPUT);
//  pinMode(8, OUTPUT);
  pinMode(9, INPUT_PULLUP);
//  pinMode(10, OUTPUT);
//  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, INPUT);
//  pinMode(14, INPUT);
//  pinMode(15, INPUT);
//  pinMode(16, INPUT);

}

void loop() {
/*
  Serial.println("New loop");
    Serial.print(9);
    Serial.print("=");
    Serial.println(digitalRead(9));
    Serial.print(1);
    Serial.print("=");
    Serial.println(digitalRead(1));


    for(int j = 0; j <= 3; j++) {
      digitalWrite(12, HIGH);
      delay(200);
      digitalWrite(12, LOW);
      delay(200);
    }
*/
  digitalWrite(12, digitalRead(0));
  digitalWrite(5, digitalRead(9));
  delay(100);
}

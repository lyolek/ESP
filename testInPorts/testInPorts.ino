
void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Booting");


    pinMode(0, INPUT_PULLUP);
//  pinMode(1, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
//  pinMode(6, OUTPUT);
//  pinMode(7, OUTPUT);
//  pinMode(8, OUTPUT);
//  pinMode(9, OUTPUT);
//  pinMode(10, OUTPUT);
//  pinMode(11, OUTPUT);
  pinMode(12, INPUT_PULLUP);
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(15, INPUT_PULLUP);
  pinMode(16, INPUT_PULLUP);
}

void loop() {
  Serial.println("New loop");

  for(int i = 0; i <= 16; i++) {
    Serial.println(i);
    Serial.println(digitalRead(i));
  }
  delay(1000);
}

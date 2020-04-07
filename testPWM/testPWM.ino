void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("Booting");
  
  pinMode(5, OUTPUT);

}

void loop() {
  for(int i = 0; i < 1024; i++) {
    delay(10);
    analogWrite(5, i); 
  }

}

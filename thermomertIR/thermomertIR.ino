#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
Adafruit_SSD1306 display = Adafruit_SSD1306();
float vBat = 0;

void setup() {
  Serial.begin(9600);
Serial.println("---");

//  mlx.begin();

  display.begin(SSD1306_SWITCHCAPVCC, 0x3c);  // initialize with the I2C addr 0x3C (for the 128x32)
  // init done

Serial.println("1");
  display.display();
  
Serial.println("2");
  display.setTextColor(WHITE);
  pinMode(13, INPUT_PULLUP);
}

void loop() {
Serial.println(digitalRead(13));

  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(3);
  if(digitalRead(13)) {
Serial.println("C");
//Serial.println(String(mlx.readObjectTempC()) + "C");
    display.print(String(mlx.readObjectTempC()) + "C");
    display.setCursor(0,30);
    display.setTextSize(2);
    display.print(String(mlx.readAmbientTempC()) + "C");
  } else {
Serial.println("V");
    vBat = analogRead(A0);
Serial.println(String(vBat) + "V");
    vBat = vBat/1024;
Serial.println(String(vBat) + "V");
    vBat = vBat*5;
Serial.println(String(vBat) + "V");
    display.print(String(vBat) + "V");
  }
  display.display();
//  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempC()); 
//  Serial.print("*C\tObject = "); Serial.print(mlx.readObjectTempC()); Serial.println("*C");
//  Serial.print("Ambient = "); Serial.print(mlx.readAmbientTempF()); 
//  Serial.print("*F\tObject = "); Serial.print(mlx.readObjectTempF()); Serial.println("*F");
//  Serial.println();

  delay(500);
}

#include <Wire.h>

int mellomsteg = 6;
int mesketank = 7;
int mesketemp;
bool mellomstegTom = false;
bool mesketankTom = false;

void setup() {

  Wire.begin(9);
  Serial.begin(9600);
  pinMode(mellomsteg, INPUT_PULLUP);
  pinMode(mesketank, INPUT_PULLUP);
}

void loop() {
  mesketemp = analogRead(0);
  mellomstegTom = digitalRead(mellomsteg);
  mesketankTom = digitalRead(mesketank);
  digitalWrite(13, mellomstegTom);
  
  Wire.beginTransmission(8); // transmit to device #8
  Wire.write(mesketemp);
  Wire.write(mellomstegTom);        // sends five bytes
  Wire.write(mesketankTom);              // sends one byte
  Wire.endTransmission();    // stop transmitting


  delay(500);
}

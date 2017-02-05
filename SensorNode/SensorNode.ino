#include <Wire.h>

int mellomsteg = 6;
int mesketank = 7;
int mesketemp;
bool mellomstegTom = false;
bool mesketankTom = false;

void sendI2c(){
  Wire.write(mesketemp);
  Wire.write(mellomstegTom);       
  Wire.write(mesketankTom);              
}

void setup() {

  Wire.begin(9);
  Serial.begin(9600);
  pinMode(mellomsteg, INPUT_PULLUP);
  pinMode(mesketank, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  Wire.onRequest(sendI2c);
}

void loop() {
  
  mesketemp = analogRead(0);
  mellomstegTom = digitalRead(mellomsteg);
  mesketankTom = digitalRead(mesketank);
  
  if(mellomstegTom){
    digitalWrite(13, HIGH);
  } 
  else{
    digitalWrite(13, LOW);
  }

}

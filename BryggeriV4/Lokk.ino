byte lokkButton; //Trykknapp for lokket
byte lokkOldbutton = 1;
byte lokkButtonpin = 37; //pin 1
int lokkPin = 38;

void LokkInit(){
  pinMode(lokkPin, OUTPUT);
  pinMode(lokkButtonpin, INPUT_PULLUP);
  Lokk = EEPROM.read(0);
  if (Lokk) {
    digitalWrite(lokkPin, LOW);
  }
  else{
    digitalWrite(lokkPin, HIGH);
  }
}

void lokk() {
  lokkButton = digitalRead(lokkButtonpin);

  if (lokkButton && !lokkOldbutton) {
    if (Lokk == false) {
      Lokk = true;
       EEPROM.write(0, Lokk);
    }
    else {
      Lokk = false;
       EEPROM.write(0, Lokk);
    }
    lokkOldbutton = 1;
  }

  else if (!lokkButton && lokkOldbutton) {
    lokkOldbutton = 0;
  }

  if (Lokk) {
    digitalWrite(lokkPin, LOW);
  }
  if (Lokk == false) {
    digitalWrite(lokkPin, HIGH);
  }
}

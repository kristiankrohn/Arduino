byte lokkButton; //Trykknapp for lokket
byte lokkOldbutton = 1;
byte lokkButtonpin = 28;
int lokkPower = 29; 
int lokkPin = 30;

void LokkInit(){
  pinMode(lokkPin, OUTPUT);
  pinMode(lokkPower, OUTPUT);
  pinMode(lokkButtonpin, INPUT_PULLUP);
  Lokk = EEPROM.read(0);
  if (Lokk) {
    digitalWrite(lokkPin, LOW);
  }
  else{
    digitalWrite(lokkPin, HIGH);
  }
  digitalWrite(lokkPower, LOW);
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


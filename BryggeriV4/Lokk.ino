byte lokkButton; //Trykknapp for lokket
byte lokkOldbutton = 1;
byte lokkButtonpin = 37; //pin 1
int lokkPin = 38;

void LokkInit(){
  pinMode(lokkPin, OUTPUT);
  digitalWrite(lokkPin, HIGH);
  pinMode(lokkButtonpin, INPUT_PULLUP);
}

void lokk() {
  lokkButton = digitalRead(lokkButtonpin);

  if (lokkButton && !lokkOldbutton) {
    if (Lokk == false) {
      Lokk = true;
    }
    else {
      Lokk = false;
    }
    lokkOldbutton = 1;
    //Serial.println("pressed");
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

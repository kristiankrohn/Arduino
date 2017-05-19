byte button; //Trykknapp for pumpa
byte oldbutton = 1;
byte buttonpin = 3;
int pumpePin = 35;
unsigned long pumpeStartTime;


void PumpeInit(){
    pinMode(buttonpin, INPUT_PULLUP); //Pumpeknapp
    pinMode(pumpePin, OUTPUT);
    digitalWrite(pumpePin, HIGH);
}

void init_pulsepumpe() {
  pumpeStartTime = millis();
}

void pulsepumpe() {

  if ((Now - pumpeStartTime) > 10000) { //time to shift the Relay Window
    pumpeStartTime += 10000;
  }
  if (6000 > Now - pumpeStartTime) {
    Pumpe = true;
  }

  else {
    Pumpe = false;
  }
}

void pumpe() {
  button = digitalRead(buttonpin);

  if (button && !oldbutton) {
    if (Pumpe == false) {
      Pumpe = true;
    }
    else {
      Pumpe = false;
    }
    oldbutton = 1;
  }
  else if (!button && oldbutton)
  {
    oldbutton = 0;
  }

  if (Pumpe) {
    digitalWrite(pumpePin, LOW);
  }
  else if (Pumpe == false) {
    digitalWrite(pumpePin, HIGH);
  }
}

int getPumpCurrent() {
  int c = analogRead(0);
  return c;
}

byte button; //Trykknapp for pumpa
byte oldbutton = 1;
byte buttonpin = 3;
int pumpePin = 31;
int currentPin = 4;
unsigned long pumpeStartTime;

const int numReadings = 200;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average


void PumpeInit() {
  pinMode(buttonpin, INPUT_PULLUP); //Pumpeknapp
  pinMode(pumpePin, OUTPUT);
  digitalWrite(pumpePin, HIGH);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
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
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(currentPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;


  //int RawValue = analogRead(currentPin);
  float Current = (average * 5.0 ) / 1.024;
  return int(Current);
}


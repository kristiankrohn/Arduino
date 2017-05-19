unsigned long varmeStartTime;
int varmePin = 36;

void TempInit(){
  pinMode(varmePin, OUTPUT);  // Varmelement
  digitalWrite(varmePin, HIGH);
  varmeStartTime = 0;
  MeskSet=celciusToAnalog(67);
}

int koktemp() {
  int temp1, temp2, tempsnitt, temp;
  temp1 = analog_koktopptemp;
  temp2 = analog_kokbunntemp;

  tempsnitt = (temp1 + temp2) / 2;

  temp = tempsnitt;

  return temp;
}

int Setpunkt() {
  int Setpunkt;
  int meskset;
  int k = 1024;

  meskset = MeskSet + 20;
  striketemp = MeskSet + 20;

  if ((Steg == 1) && (koketankvolum > 35)) {
    Setpunkt = striketemp;
  }

  if (Steg == 2) {
    Setpunkt = striketemp;
  }

  else if (Steg == 5) {
    Setpunkt = meskset;
  }

  else if ((Steg == 11) || (Steg == 12)) {
    Setpunkt = k;
  }

  else if (Steg == 0) {
    if (mankok) {
      Setpunkt = man_koktemp;
    }

    else if (manmesk) {
      Setpunkt = man_mesktemp + 20;
    }
  }
  else {
    Setpunkt = 0;
  }
  return Setpunkt;
}

void init_varmereg() {
  varmeStartTime = millis();
}

void varmeReg() {
  if (Steg != 0) {
    if ((Now - varmeStartTime) > 10000) { //time to shift the Relay Window
      if (Setpoint >= Input) {
        digitalWrite(varmePin, LOW); // Aktiv lav
      }
      else {
        digitalWrite(varmePin, HIGH);
      }
      varmeStartTime += 10000;
    }
  }
  else {
    digitalWrite(varmePin, HIGH);
  }
}

void getAnalogdata() {
  //analog_mesktemp = analogRead(0); moved this sensor to sensornode

  //Wire.requestFrom(9, 2);
  //analog_mesktemp = Wire.read();
  //analog_mesktemp |= (Wire.read() << 8);
  analog_koktopptemp = analogRead(1);
  analog_kokbunntemp = analogRead(2);

  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    unsigned char len = 0;
    CAN.readMsgBuf(&len, CANbuf);
    analog_mesktemp = CANbuf[0];
    analog_mesktemp |= CANbuf[1] << 8;
    mellomstegTom = bool(CANbuf[2]);
    mesketankTom = bool(CANbuf[3]);
  }

}


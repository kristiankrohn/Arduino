int s0 = 31;
int s1 = 39;  //Pinout solenoidventiler
int s2 = 40;
int s3 = 41;
int s4 = 42;
int s5 = 43;
int s6 = 44;
int s7 = 45;
int s8 = 46;
int s9 = 47;
int mellomsteg_knapp = 32;
int mellomstegpower = 30;
int mellomstegretning = 29;
int regventpower = 33;
int regventretning = 34;
int automan_knapp = 5;
unsigned long pulseventilStartTime;
unsigned long ventilStartTime;

void VentilInit(){
  pinMode(mellomstegpower, OUTPUT);
  pinMode(mellomstegretning, OUTPUT);
  pinMode(regventpower, OUTPUT);
  pinMode(regventretning, OUTPUT);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT); // Solenoid Ventiler
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(s4, OUTPUT);
  pinMode(s5, OUTPUT);
  pinMode(s6, OUTPUT);
  pinMode(s7, OUTPUT);
  pinMode(s8, OUTPUT);
  pinMode(s9, OUTPUT);
  digitalWrite(s0, HIGH);
  digitalWrite(s1, HIGH);   //AKTIV LAV
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  digitalWrite(s4, HIGH);
  digitalWrite(s5, HIGH);
  digitalWrite(s6, HIGH);
  digitalWrite(s7, HIGH);
  digitalWrite(s8, HIGH);
  digitalWrite(s9, HIGH);
  digitalWrite(regventpower, HIGH);
  digitalWrite(regventretning, HIGH);
  digitalWrite(mellomstegpower, HIGH);
  digitalWrite(mellomstegretning, HIGH);
  pinMode(mellomsteg_knapp, INPUT_PULLUP);
  pinMode(automan_knapp, INPUT_PULLUP); // AUTO-MAN knapp
}


void init_reguleringsventil() {
  ventilStartTime = millis();
}

void opneRegvent() {

  bool c;
  c = digitalRead(5);
  if (!c) {
    digitalWrite(regventpower, HIGH);
    digitalWrite(regventretning, HIGH);
  }
  else {
    if (5000 > (Now - ventilStartTime)) {
      digitalWrite(regventpower, LOW);
      digitalWrite(regventretning, LOW);
    } else  {
      digitalWrite(regventpower, HIGH);
    }
  }
}

void lukkeRegvent() {

  if (5000 > (Now - ventilStartTime)) {
    digitalWrite(regventpower, LOW);
    digitalWrite(regventretning, HIGH);
  } else {
    digitalWrite(regventpower, HIGH);
  }
}

void lukkeRegventUpower(){
    digitalWrite(regventpower, LOW);
    digitalWrite(regventretning, HIGH);
}

void reguleringsventil(int mesk_set) {  //Vurder å skrive om til PWM modulert PI reg
  bool c;
  c = digitalRead(5);
  if (!c) {
    digitalWrite(regventpower, HIGH);
    digitalWrite(regventretning, HIGH);
  }
  else {
    int k = analog_mesktemp;

    if ((Now - ventilStartTime) > 5000) { //time to shift the Relay Window
      ventilStartTime += 5000;

      if (mesk_set >= k) {               //ÅPNE
        digitalWrite(regventretning, LOW);
      }

      else {
        digitalWrite(regventretning, HIGH);
      }

    }
    if (300 > Now - ventilStartTime) {
      digitalWrite(regventpower, LOW);
    }

    else {
      digitalWrite(regventpower, HIGH);
    }
  }
}

void reguleringsventilSkyll(int aapningstid){
    if ((Now - ventilStartTime) < aapningstid) {
    digitalWrite(regventpower, LOW);
    digitalWrite(regventretning, HIGH);
  } else {
    digitalWrite(regventpower, HIGH);
  }
}

void init_mellomstegsventil() {
  pulseventilStartTime = millis();
}

void mellomstegsventil(int aapningstid) {

  if ((Now - pulseventilStartTime) < aapningstid) { //time to shift the Relay Window
    digitalWrite(mellomstegpower, LOW);
    digitalWrite(mellomstegretning, LOW);
  }

  else {
    digitalWrite(mellomstegpower, HIGH);
    digitalWrite(mellomstegretning, HIGH);
  }
}

void lukkemellomstegsventil() {
  digitalWrite(mellomstegpower, LOW);
  digitalWrite(mellomstegretning, HIGH);
}

void solenoid() {
  bool c;
  c = digitalRead(5);
  if (!c) { // Manuell ventilkjøring
    digitalWrite(s0, HIGH);
    digitalWrite(s1, HIGH);   //AKTIV LAV
    digitalWrite(s2, HIGH);
    digitalWrite(s3, HIGH);
    digitalWrite(s4, HIGH);
    digitalWrite(s5, HIGH);
    digitalWrite(s6, HIGH);
    digitalWrite(s7, HIGH);
    digitalWrite(s8, HIGH);
    digitalWrite(s9, HIGH);
    digitalWrite(mellomstegpower, LOW);

    if (digitalRead(mellomsteg_knapp) == false) {
      digitalWrite(mellomstegretning, LOW);
    }
    else {
      digitalWrite(mellomstegretning, HIGH);
    }

  }
  else {
    if (Steg == 1) { // FYLLE VANN I KOKETANK
      digitalWrite(s0, HIGH);
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, HIGH);
      digitalWrite(s5, LOW);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, LOW);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else if (Steg == 2) { // VARME VANN TIL STRIKETEMP
      digitalWrite(s0, HIGH);
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, LOW);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, LOW);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else if (Steg == 3) { // STRIKE
      digitalWrite(s0, HIGH);
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, LOW);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, LOW);
      digitalWrite(s7, HIGH);
      digitalWrite(s8, LOW);
      digitalWrite(s9, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }
    else if (Steg == 4) { // LUFTING
      digitalWrite(s0, LOW);
      digitalWrite(s1, LOW);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, LOW);
      digitalWrite(s4, HIGH);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, HIGH);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }
    else if (Steg == 5) { // MESK
      digitalWrite(s0, LOW);
      digitalWrite(s1, LOW);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, LOW);
      digitalWrite(s7, HIGH);
      digitalWrite(s8, LOW);
      digitalWrite(s9, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else if (Steg == 6) { //SKYLLING - renning ned i mellomsteg
      digitalWrite(s0, LOW);
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, LOW);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, LOW);
      digitalWrite(s7, HIGH);
      digitalWrite(s8, LOW);
      digitalWrite(s9, HIGH);
    }

    else if (Steg == 7) { //AVRENNING - pulsepumpe
      digitalWrite(s0, LOW);
      digitalWrite(s1, LOW);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, LOW);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
    }

    else if (Steg == 8) { // Renne ned i mellomsteg
      digitalWrite(s0, LOW);
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, HIGH);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, HIGH);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
    }
    else if (Steg == 9) { // Lufte mellomsteg
      digitalWrite(s0, HIGH);
      digitalWrite(s1, LOW);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, LOW);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, LOW);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, LOW);
    }
    else if (Steg == 10) {  // Pumpe opp fra mellomsteg til koketank
      digitalWrite(s0, HIGH);
      digitalWrite(s1, LOW);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, LOW);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, LOW);
    }
    else if (Steg == 11) { //OPPKOK
      digitalWrite(s0, HIGH);
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, LOW);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, LOW);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else if (Steg == 12) { //KOKING
      digitalWrite(s0, HIGH);
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, LOW);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, LOW);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else if (Steg == 13) { //NEDKJØLING
      digitalWrite(s0, HIGH);
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, HIGH);
      digitalWrite(s5, LOW);
      digitalWrite(s6, LOW);
      digitalWrite(s7, HIGH);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, LOW);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else {
      digitalWrite(s0, HIGH);
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, HIGH);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, HIGH);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
      digitalWrite(mellomstegpower, HIGH);
      digitalWrite(mellomstegretning, HIGH);
    }
  }
}



int v1 = 37;
int v3 = 40; 
int v4 = 41;
int v5 = 42;
int v6 = 43;
int v7 = 44;
int v8 = 45;
int v9 = 46;
int v10 = 47;
int mellomsteg_knapp = 27;
int mellomstegpower = 39;
int mellomstegretning = 38;
int regventpower = 35;
int regventretning = 36;
int automan_knapp = 5;
unsigned long pulseventilStartTime;
unsigned long ventilStartTime;

void VentilInit() {
  pinMode(mellomstegpower, OUTPUT);
  pinMode(mellomstegretning, OUTPUT);
  pinMode(regventpower, OUTPUT);
  pinMode(regventretning, OUTPUT);
  pinMode(v1, OUTPUT);
  pinMode(v3, OUTPUT); // Solenoid Ventiler
  pinMode(v4, OUTPUT);
  pinMode(v5, OUTPUT);
  pinMode(v6, OUTPUT);
  pinMode(v7, OUTPUT);
  pinMode(v8, OUTPUT);
  pinMode(v9, OUTPUT);
  pinMode(v10, OUTPUT);
  digitalWrite(v1, HIGH);
  digitalWrite(v3, HIGH);   //AKTIV LAV
  digitalWrite(v4, HIGH);
  digitalWrite(v5, HIGH);
  digitalWrite(v6, HIGH);
  digitalWrite(v7, HIGH);
  digitalWrite(v8, HIGH);
  digitalWrite(v9, HIGH);
  digitalWrite(v10, HIGH);
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

void lukkeRegventUpower() {
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

void reguleringsventilSkyll(int aapningstid) {
  if ((Now - ventilStartTime) < aapningstid) {
    digitalWrite(regventpower, LOW);
    digitalWrite(regventretning, LOW);
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

void ventil() {
  bool c;
  c = digitalRead(automan_knapp);
  if (!c) { // Manuell ventilkjøring
    digitalWrite(v1, HIGH);
    digitalWrite(v3, HIGH);   //AKTIV LAV
    digitalWrite(v4, HIGH);
    digitalWrite(v5, HIGH);
    digitalWrite(v6, HIGH);
    digitalWrite(v7, HIGH);
    digitalWrite(v8, HIGH);
    digitalWrite(v9, HIGH);
    digitalWrite(v10, HIGH);
    if (screen < 40) {
      digitalWrite(mellomstegpower, LOW);

      if (digitalRead(mellomsteg_knapp) == false) {
        digitalWrite(mellomstegretning, LOW);
      }
      else {
        digitalWrite(mellomstegretning, HIGH);
      }
    }
  }
  else {
    if (Steg == 1) { // FYLLE VANN I KOKETANK
      digitalWrite(v1, HIGH);
      digitalWrite(v3, HIGH);   //AKTIV LAV
      digitalWrite(v4, HIGH);
      digitalWrite(v5, HIGH);
      digitalWrite(v6, HIGH);
      digitalWrite(v7, LOW);
      digitalWrite(v8, HIGH);
      digitalWrite(v9, LOW);
      digitalWrite(v10, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else if (Steg == 2) { // VARME VANN TIL STRIKETEMP
      digitalWrite(v1, HIGH);
      digitalWrite(v3, HIGH);   //AKTIV LAV
      digitalWrite(v4, LOW);
      digitalWrite(v5, HIGH);
      digitalWrite(v6, LOW);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, HIGH);
      digitalWrite(v9, LOW);
      digitalWrite(v10, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else if (Steg == 3) { // STRIKE
      digitalWrite(v1, HIGH);
      digitalWrite(v3, HIGH);   //AKTIV LAV
      digitalWrite(v4, LOW);
      digitalWrite(v5, HIGH);
      digitalWrite(v6, LOW);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, LOW);
      digitalWrite(v9, HIGH);
      digitalWrite(v10, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }
    else if (Steg == 4) { // LUFTING
      digitalWrite(v1, LOW);
      digitalWrite(v3, LOW);   //AKTIV LAV
      digitalWrite(v4, HIGH);
      digitalWrite(v5, LOW);
      digitalWrite(v6, HIGH);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, HIGH);
      digitalWrite(v9, HIGH);
      digitalWrite(v10, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }
    else if (Steg == 5) { // MESK
      digitalWrite(v1, LOW);
      digitalWrite(v3, LOW);   //AKTIV LAV
      digitalWrite(v4, HIGH);
      digitalWrite(v5, HIGH);
      digitalWrite(v6, LOW);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, LOW);
      digitalWrite(v9, HIGH);
      digitalWrite(v10, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else if (Steg == 6) { //SKYLLING - renning ned i mellomsteg
      digitalWrite(v1, LOW);
      digitalWrite(v3, HIGH);   //AKTIV LAV
      digitalWrite(v4, HIGH);
      digitalWrite(v5, LOW);
      digitalWrite(v6, LOW);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, LOW);
      digitalWrite(v9, HIGH);
      digitalWrite(v10, HIGH);
    }

    else if (Steg == 7) { //AVRENNING - pulsepumpe
      digitalWrite(v1, LOW);
      digitalWrite(v3, LOW);   //AKTIV LAV
      digitalWrite(v4, HIGH);
      digitalWrite(v5, HIGH);
      digitalWrite(v6, LOW);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, HIGH);
      digitalWrite(v9, LOW);
      digitalWrite(v10, HIGH);
    }

    else if (Steg == 8) { // Renne ned i mellomsteg
      digitalWrite(v1, LOW);
      digitalWrite(v3, HIGH);   //AKTIV LAV
      digitalWrite(v4, HIGH);
      digitalWrite(v5, HIGH);
      digitalWrite(v6, HIGH);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, HIGH);
      digitalWrite(v9, HIGH);
      digitalWrite(v10, HIGH);
    }
    else if (Steg == 9) { // Lufte mellomsteg
      digitalWrite(v1, HIGH);
      digitalWrite(v3, LOW);   //AKTIV LAV
      digitalWrite(v4, HIGH);
      digitalWrite(v5, LOW);
      digitalWrite(v6, LOW);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, HIGH);
      digitalWrite(v9, LOW);
      digitalWrite(v10, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, LOW);
    }
    else if (Steg == 10) {  // Pumpe opp fra mellomsteg til koketank
      digitalWrite(v1, HIGH);
      digitalWrite(v3, LOW);   //AKTIV LAV
      digitalWrite(v4, HIGH);
      digitalWrite(v5, HIGH);
      digitalWrite(v6, LOW);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, HIGH);
      digitalWrite(v9, LOW);
      digitalWrite(v10, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, LOW);
    }
    else if (Steg == 11) { //OPPKOK
      digitalWrite(v1, HIGH);
      digitalWrite(v3, HIGH);   //AKTIV LAV
      digitalWrite(v4, LOW);
      digitalWrite(v5, HIGH);
      digitalWrite(v6, LOW);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, HIGH);
      digitalWrite(v9, LOW);
      digitalWrite(v10, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else if (Steg == 12) { //KOKING
      digitalWrite(v1, HIGH);
      digitalWrite(v3, HIGH);   //AKTIV LAV
      digitalWrite(v4, LOW);
      digitalWrite(v5, HIGH);
      digitalWrite(v6, LOW);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, HIGH);
      digitalWrite(v9, LOW);
      digitalWrite(v10, HIGH);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else if (Steg == 13) { //NEDKJØLING
      digitalWrite(v1, HIGH);
      digitalWrite(v3, HIGH);   //AKTIV LAV
      digitalWrite(v4, HIGH);
      digitalWrite(v5, HIGH);
      digitalWrite(v6, HIGH);
      digitalWrite(v7, LOW);
      digitalWrite(v8, LOW);
      digitalWrite(v9, HIGH);
      digitalWrite(v10, LOW);
      digitalWrite(mellomstegpower, LOW);
      digitalWrite(mellomstegretning, HIGH);
    }

    else {
      digitalWrite(v1, HIGH);
      digitalWrite(v3, HIGH);   //AKTIV LAV
      digitalWrite(v4, HIGH);
      digitalWrite(v5, HIGH);
      digitalWrite(v6, HIGH);
      digitalWrite(v7, HIGH);
      digitalWrite(v8, HIGH);
      digitalWrite(v9, HIGH);
      digitalWrite(v10, HIGH);
      if (screen < 40) {
        digitalWrite(mellomstegpower, HIGH);
        digitalWrite(mellomstegretning, HIGH);
      }
    }
  }
}



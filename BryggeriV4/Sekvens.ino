bool strikeFerdig = false;
bool meskFerdig = false;
bool kokFerdig = false;
bool skyllFerdig = false;
bool avrenningFerdig = false;
bool luftingFerdig = false;
unsigned long startTid;
unsigned long delayTid;


void TimerInit() {
  timer.setCounter(0, 0, 1, timer.COUNT_DOWN, timerComplete);
  timer.start();
  timer.setInterval(print_time2, 1000); // kan ikke fjernes
}

void timerComplete() {
  if (Steg == 0) { //Fjern etter testing
    //Start=true;
    //Serial.println("START!");
  }
  if (Steg == 3) {
    strikeFerdig = true;
  }
  if (Steg == 4) {
    luftingFerdig = true;
  }
  if (Steg == 5) {
    meskFerdig = true;
  }
  if (Steg == 6) {
    skyllFerdig = true;
  }
  if (Steg == 7) {
    avrenningFerdig = true;
  }
  if (Steg == 9) {
    luftemellomsteg = true;
  }
  if (Steg == 12) {
    kokFerdig = true;
    //Serial.println("Kok ferdig!");
  }
}

void TimerReset() {
  strikeFerdig == false;
  meskFerdig = false;
  skyllFerdig = false;
  avrenningFerdig = false;
  kokFerdig = false;
  luftingFerdig = false;
  luftemellomsteg = false;
}

void print_time2() //Fjern etter testing
{
  //Serial.print("Tiden er: ");
  //Serial.println(timer.getCurrentTime());
}

void setupSteg(int nxtSteg) {

  TimerReset();
  timer.pause();
  timer.setCounter(0, 0, 0, timer.COUNT_DOWN, timerComplete);
  Start = true;
  
  if (nxtSteg == 0) {
    Steg = 0;
  }

  else if (nxtSteg == 1) {
    Steg = 1;
  }

  else if (nxtSteg == 2) {
    Steg = 2;
    Pumpe = true;
  }

  else if (nxtSteg == 3) {
    Steg = 3;
    Serial.print("Striketemp var: ");
    Serial.println(striketemp);
    Pumpe = true;
    // Oppsett av timer for neste steg.
    int striketid;
    striketid = (int)(meskevolum * pumpekonstant);
    int t0 = 0;
    int i0 = 0;

    if (striketid >= 60) {
      t0 = striketid / 60;
      i0 = striketid % 60;
    }
    else {
      i0 = striketid;
      t0 = 0;
    }
    init_reguleringsventil();
    timer.setCounter(0, t0, i0, timer.COUNT_DOWN, timerComplete);
    timer.start();
    /*Serial.print(t0);
      Serial.print(i0);
      Serial.println(striketid); */
    Serial.println("Steg 3");
    startTid = millis();
  }

  else if (nxtSteg == 4) {
    Steg = 4;
    Serial.println("Steg 4");
    Pumpe = false;
    // Oppsett av timer for lufting
    timer.setCounter(0, 0, 10, timer.COUNT_DOWN, timerComplete);
    timer.start();
  }

  else if (nxtSteg == 5) {
    Steg = 5;
    Serial.println("Steg 5");
    Pumpe = true;
    // Oppsett av timer for mesking
    int t1 = 0, i1 = 0;
    if (mesketid >= 60) {
      t1 = mesketid / 60; //Timer
      i1 = mesketid % 60; //Minutter
    }
    else {
      i1 = mesketid;
    }

    init_reguleringsventil();
    timer.setCounter(t1, i1, 0, timer.COUNT_DOWN, timerComplete);
    timer.start();
  }

  else if (nxtSteg == 6) {
    Steg = 6;
    Serial.println("Steg 5");
    Pumpe = true;
    //Oppsett av timer for skylling.
    int t2 = 0, i2 = 0;
    int skylletid = 0;
    skylletid = (int)(skyllevolum * pumpekonstant);

    if (skylletid >= 60) {
      t2 = (int)skylletid / 60;
      i2 = (int)skylletid % 60;
    }

    else {
      i2 = skylletid;
    }

    timer.setCounter(0, t2, i2, timer.COUNT_DOWN, timerComplete);
    timer.start();
    init_reguleringsventil();
    init_mellomstegsventil();
  }

  else if (nxtSteg == 7) {
    Steg = 7;
    Serial.println("Steg 6"); //Pulse pumpe

    //Oppsett av timer for avrenning.
    int t3 = 0, i3 = 0;

    if (avrenningstid >= 60) {
      t3 = avrenningstid / 60;
      i3 = avrenningstid % 60;
    }

    else {
      i3 = avrenningstid;
    }

    timer.setCounter( t3, i3, 0, timer.COUNT_DOWN, timerComplete);
    timer.start();
    init_reguleringsventil();
    init_pulsepumpe();
  }

  else if (nxtSteg == 8) {
    if (mesketankTom == true) {
      Steg = 9;
      timer.setCounter(0, 0, 10, timer.COUNT_DOWN, timerComplete);
      timer.start();
    }
    else {
      Steg = 8;
    }
    Serial.println("Steg 7");
    init_mellomstegsventil();
    //Oppsett timer lufting

    Pumpe = false;
  }

  else if (nxtSteg == 9) {
    Steg = 9;
    init_mellomstegsventil();
    timer.setCounter(0, 0, 10, timer.COUNT_DOWN, timerComplete);
    timer.start();
  }

  else if (nxtSteg == 10) {
    Steg = 10;
    Pumpe = true;
  }

  else if (nxtSteg == 11) {
    if ((Now - delayTid) > 6000) {

      int t4 = 0, i4 = 0;
      if (koketid >= 60) {
        t4 = koketid / 60; //Timer
        i4 = koketid % 60; //Minutter
      }
      else {
        i4 = koketid;
      }
      Pumpe = true;
      timer.setCounter(t4, i4, 0, timer.COUNT_DOWN, timerComplete);
      //Oppsett av timer for koking
      Steg = 11;
    }

    else if (mellomstegTom == false) {
      delayTid = millis();
    }
  }

  else if (nxtSteg == 12) {
    int t4 = 0, i4 = 0;
    if (koketid >= 60) {
      t4 = koketid / 60; //Timer
      i4 = koketid % 60; //Minutter
    }
    else {
      i4 = koketid;
    }
    timer.setCounter(t4, i4, 0, timer.COUNT_DOWN, timerComplete);
    timer.start();
    Steg = 12;
    Serial.println("Steg 8");
    Pumpe = true;
  }

  else if (nxtSteg == 13) {
    Steg = 13;
    Serial.println("Steg 9");
    Pumpe = false;

  }
  //SendSteg();
  SendStartBrew();
}



void sekvens() { //SEKVENS          SEKVENS          SEKVENS          SEKVENS          SEKVENS          SEKVENS
  if (Steg == 0) {
    //Venter på startsignal
    if (Start == true) {
      Steg = 1;
      tick = 0;
      screen = 7;
      //striketemp = MeskSet + 40;
      Serial.println("Steg 1");
      SendStartBrew();
    }
  }


  else if ((Steg == 1) && (Start == true)) {
    //Fyller vann i koketanken
    koketankvolum = tick / flowmeterkonstant; //148,15 tics/L

    if (koketankvolum >= (meskevolum + skyllevolum)) { //Når koketankvolum = meskevolum + skyllevolum

      Serial.println("Steg 2");

      Steg = 2;
      Pumpe = true;
      SendSteg();
    }
  }


  else if ((Steg == 2) && (Start == true)) {
    //Varmer opp vannet i koketanken til striketemp

    //if ((Input >= striketemp) && (menuNav == 6)) { //koketanktemp == Input
    if (Input >= striketemp) {
      Steg = 3;
      Serial.print("Striketemp var: ");
      Serial.println(striketemp);
      Pumpe = true;
      // Oppsett av timer for neste steg.
      int striketid;
      striketid = (int)(meskevolum * pumpekonstant);
      int t0 = 0;
      int i0 = 0;

      if (striketid >= 60) {
        t0 = striketid / 60;
        i0 = striketid % 60;
      }
      else {
        i0 = striketid;
        t0 = 0;
      }
      init_reguleringsventil();
      timer.setCounter(0, t0, i0, timer.COUNT_DOWN, timerComplete);
      timer.start();
      /*Serial.print(t0);
        Serial.print(i0);
        Serial.println(striketid); */
      Serial.println("Steg 3");
      startTid = millis();
      SendSteg();
    }
  }


  else if ((Steg == 3) && (Start == true)) {
    //Pumper strikevann over i mesketank
    opneRegvent();

    if (strikeFerdig) {
      Steg = 4;
      Serial.println("Steg 4");
      Pumpe = false;
      // Oppsett av timer for lufting
      timer.setCounter(0, 0, 10, timer.COUNT_DOWN, timerComplete);
      timer.start();
      SendSteg();
    }
  }
  else if ((Steg == 4) && (Start == true)) {


    if (luftingFerdig) {
      Serial.println("Steg 5");
      Pumpe = true;
      Steg = 5;
      SendSteg();
      // Oppsett av timer for mesking
      int t1 = 0, i1 = 0;
      if (mesketid >= 60) {
        t1 = mesketid / 60; //Timer
        i1 = mesketid % 60; //Minutter
      }
      else {
        i1 = mesketid;
      }

      init_reguleringsventil();
      timer.setCounter(t1, i1, 0, timer.COUNT_DOWN, timerComplete);
      timer.start();
    }
  }
  else if ((Steg == 5) && (Start == true)) {
    //Mesking
    reguleringsventil(MeskSet);
    int m = (int)timer.getCurrentMinutes();
    /*if (m < 14) { // Setter setpunkt til mashout
      Serial.println(m);
      //MeskSet = MeskSet; //Skylletemp
      }*/

    if (meskFerdig) {
      Steg = 6;
      SendSteg();
      Serial.println("Steg 6");
      Pumpe = true;
      //Oppsett av timer for skylling.
      int t2 = 0, i2 = 0;
      int skylletid = 0;
      skylletid = (int)(skyllevolum * pumpekonstant);

      if (skylletid >= 60) {
        t2 = (int)skylletid / 60;
        i2 = (int)skylletid % 60;
      }

      else {
        i2 = skylletid;
      }

      timer.setCounter(0, t2, i2, timer.COUNT_DOWN, timerComplete);
      timer.start();
      init_reguleringsventil();
      init_mellomstegsventil();
    }
  }

  else if ((Steg == 6) && (Start == true)) {
    // Skylling
    reguleringsventilSkyll(reguleringaapning);
    mellomstegsventil(mellomstegaapning);
    if (menuNav == 6) {     // NB! NB! NB! NB! Dette gjør at man må trykke enter for å komme videre i programmet når det er tomt for skyllevann
      //if (skyllFerdig) {
      Steg = 7;
      SendSteg();
      Serial.println("Steg 6"); //Pulse pumpe

      //Oppsett av timer for avrenning.
      int t3 = 0, i3 = 0;

      if (avrenningstid >= 60) {
        t3 = avrenningstid / 60;
        i3 = avrenningstid % 60;
      }

      else {
        i3 = avrenningstid;
      }

      timer.setCounter( t3, i3, 0, timer.COUNT_DOWN, timerComplete);
      timer.start();
      init_reguleringsventil();
      init_pulsepumpe();
    }
  }

  else if ((Steg == 7) && (Start == true)) {
    // tapping fra mesketank
    lukkeRegvent();
    pulsepumpe();
    //getSensordata();
    lukkemellomstegsventil();
    if ((mesketankTom == true) || (avrenningFerdig == true)) {
      if (mesketankTom == true) {
        Steg = 9;
        SendSteg();
        timer.setCounter(0, 0, 10, timer.COUNT_DOWN, timerComplete);
        timer.start();
      }
      else {
        Steg = 8;
        SendSteg();
      }
      Serial.println("Steg 7");
      init_mellomstegsventil();
      //Oppsett timer lufting

      Pumpe = false;
    }
  }
  //8. Renne resten ned i mellomsteg
  else if ((Steg == 8) && (Start == true)) {
    //getSensordata();
    mellomstegsventil(mellomstegaapning);

    if (mesketankTom == true) {
      Steg = 9;
      SendSteg();
      init_mellomstegsventil();
      timer.setCounter(0, 0, 10, timer.COUNT_DOWN, timerComplete);
      timer.start();
    }

  }
  //9. Lufte mellomsteg
  else if ((Steg == 9) && (Start == true)) {
    mellomstegsventil(5000);
    if (luftemellomsteg == true) {
      Steg = 10;
      SendSteg();
      Pumpe = true;
    }

  }
  //10. pumpe opp fra mellomsteg
  else if ((Steg == 10) && (Start == true)) {

    //getSensordata();

    if (mellomstegTom == true) {
      if ((Now - delayTid) > 6000) {

        int t4 = 0, i4 = 0;
        if (koketid >= 60) {
          t4 = koketid / 60; //Timer
          i4 = koketid % 60; //Minutter
        }
        else {
          i4 = koketid;
        }
        Pumpe = true;
        timer.setCounter(t4, i4, 0, timer.COUNT_DOWN, timerComplete);
        //Oppsett av timer for koking
        Steg = 11;
        SendSteg();
      }
    }
    else if (mellomstegTom == false) {
      delayTid = millis();
    }

  }
  else if ((Steg == 11) && (Start == true)) {
    // Koking
    lukkemellomstegsventil();
    if (Input >= kokepunkt) { //koketanktemp == Input
      timer.start();
      Steg = 12;
      SendSteg();
      Serial.println("Steg 8");
      Pumpe = true;
    }

  }

  else if ((Steg == 12) && (Start == true)) {

    if (kokFerdig == true) {
      Steg = 13;
      SendSteg();
      Serial.println("Steg 9");
      Pumpe = false;

    }
  }

  else if ((Steg == 13) && (Start == true)) {
    // Nedkjøling

    if (Input < pitchtemp) { //Input == koketanktemp
      Steg = 0;
      SendSteg();
      Serial.println("Ferdig!");
      TimerReset();
      Start = false;

      screen = 0;
    }
  }

  /* else{
     Steg = 0;
     Serial.println("Steg 0");
    }*/

}


//SKJERM      SKJERM      SKJERM      SKJERM      SKJERM      SKJERM      SKJERM      SKJERM
static char** skjermbuffer = NULL;
LiquidCrystal_I2C lcd(0x27, 20, 4);
const int back = 9;
const int down = 8;
const int up = 7;
const int enter = 6;
unsigned long screenStartTime;
int man_volum;
int man_koktemp;
int man_koktempC = 67;
int man_mesktemp;
int man_mesktempC = 67;

bool Back, Down, Up, Enter;
bool oldBack, oldDown, oldUp, oldEnter;
int8_t menuPos = 0;
int MeskSetC;

void printString(String c, int i) {
  for (int j = 0; j < c.length(); j++) {
    skjermbuffer[i][j + 1] = c[j];
  }
}
void printBuffer(char** skjermbuffer) {
  for (int i = 0; i < 4; i++) {
    lcd.setCursor(0, i);
    for (int j = 0; j < 20; j++) {
      lcd.print(skjermbuffer[i][j]);
    }
  }
}

void menuNavigation() {

  Back = digitalRead(back);
  Down = digitalRead(down);
  Up = digitalRead(up);
  Enter = digitalRead(enter);

  if ((Back == false) && (Back != oldBack)) {
    menuNav = 4;
  }
  else if ((Down == false) && (Down != oldDown)) {
    menuNav = 2;
    menuPos ++;
  }
  else if ((Up == false) && (Up != oldUp)) {
    menuNav = 8;
    menuPos --;
  }
  else if ((Enter == false) && (Enter != oldEnter)) {
    menuNav = 6;
  }
  else {
    menuNav = 5;
  }
  oldBack = Back;
  oldDown = Down;
  oldUp = Up;
  oldEnter = Enter;
  //Serial.println(menuPos);
  if (menuPos > 3) {
    menuPos = 0;
  }
  else if (menuPos < 0) {
    menuPos = 3;
  }
}


int analogToCelcius(int analog) {
  //Bruk regresjonsanalyse etter kalibrering for å komme fram til polynomfunksjon
  float y2, y1, y0, y;
  float x;
  x = (float)analog;
  y2 = 7.303936479 * pow(10, -6) * pow(x, 2);
  y1 = 8.864423369 * pow(10, -2) * x;
  y0 = 5.550170204 * pow(10, -1);
  y = y2 + y1 - y0 ;

  return (int) constrain(y, 0, 99);
}

int celciusToAnalog(int celcius) {
  //Bruk regresjonsanalyse etter kalibrering for å komme fram til polynomfunksjon
  float y8, y7, y6, y5, y4, y3, y2, y1, y0, y;
  float x;
  x = (float)celcius;
  y8 = 7.65775221  * pow(10, -11) * pow(x, 8);
  y7 = 3.125765713 * pow(10, -8) * pow(x, 7);
  y6 = 5.364581341 * pow(10, -6) * pow(x, 6);
  y5 = 5.028645646 * pow(10, -4) * pow(x, 5);
  y4 = 2.795824874 * pow(10, -2) * pow(x, 4);
  y3 = 9.350909963 * pow(10, -1) * pow(x, 3);
  y2 = 18.1332556  * pow(x, 2);
  y1 = 172.4566288 * x;
  y0 = 733.2841988;
  y = y8 - y7 + y6 - y5 + y4 - y3 + y2 - y1 + y0;

  return (int) constrain(y, 0, 1023);
}

void writeSkjermbuffer() {

  skjermbufferClear();

  switch (menuNav) {
    case 6: {
        if (screen == 0) {
          if (menuPos == 0) {
            screen = 1;
            MeskSetC = analogToCelcius(MeskSet);
          }
          else if (menuPos == 1) {
            screen = 20;
          }
          else if (menuPos == 2) {
            screen = 30;
          }
          else if (menuPos == 3) {
            screen = 40;
          }
        }
        else if (screen == 1) {
          screen = 2;
          MeskSet = celciusToAnalog(MeskSetC);
        }
        else if (screen == 2) {
          screen = 3;
        }
        else if (screen == 3) {
          screen = 4;
        }
        else if (screen == 4) {
          screen = 5;
        }
        else if (screen == 5) {
          screen = 6;
        }
        else if (screen == 6) {
          screen = 7;
        }
        else if (screen == 30) {
          if (menuPos == 0) {
            screen = 31;
          }
          else if (menuPos == 1) {
            screen = 33;
          }
          else if (menuPos == 2) {
            screen = 35;
            man_tick = 0;
          }
          else if (menuPos == 3) {
            screen = 37; // Fjern kommentar for å kunne gå inn i fjerde linje på manuellvalg
          }
        }
        else if (screen == 31) {
          screen = 32;
          man_koktemp = celciusToAnalog(man_koktempC);
        }
        else if (screen == 33) {
          screen = 34;
          man_mesktemp = celciusToAnalog(man_mesktempC);
        }
        else if (screen == 40) {
          if (menuPos == 0) {
            screen = 41;
          }
          else if (menuPos == 1) {
            screen = 42;
          }
          else if (menuPos == 2) {
            screen = 44;
          }
        }
        else if (screen == 42) { //Regvent
          screen = 43;
          EEPROMWriteInt(reguleringaapningAddr, reguleringaapning);
          init_reguleringsventil();
        }
        else if (screen == 44) {
          screen = 45;
          EEPROMWriteInt(mellomstegaapningAddr, mellomstegaapning);
          init_mellomstegsventil();
          
        }
        else if ((screen == 43)||(screen == 45)){
          screen = 40;
        }

        menuPos = 0;
        
      }
      break;
    case 4: {
        menuPos = 0;
        if (screen == 0) {
          screen = 0;
        }
        else if (screen == 20) {
          screen = 0;
        }
        else if (screen == 30) {
          screen = 0;
        }
        else if (screen == 33) {
          screen = 30;
        }
        else if (screen == 35) {
          screen = 30;
        }
        else if (screen == 37) {
          screen = 30;
        }
        else if (screen == 40) {
          screen = 0;
        }
        else if (screen == 42) {
          screen = 40;
        }
        else if (screen == 44) {
          screen = 40;
        }
        else {
          screen--;
        }
      }
      break;
    default: break;
  }
  switch (screen) {
    case 0: {
        printString("Start brygging", 0);
        printString("Vaskeprogram", 1);
        printString("Manuell drift", 2);
        printString("Kalibrering", 3);
        printArrow();
      }
      break;
    case 1: {
        String meskeTempstring = String(MeskSetC);
        printString(String("Mesketemp = " + meskeTempstring), 1);
        MeskSetC = changeVariable(MeskSetC);
      }
      break;

    case 2: {
        String meskeTidString = String(mesketid);
        printString(String("Mesketid = " + meskeTidString), 1);
        mesketid = changeVariableTens(mesketid);
      }
      break;

    case 3: {
        String kokeTidString = String(koketid);
        printString(String("Koketid = " + kokeTidString), 1);
        koketid = changeVariableTens(koketid);
      }
      break;

    case 4: {
        String meskeVolumString = String(meskevolum);
        printString(String("Meskevolum = " + meskeVolumString), 1);
        meskevolum = changeVariable(meskevolum);
      }
      break;

    case 5: {
        String skylleVolumString = String(skyllevolum);
        printString(String("Skyllevolum = " + skylleVolumString), 1);
        skyllevolum = changeVariable(skyllevolum);
      }
      break;

    case 6: {
        printString("Falskbunn og filter", 0);
        printString("montert?", 1);
        printString("Klart til start?", 3);
      }
      break;

    case 7: {
        String stringSteg;
        if (Steg == 0) {
          stringSteg = "Standby";
        }
        else if (Steg == 1) {
          stringSteg = String("Fyller vann: " + String(koketankvolum));
        }
        else if (Steg == 2) {
          stringSteg = String("Varmer vann til: " + String(analogToCelcius(striketemp)));
        }
        else if (Steg == 3) {
          stringSteg = "Strike";
          String TimerTimer = String(timer.getCurrentTime());
          printString(String("Timer: " + TimerTimer), 3);
        }
        else if (Steg == 4) {
          stringSteg = "Lufting";
          String TimerTimer = String(timer.getCurrentTime());
          printString(String("Timer: " + TimerTimer), 3);
        }
        else if (Steg == 5) {
          stringSteg = "Mesking";
          String TimerTimer = String(timer.getCurrentTime());
          printString(String("Timer: " + TimerTimer), 3);
        }
        else if (Steg == 6) {
          stringSteg = "Skylling, entr fr nxt";
        }
        else if ((Steg <= 10) && (Steg >= 7)) {
          stringSteg = "Avrenning";
        }
        else if (Steg == 11) {
          stringSteg = "Oppkok";
        }
        else if (Steg == 12) {
          stringSteg = "Koking";
          String TimerTimer = String(timer.getCurrentTime());
          printString(String("Timer: " + TimerTimer), 3);
        }
        else if (Steg == 13) {
          stringSteg = "Nedkjøling";
        }
        Start = true;
        printString(stringSteg, 0);
        String mesketemp = String(analogToCelcius(analog_mesktemp));
        printString(String("Mesketemperatur: " + mesketemp), 1);
        String koketemp = String(analogToCelcius(koktemp()));
        printString(String("Koketemperatur: " + koketemp), 2);

      }
      break;

    case 20: {
        printString("Wait for it", 1);
      }
      break;
    case 30: {
        printString("TmpRegKok", 0);
        printString("TmpRegMesk", 1);
        printString("Vannfylling", 2);
        printString("Bustest", 3);
        printArrow();
      }
      break;
    case 31: {
        String man_kokeTempstring = String(man_koktempC);
        printString(String("KokSet = " + man_kokeTempstring), 1);
        man_koktempC = changeVariable(man_koktempC);
      }
      break;
    case 32: {
        String man_kokeTempstring = String(man_koktempC);
        printString(String("KokSet = " + man_kokeTempstring), 1);
        String koketemp = String(analogToCelcius(koktemp()));
        printString(String("Koketemperatur: " + koketemp), 2);
      }
      break;
    case 33: {
        String man_meskeTempstring = String(man_mesktempC);
        printString(String("MeskSet = " + man_meskeTempstring), 1);
        man_mesktempC = changeVariable(man_mesktempC);
        init_reguleringsventil();
      }
      break;
    case 34: {
        String man_meskset = String(man_mesktempC);
        printString(String("MeskSet = " + man_meskset), 1);
        String mesketemp = String(analogToCelcius(analog_mesktemp));
        printString(String("Mesketemperatur: " + mesketemp), 2);
        reguleringsventil(man_mesktemp);
      }
      break;
    case 35: {
        man_volum = man_tick / flowmeterkonstant;
        String manvolum = String(man_volum);
        String mantick = String(man_tick);
        printString(String("Volum: " + manvolum), 1);
        printString(String("Ticks: " + mantick), 2);
      }
      break;
    case 37: {
        //CANbus test
        //getSensordata();
        String mesketankTomstring = String(mesketankTom);
        String mellomstegTomstring = String(mellomstegTom);
        printString(String("Mesketank = " + mesketankTomstring), 1);
        printString(String("Mellomsteg = " + mellomstegTomstring), 2);
      }
      break;
    case 40: {
        printString("Analog 0", 0);
        printString("Reguleringsventil", 1);
        printString("Mellomstegsventil", 2);
        printArrow();

      }
      break;
    case 41: {
        String analog0 = String(analog_mesktemp);
        printString(String("Analog 0 = " + analog0), 1);
      }
      break;
    case 42: {
        lukkeRegventUpower();
        reguleringaapning = changeVariableHunds(reguleringaapning);
        String regventaapningString = String(reguleringaapning);
        printString("Regvent tuning", 0);
        printString(String("Timer = :" + regventaapningString), 1);
      }
      break;
    case 43: {
        reguleringsventilSkyll(reguleringaapning);
        printString("Regvent saved", 1);
      }
      break;
    case 44: {
        lukkemellomstegsventil();
        mellomstegaapning = changeVariableHunds(mellomstegaapning);
        String mellomstegaapningString = String(mellomstegaapning);
        printString("Mellomsteg tuning", 0);
        printString(String("Timer = " + mellomstegaapningString), 1);
      }
      break;
    case 45: {
        mellomstegsventil(mellomstegaapning);
        printString("Mellomstg Saved", 1);
      }
      break;
  }

  if (screen == 32) {
    mankok = true;
  }
  else {
    mankok = false;
  }

  if (screen == 34) {
    manmesk = true;
  }
  else {
    manmesk = false;
  }
}

void printArrow() {
  /* if (menuPos > 3) {
     menuPos = 0;
    }
    else if (menuPos < 0) {
     menuPos = 3;
    }
  */
  for (int i = 0; i < 4; i++) {
    if (i == menuPos)
    {
      skjermbuffer[i][0] = 0x7E;
    }
    else {
      skjermbuffer[i][0] = 32;
    }
  }
}

int changeVariable(int i) {
  if (menuNav == 8) {
    i++;
  }
  else if (menuNav == 2) {
    i--;
  }
  i = max(i, 0);
  return i;
}

int changeVariableTens(int i) {
  if (menuNav == 8) {
    i = i + 10;
  }
  else if (menuNav == 2) {
    i = i - 10;
  }
  i = max(i, 0);
  return i;
}

int changeVariableHunds(int i) {
  if (menuNav == 8) {
    i = i + 100;
  }
  else if (menuNav == 2) {
    i = i - 100;
  }
  i = max(i, 0);
  return i;
}

void skjermbufferInit() {
  skjermbuffer = (char**)malloc(sizeof(char*) * 4);
  if (skjermbuffer == NULL) {
    Serial.println("buffer** not allocated\n");
  }

  for (int i = 0; i < 4; i++) {
    skjermbuffer[i] = malloc(sizeof(char) * 20);
    if (skjermbuffer[i] == NULL) {
      Serial.println("buffer not allocated\n");
    }
  }

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 20; j++) {
      skjermbuffer[i][j] = 32;
      //skjermbuffer[i][j] = i+j+64; //Testvariabel
    }
  }
}

void skjermbufferClear() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 20; j++) {
      skjermbuffer[i][j] = 32;
    }
  }
}

void lcdRestore() {
  if (!digitalRead(up) && !digitalRead(down)) {
    lcd.init();
    lcd.backlight();
  }
}

void lcdInit() {
  pinMode(back, INPUT_PULLUP);
  pinMode(down, INPUT_PULLUP);
  pinMode(up, INPUT_PULLUP);
  pinMode(enter, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  skjermbufferInit();
  delay(100);
  printString("   Breidabrygg", 0);
  printString("    Laget av:", 1);
  printString("Krohn & Hoel Eng.", 2);
  printBuffer(skjermbuffer);
  delay(1000);
  screenStartTime = millis();
}

void lcdLoop() {
  menuNavigation();
  writeSkjermbuffer();
  lcdRestore();
  if (Now - screenStartTime > 500)
  {

    printBuffer(skjermbuffer);
    windowStartTime += 500;

  }
}


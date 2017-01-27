#include <OPC.h>
#include <Countimer.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Bridge.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//#include <PID_v1.h>

//#define RelayPin 36 //Varmeelement


/* Sekvenskode med følgende steg                        Betingelser
    0. Standby                                          Start=0
    1. Fylle vann for mesking og skylling i koketank    koktankvolum=meskvolum+skyllvolum
    2. Varme opp vannet til striketemperatur            kokset=striketemp
    3. Pumpe meskevann inn i mesketank                  mesktankvolum=meskvolum
    4. Meske                                            Nedtelling mesketid
    5. Skylle                                           skylle mesk til koktankvolum=0
    6. Avrenning                                        pulse pumpe 10s intervall i en gitt tid. pumpe mesk opp i kokekar
    7. Oppkok                                           Mesketank er tom. Vent på oppkok
    8. Kok                                              Nedtelling koketid. Humletilsetninger
    9. Nedkjøling                                       Kokset=0, Kjølespiral


*/

OPCEthernet aOPC;
LiquidCrystal_I2C lcd(0x27, 20, 4);
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

IPAddress ip(10, 0, 0, 100);
IPAddress gateway(10, 0, 0, 1);
IPAddress dns_server(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);

/*
  IPAddress ip(192, 168, 0, 100);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress dns_server(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
*/
const int listen_port = 80;


opcOperation digital_status_input[14], analog_status_input[6];

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
//PID myPID(&Input, &Output, &Setpoint, 1,10000,0, DIRECT);
//int WindowSize = 10000; //Setter frekvensen for PWM output fra PID 10000ms = 10s
unsigned long windowStartTime;
unsigned long ventilStartTime;
unsigned long pumpeStartTime;
unsigned long startTid;
unsigned long varmeStartTime;
unsigned long pulseventilStartTime;
Countimer timer;
const byte flowMeter = 2; // Må være enten pin 2, 3, 18, 19, 20, 21
unsigned long tick = 0; // Flowmeter tics
unsigned long man_tick = 0;
bool strikeFerdig = false;
bool meskFerdig = false;
bool kokFerdig = false;
bool skyllFerdig = false;
bool avrenningFerdig = false;
bool luftingFerdig = false;
bool mankok = false;
bool manmesk = false;
bool Start;
int resetindicator;
int Steg;
int meskevolum = 40;  // FJERN TILORDNING ETTER TESTING
int skyllevolum = 35; // FJERN TILORDNING ETTER TESTING
int koketankvolum;
int mesketankvolum;
int striketemp;
int skylletemp;
int pitchtemp = 556; // HUSK Å KALIBRERE DENNE
int kokepunkt = 667; // HUSK Å KALIBRERE DENNE
int mesketid = 90;
int koketid = 90;
int avrenningstid = 15; //Minutter
const float pumpekonstant = 14.25; //Sek/Liter 9min og 30sek for 40L = 570sek/40L =
int MeskSet = 631;
int regventpower = 33;
int regventretning = 34;
int man_volum;
int man_koktemp;
int man_koktempC = 67;
int man_mesktemp;
int man_mesktempC = 67;
int screen = 0;

int s1 = 39;  //Pinout solenoidventiler
int s2 = 40;
int s3 = 41;
int s4 = 42;
int s5 = 43;
int s6 = 44;
int s7 = 45;
int s8 = 46;
int s9 = 47;

//Pin 4 og pin 10 brukes til CS til Ethernetshield

byte button; //Trykknapp for pumpa
byte oldbutton = 1;
byte buttonpin = 3;

byte lokkButton; //Trykknapp for lokket
byte lokkOldbutton = 1;
byte lokkButtonpin = 37; //pin 1

int pumpePin = 35;
int varmePin = 36;
int lokkPin = 38;



void flowtick() { //Interrupt Service Routine for flowmeter
  tick++;
  man_tick++;
}



//SKJERM      SKJERM      SKJERM      SKJERM      SKJERM      SKJERM      SKJERM      SKJERM
static char** skjermbuffer = NULL;

const int back = 9;
const int down = 8;
const int up = 7;
const int enter = 6;

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

bool Back, Down, Up, Enter;
bool oldBack, oldDown, oldUp, oldEnter;
int menuNav;
int8_t menuPos = 0;

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
  float y5, y4, y3, y2, y1, y0, y;
  float x;

  //y = (6.821153642 * pow(10, -10) * pow(x, 5)) - (1.769379423 * pow(10, -6) * pow(x, 4)) + (1.800293103 * pow(10, -3) * pow(x, 3)) - (8.921113014 * pow(10, -1) * pow(x, 2))+ (213.5846863 * x) - 19535.88672;
  x = (float)analog;

  y5 = 6.821153642 * pow(10, -10) * pow(x, 5);
  y4 = 1.769379423 * pow(10, -6) * pow(x, 4);
  y3 = 1.800293103 * pow(10, -3) * pow(x, 3);
  y2 = 8.921113014 * pow(10, -1) * pow(x, 2);
  y1 = 213.5846863 * x;
  y0 = 19535.88672;
  y = y5 - y4 + y3 - y2 + y1 - y0;

  return (int) y;
}

int celciusToAnalog(int celcius) {
  //Bruk regresjonsanalyse etter kalibrering for å komme fram til polynomfunksjon
  float y7, y6, y5, y4, y3, y2, y1, y0, y;
  float x;
  x = (float)celcius;

  y7 = 7.989214793 * pow(10, -10) * pow(x, 7);
  y6 = 3.212325030 * pow(10, -7) * pow(x, 6);
  y5 = 5.336986237 * pow(10, -5) * pow(x, 5);
  y4 = 4.733179194 * pow(10, -3) * pow(x, 4);
  y3 = 2.409499472 * pow(10, -1) * pow(x, 3);
  y2 = 7.006408924 * pow(x, 2);
  y1 = 108.7976574 * x;
  y0 = 138.9718901;
  y = y7 - y6 + y5 - y4 + y3 - y2 + y1 - y0;

  return (int) y;
}

int MeskSetC;
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
            //screen = 37; // Fjern kommentar for å kunne gå inn i fjerde linje på manuellvalg
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
      }
      break;
    case 4: {
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
        printString("Manuell tempreg", 2);
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
        mesketid = changeVariable(mesketid);
      }
      break;

    case 3: {
        String kokeTidString = String(koketid);
        printString(String("Koketid = " + kokeTidString), 1);
        koketid = changeVariable(koketid);
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
          stringSteg = "Fyller vann";
        }
        else if (Steg == 2) {
          stringSteg = "Varmer vann";
        }
        else if (Steg == 3) {
          stringSteg = "Strike";
        }
        else if (Steg == 4) {
          stringSteg = "Lufting";
        }
        else if (Steg == 5) {
          stringSteg = "Mesking";
        }
        else if (Steg == 6) {
          stringSteg = "Skylling";
        }
        else if (Steg == 7) {
          stringSteg = "Avrenning";
        }
        else if (Steg == 8) {
          stringSteg = "Oppkok";
        }
        else if (Steg == 9) {
          stringSteg = "Koking";
        }
        else if (Steg == 10) {
          stringSteg = "Nedkjøling";
        }
        Start = true;
        printString(stringSteg, 0);
        String mesketemp = String(analogToCelcius(analogRead(0)));
        printString(String("Mesketemperatur: " + mesketemp), 1);
        String koketemp = String(analogToCelcius((analogRead(1) + analogRead(2)) / 2));
        printString(String("Koketemperatur: " + koketemp), 2);
        String TimerTimer = String(timer.getCurrentTime());
        printString(String("Timer: " + TimerTimer), 3);
      }
      break;

    case 20: {
        printString("Wait for it", 1);
      }
      break;
    case 30: {
        printString("Koketank", 0);
        printString("Mesketank", 1);
        printString("Vannfylling", 2);
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
        String koketemp = String(analogToCelcius((analogRead(1) + analogRead(2)) / 2));
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
        String mesketemp = String(analogToCelcius(analogRead(0)));
        printString(String("Mesketemperatur: " + mesketemp), 2);
        reguleringsventil(man_mesktemp);
      }
      break;
    case 35: {
        man_volum = man_tick / 142;
        String manvolum = String(man_volum);
        printString(String("Volum: " + manvolum), 1);
      }
      break;
    case 40: {
        String analog0 = String(analogRead(0));
        printString(String("Analog 0 = " + analog0), 1);
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
  delay(5000);
}

void lcdLoop() {
  menuNavigation();
  writeSkjermbuffer();
  printBuffer(skjermbuffer);
}


//OPC     OPC     OPC     OPC     OPC     OPC     OPC     OPC     OPC     OPC     OPC     OPC     OPC
bool Pumpe;
bool pumpe(const char *itemID, const opcOperation opcOP, const bool value) {
  static bool pumpeValue = true;
  if (opcOP == opc_opwrite) {
    pumpeValue = value;

    if (pumpeValue)
      Pumpe = true;
    else
      Pumpe = false;
  }
  else
    return Pumpe;
}

bool startRW(const char *itemID, const opcOperation opcOP, const bool value) {

  if (opcOP == opc_write) {
    Start = value;
  }
  else
    return Start;
}

int CurrentMins(const char *itemID, const opcOperation opcOP, const int value) {
  return (int)timer.getCurrentMinutes();
}

int CurrentHours(const char *itemID, const opcOperation opcOP, const int value) {
  return (int)timer.getCurrentHours();
}

int Step(const char *itemID, const opcOperation opcOP, const int value) {
  if (opcOP == opc_write) {
    Steg = value;
  }
  else
    return Steg;
}

int analogPin0 = 0;
int write_mesktemp(const char *itemID, const opcOperation opcOP, const int value) {
  return analogRead(analogPin0);
}


int analogPin1 = 1;
int write_koktopptemp(const char *itemID, const opcOperation opcOP, const int value) {
  return analogRead(analogPin1);
}


int analogPin2 = 2;
int write_kokbunntemp(const char *itemID, const opcOperation opcOP, const int value) {
  return analogRead(analogPin2);
}

int meskevolumRW(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    meskevolum = value;
  }
  else {
    return meskevolum;
  }
}

int skyllevolumRW(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    skyllevolum = value;
  }
  else {
    return skyllevolum;
  }
}

int mesketidRW(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    mesketid = value;
  }
  else {
    return mesketid;
  }
}

int koketidRW(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    koketid = value;
  }
  else {
    return koketid;
  }
}

int avrenningstidRW(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    avrenningstid = value;
  }
  else {
    return avrenningstid;
  }
}

int read_MeskSet(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    MeskSet = value;
  }
  else {
    return MeskSet;
  }
}

int write_setpoint(const char *itemID, const opcOperation opcOP, const int value) {
  return Setpoint;
}

int write_output(const char *itemID, const opcOperation opcOP, const int value) {
  return Output;
}

int write_input(const char *itemID, const opcOperation opcOP, const int value) {
  return Input;
}



int koktemp() {
  int temp1, temp2, tempsnitt, temp;
  temp1 = analogRead(A1);
  temp2 = analogRead(A2);

  tempsnitt = (temp1 + temp2) / 2;

  temp = tempsnitt;

  return temp;
}

int Setpunkt(int Steg, int MeskSet, int striketemp) {
  int Setpunkt;
  int meskset;
  int k = 1023;

  meskset = MeskSet + 2;

  if (Steg == 2) {
    Setpunkt = striketemp;
  }

  else if (Steg == 5) {
    Setpunkt = meskset;
  }

  else if ((Steg == 8) || (Steg == 9)) {
    Setpunkt = k;
  }

  else if (mankok) {
    Setpunkt = man_koktemp;
  }

  else if (manmesk) {
    Setpunkt = man_mesktemp + 2;
  }
  else {
    Setpunkt = 0;
  }
  return Setpunkt;
}



void sekvens() { //SEKVENS          SEKVENS          SEKVENS          SEKVENS          SEKVENS          SEKVENS
  if (Steg == 0) {
    //Venter på startsignal
    if (Start == true) {
      Steg = 1;
      tick = 0;
      Serial.println("Steg 1");
    }
  }


  else if ((Steg == 1) && (Start == true)) {
    //Fyller vann i koketanken
    koketankvolum = tick / 142; //148,15 tics/L

    if (koketankvolum >= (meskevolum + skyllevolum)) { //Når koketankvolum = meskevolum + skyllevolum

      Serial.println("Steg 2");
      //Pumpe = true;
      Steg = 2;
      Pumpe = true;
    }
  }


  else if ((Steg == 2) && (Start == true)) {
    //Varmer opp vannet i koketanken til striketemp
    striketemp = MeskSet + 5;   //                     <---------   DENNE MÅ KALIBRERES!!!
    if (Input >= striketemp) { //koketanktemp == Input
      Steg = 3;

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

      timer.setCounter(0, t0, i0, timer.COUNT_DOWN, timerComplete);
      timer.start();
      /*Serial.print(t0);
        Serial.print(i0);
        Serial.println(striketid); */
      Serial.println("Steg 3");
      startTid = millis();
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
    }
  }
  else if ((Steg == 4) && (Start == true)) {


    if (luftingFerdig) {
      Serial.println("Steg 5");
      Pumpe = true;
      Steg = 5;
      // Oppsett av timer for mesking
      int t1 = 0, i1 = 0;
      if (mesketid >= 60) {
        t1 = mesketid / 60; //Timer
        i1 = mesketid % 60; //Minutter
      }
      else {
        i1 = mesketid;
      }
      ventilStartTime = millis();
      timer.setCounter(t1, i1, 0, timer.COUNT_DOWN, timerComplete);
      timer.start();
    }
  }
  else if ((Steg == 5) && (Start == true)) {
    //Mesking
    reguleringsventil(MeskSet);
    int m = (int)timer.getCurrentMinutes();
    if (m < 14) { // Setter setpunkt til mashout
      Serial.println(m);
      //MeskSet = MeskSet; //Skylletemp
    }

    if (meskFerdig) {
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
    }
  }

  else if ((Steg == 6) && (Start == true)) {
    // Skylling
    opneRegvent();
    if (skyllFerdig) {
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
  }

  else if ((Steg == 7) && (Start == true)) {
    // tapping fra mesketank
    lukkeRegvent();
    pulsepumpe();
    if (avrenningFerdig == true) {
      Steg = 8;
      Serial.println("Steg 7");
      int t4 = 0, i4 = 0;
      if (koketid >= 60) {
        t4 = koketid / 60; //Timer
        i4 = koketid % 60; //Minutter
      }
      else {
        i4 = koketid;
      }

      timer.setCounter(t4, i4, 0, timer.COUNT_DOWN, timerComplete);
      //Oppsett av timer for koking
    }
  }

  else if ((Steg == 8) && (Start == true)) {
    // Koking

    if (Input >= kokepunkt) { //koketanktemp == Input
      timer.start();
      Steg = 9;
      Serial.println("Steg 8");
    }

  }

  else if ((Steg == 9) && (Start == true)) {

    if (kokFerdig == true) {
      Steg = 10;
      Serial.println("Steg 9");
      Pumpe = false;

    }
  }

  else if ((Steg == 10) && (Start == true)) {
    // Nedkjøling

    if (Input < pitchtemp) { //Input == koketanktemp
      Steg = 0;
      Serial.println("Ferdig!");
      strikeFerdig == false;
      meskFerdig = false;
      skyllFerdig = false;
      avrenningFerdig = false;
      kokFerdig = false;
      Start = false;
      luftingFerdig = false;
    }
  }

  /* else{
     Steg = 0;
     Serial.println("Steg 0");
    }*/

}


void timerComplete() {
  if (Steg == 0) { //Fjern etter testing
    //Start=true;
    Serial.println("START!");
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
    kokFerdig = true;
    Serial.println("Kok ferdig!");
  }
}




void print_time2() //Fjern etter testing
{
  Serial.print("Tiden er: ");
  Serial.println(timer.getCurrentTime());
}

void solenoid() {
  bool c;
  c = digitalRead(5);
  if (!c) {
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
  else {
    if (Steg == 1) { // FYLLE VANN I KOKETANK
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, HIGH);
      digitalWrite(s5, LOW);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, LOW);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
    }

    else if (Steg == 2) { // VARME VANN TIL STRIKETEMP
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, LOW);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, LOW);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
    }

    else if (Steg == 3) { // STRIKE
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, LOW);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, LOW);
      digitalWrite(s7, HIGH);
      digitalWrite(s8, LOW);
      digitalWrite(s9, HIGH);
    }
    else if (Steg == 4) { // LUFTING
      digitalWrite(s1, LOW);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, LOW);
      digitalWrite(s4, HIGH);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, HIGH);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
    }
    else if (Steg == 5) { // MESK
      digitalWrite(s1, LOW);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, LOW);
      digitalWrite(s7, HIGH);
      digitalWrite(s8, LOW);
      digitalWrite(s9, HIGH);
    }

    else if (Steg == 6) { //SKYLLING
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

    else if (Steg == 7) { //AVRENNING
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

    else if (Steg == 8) { //OPPKOK
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, LOW);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, LOW);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
    }

    else if (Steg == 9) { //KOKING
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, LOW);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, LOW);
      digitalWrite(s5, HIGH);
      digitalWrite(s6, HIGH);
      digitalWrite(s7, LOW);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, HIGH);
    }

    else if (Steg == 10) { //NEDKJØLING
      digitalWrite(s1, HIGH);   //AKTIV LAV
      digitalWrite(s2, HIGH);
      digitalWrite(s3, HIGH);
      digitalWrite(s4, HIGH);
      digitalWrite(s5, LOW);
      digitalWrite(s6, LOW);
      digitalWrite(s7, HIGH);
      digitalWrite(s8, HIGH);
      digitalWrite(s9, LOW);
    }

    else {
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
  }
}

void init_reguleringsventil() {
  ventilStartTime = millis();
}

void opneRegvent() {
  unsigned long Now = millis();
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
    }
  }
}

void lukkeRegvent() {
  unsigned long Now = millis();
  if (5000 > (Now - ventilStartTime)) {
    digitalWrite(regventpower, LOW);
    digitalWrite(regventretning, HIGH);
  }
}

void reguleringsventil(int mesk_set) {  //Vurder å skrive om til PWM modulert PI reg
  bool c;
  c = digitalRead(5);
  if (!c) {
    digitalWrite(regventpower, HIGH);
    digitalWrite(regventretning, HIGH);
  }
  else {
    int k = analogRead(analogPin0);
    unsigned long Now = millis();
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

void init_varmereg() {
  varmeStartTime = millis();
}

void varmeReg() {
  unsigned long Now = millis();
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

void init_pulsepumpe() {
  pumpeStartTime = millis();
}

void pulsepumpe() {
  unsigned long Now = millis();
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

void init_pulseventil() {
  pulseventilStartTime = millis();
}

void pulseventil(int ventil) {
  unsigned long Now = millis();
  if ((Now - pulseventilStartTime) > 6000) { //time to shift the Relay Window
    pulseventilStartTime += 6000;
  }
  if (3000 > Now - pulseventilStartTime) {
    digitalWrite(ventil, HIGH);
  }

  else {
    digitalWrite(ventil, LOW);
  }
}


void pumpe() {
  button = digitalRead(buttonpin);

  if (button && !oldbutton) { // same as if(button == high && oldbutton == low) we have a new button press
    if (Pumpe == false) { // if the state is off, turn it on
      //do stuff
      Pumpe = true;
    }
    else { // if the state is on, turn it off
      //do stuff
      Pumpe = false;
    }
    oldbutton = 1;
  }
  else if (!button && oldbutton) // same as if(button == low && oldbutton == high)
  {
    // the button was released
    oldbutton = 0;
  }

  if (Pumpe) {
    digitalWrite(pumpePin, LOW);
  }
  if (Pumpe == false) {
    digitalWrite(pumpePin, HIGH);
  }
}
bool Lokk;
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
    Serial.println("pressed");
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


void setup() {//SETUP           SETUP           SETUP           SETUP           SETUP            SETUP            SETUP
  Ethernet.begin(mac, ip, dns, gateway, subnet);
  Serial.begin(9600);
  pinMode(pumpePin, OUTPUT);
  pinMode(lokkPin, OUTPUT);
  digitalWrite(pumpePin, HIGH);
  digitalWrite(lokkPin, HIGH);
  pinMode(varmePin, OUTPUT);  // Varmelement
  resetindicator = 0;   // Debug indicator

  pinMode(regventpower, OUTPUT);
  pinMode(regventretning, OUTPUT);
  pinMode(s1, OUTPUT); // Solenoid Ventiler
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);
  pinMode(s4, OUTPUT);
  pinMode(s5, OUTPUT);
  pinMode(s6, OUTPUT);
  pinMode(s7, OUTPUT);
  pinMode(s8, OUTPUT);
  pinMode(s9, OUTPUT);

  digitalWrite(s1, HIGH);   //AKTIV LAV
  digitalWrite(s2, HIGH);
  digitalWrite(s3, HIGH);
  digitalWrite(s4, HIGH);
  digitalWrite(s5, HIGH);
  digitalWrite(s6, HIGH);
  digitalWrite(s7, HIGH);
  digitalWrite(s8, HIGH);
  digitalWrite(s9, HIGH);

  Start = false;
  pinMode(lokkButtonpin, INPUT_PULLUP);
  pinMode(buttonpin, INPUT_PULLUP); //Pumpeknapp
  pinMode(5, INPUT_PULLUP); // AUTO-MAN knapp

  pinMode(flowMeter, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowMeter), flowtick, RISING);
  digitalWrite(regventpower, HIGH);
  digitalWrite(regventretning, HIGH);
  lcdInit();
  //initialize the variables we're linked to
  Setpoint = 100;

  varmeStartTime = 0;
  //windowStartTime = millis();
  //tell the PID to range between 0 and the full window size
  // myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  //myPID.SetMode(AUTOMATIC);


  byte k;
  for (k = 0; k < 14; k++) digital_status_input[k] = opc_opread;
  for (k = 0; k < 5; k++) analog_status_input[k] = opc_opread;

  aOPC.setup(listen_port, mac, ip);
  aOPC.addItem("Pumpe", opc_readwrite, opc_bool, pumpe);
  aOPC.addItem("Start", opc_readwrite, opc_bool, startRW);
  aOPC.addItem("Steg", opc_readwrite, opc_int, Step);
  aOPC.addItem("CurrentMins", opc_readwrite, opc_int, CurrentMins);
  aOPC.addItem("CurrentHours", opc_readwrite, opc_int, CurrentHours);
  aOPC.addItem("Mesktemp", opc_readwrite, opc_int, write_mesktemp);
  aOPC.addItem("KokToppTemp", opc_readwrite, opc_int, write_koktopptemp);
  aOPC.addItem("KokBunnTemp", opc_readwrite, opc_int, write_kokbunntemp);
  aOPC.addItem("Meskevolum", opc_readwrite, opc_int, meskevolumRW);
  aOPC.addItem("Skyllevolum", opc_readwrite, opc_int, skyllevolumRW);
  aOPC.addItem("Mesketid", opc_readwrite, opc_int, mesketidRW);
  aOPC.addItem("Koketid", opc_readwrite, opc_int, koketidRW);
  aOPC.addItem("Avrenningstid", opc_readwrite, opc_int, avrenningstidRW);
  aOPC.addItem("MeskSet", opc_readwrite, opc_int, read_MeskSet);
  aOPC.addItem("Setpoint", opc_readwrite, opc_int, write_setpoint);
  aOPC.addItem("Output", opc_readwrite, opc_int, write_output);
  aOPC.addItem("Input", opc_readwrite, opc_int, write_input);

  timer.setCounter(0, 0, 1, timer.COUNT_DOWN, timerComplete);
  timer.start();
  timer.setInterval(print_time2, 1000);
}






void loop() {//MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN
  aOPC.processOPCCommands();
  timer.run();
  //Serial.println(resetindicator);
  //resetindicator++;

  lcdLoop();
  Input = koktemp();
  sekvens();
  solenoid();
  pumpe();
  lokk();
  aOPC.processOPCCommands();
  //Serial.println(tick);
  Setpoint = Setpunkt(Steg, MeskSet, striketemp);
  varmeReg();
  /* myPID.Compute();

    /************************************************
      turn the output pin on/off based on pid output
    ************************************************
    unsigned long now = millis();
    if(now - windowStartTime>WindowSize)
    { //time to shift the Relay Window
     windowStartTime += WindowSize;
    }
    if(Output > now - windowStartTime) digitalWrite(RelayPin,HIGH);
    else digitalWrite(RelayPin,LOW);
  */

}

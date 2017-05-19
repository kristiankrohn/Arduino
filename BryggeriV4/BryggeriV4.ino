#include <OPC.h>
#include <Ethernet.h>
#include <Countimer.h>
#include <SPI.h>
#include <Bridge.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <mcp_can.h>

Countimer timer;


int menuNav;
double Setpoint, Input, Output;

unsigned long Now;
unsigned long windowStartTime;

volatile unsigned long tick = 0; // Flowmeter tics
volatile unsigned long man_tick = 0;
bool mankok = false;
bool manmesk = false;
bool Start;
bool mesketankTom = false;
bool mellomstegTom = false;
bool luftemellomsteg = false;
bool Lokk;
int Steg;
int meskevolum = 40;  
int skyllevolum = 35; 
int koketankvolum;
int mesketankvolum;
int striketemp;
int skylletemp;
int pitchtemp = 226; // HUSK Å KALIBRERE DENNE
int kokepunkt = 990; // HUSK Å KALIBRERE DENNE
int mesketid = 60;
int koketid = 60;
int avrenningstid = 15; //Minutter
const float pumpekonstant = 12; //Sek/Liter 9min og 30sek for 40L = 570sek/40L =
const int flowmeterkonstant = 140.5;
int MeskSet;

int screen = 0;
bool Pumpe;
int analog_mesktemp;
int analog_koktopptemp;
int analog_kokbunntemp;




void setup() {//SETUP           SETUP           SETUP           SETUP           SETUP            SETUP            SETUP
  Serial.begin(9600);
  Wire.begin(8);
  pinMode(4, OUTPUT); // Disable SDcard reader on ethernet shield
  digitalWrite(4, HIGH);
  LokkInit();
  VentilInit();
  PumpeInit();
  Start = false;
  TempInit();
  FlowMeterInit();
  lcdInit();
  windowStartTime = millis();
  OPCinit(); 
  TimerInit();
  CanBusInit();
}

void loop() {//MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN
  runOPC();
  //noInterrupts();
  timer.run();
  Now = millis();
  //interrupts();
  getAnalogdata();
  lcdLoop();
  Input = koktemp();
  Setpoint = Setpunkt();
  sekvens();
  varmeReg();
  if (Now - windowStartTime > 500) {
    solenoid();
    pumpe();
    lokk();
    windowStartTime += 500;
    //Serial.println(getPumpCurrent());
  }
}
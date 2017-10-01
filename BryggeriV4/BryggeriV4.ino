#include <OPC.h>
#include <Ethernet.h>
#include <Countimer.h>
#include <SPI.h>
#include <Bridge.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <mcp_can.h>
#include <EEPROM.h>
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
const float pumpekonstant = 10.5; //Sek/Liter 9min og 30sek for 40L = 570sek/40L =
const int flowmeterkonstant = 140.5;
int MeskSet;
int mellomstegaapning;
int reguleringaapning;
int mellomstegaapningAddr = 2;
int reguleringaapningAddr = 4;
int screen = 0;
bool Pumpe;
int analog_mesktemp;
int analog_koktopptemp;
int analog_kokbunntemp;
int pumpeTerskel;
int pumpeTerskelAddr = 6;
unsigned char flagRecv = 0;

//This function will write a 2 byte integer to the eeprom at the specified address and address + 1
void EEPROMWriteInt(int p_address, int p_value)
     {
     byte lowByte = ((p_value >> 0) & 0xFF);
     byte highByte = ((p_value >> 8) & 0xFF);

     EEPROM.write(p_address, lowByte);
     EEPROM.write(p_address + 1, highByte);
     }

//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
int EEPROMReadInt(int p_address)
     {
     byte lowByte = EEPROM.read(p_address);
     byte highByte = EEPROM.read(p_address + 1);

     return ((lowByte << 0) & 0xFF) + ((highByte << 8) & 0xFF00);
     }


void VariablesInit(){
  mellomstegaapning = EEPROMReadInt(mellomstegaapningAddr);
  reguleringaapning = EEPROMReadInt(reguleringaapningAddr);
  pumpeTerskel = EEPROMReadInt(pumpeTerskelAddr);
  Serial.print("Mellomsteg loaded: ");
  Serial.println(mellomstegaapning);
  Serial.print("Regvent loaded: ");
  Serial.println(reguleringaapning);
}


void setup() {//SETUP           SETUP           SETUP           SETUP           SETUP            SETUP            SETUP
  Serial.begin(9600);
  Wire.begin(8);
  pinMode(4, OUTPUT); // Disable SDcard reader on ethernet shield
  digitalWrite(4, HIGH);
  VariablesInit();
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
  //MeskSet = celciusToAnalog(67);

}

void loop() {//MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN       MAIN

  //Serial.println(MeskSet);
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
  if(flagRecv){
    flagRecv = 0;
    getCANmessage();
  }
  if (Now - windowStartTime > 500) {
    //Serial.println("Timer");
    ventil();
    pumpe();
    lokk();
    windowStartTime += 500;
    //Serial.println(getPumpCurrent());
    SendTimeandTemp();
    if(Steg == 1){
      SendTick();
    }
  }
}

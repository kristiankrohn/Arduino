#include <OPC.h>
#include <Ethernet.h>
#include <SPI.h>
#include <Bridge.h>
#include <PID_v1.h>

#define RelayPin 37


OPCEthernet aOPC;

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192,168 ,0,150);
IPAddress gateway(192,168,0,1);
IPAddress dns_server(192,168,0,1);
IPAddress subnet(255,255,255,0);
const int listen_port = 80;



opcOperation digital_status_input[14], analog_status_input[6];




//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint, 1,1000,0, DIRECT);

int WindowSize = 5000;
unsigned long windowStartTime;


int pumpePin = 6;
bool Pumpe;
bool pumpe(const char *itemID, const opcOperation opcOP, const bool value){
  static bool pumpeValue = true;
  if (opcOP == opc_opwrite) {
    pumpeValue = value;
    
    if (pumpeValue)  
      Pumpe=true;
    else
      Pumpe=false;
  } 
  else 
    return Pumpe;  
}


bool prosessValg;
bool prosessvalg(const char *itemID, const opcOperation opcOP, const bool value){ 
  static bool prosessValue = true;
  if (opcOP == opc_opwrite) {
    prosessValue = value;
    
    if (prosessValue)  
      prosessValg = HIGH;
    else
      prosessValg = LOW;
  } 
  else 
    return prosessValue; 
  }  


int analogPin0 = 0;
int write_mesktemp(const char *itemID, const opcOperation opcOP, const int value){
  return analogRead(analogPin0);
}


int analogPin1 = 1;
int write_koktopptemp(const char *itemID, const opcOperation opcOP, const int value){
  return analogRead(analogPin1);
}


int analogPin2 = 2;
int write_kokbunntemp(const char *itemID, const opcOperation opcOP, const int value){
  return analogRead(analogPin2);
}


int MeskSet;
int read_MeskSet(const char *itemID, const opcOperation opcOP, const int value){
 
  if (opcOP == opc_write) 
    MeskSet = value;
  else  
    return MeskSet;
}


int KokSet;
int read_KokSet(const char *itemID, const opcOperation opcOP, const int value){
  if (opcOP == opc_write) 
    KokSet = value;
  else  
    return KokSet;
}

int write_setpoint(const char *itemID, const opcOperation opcOP, const int value){
  return Setpoint;
}

int write_output(const char *itemID, const opcOperation opcOP, const int value){
  return Output;
}

int write_input(const char *itemID, const opcOperation opcOP, const int value){
  return Input;
}

double tempvalg(bool Prosessvalg){
  double temp0, temp1, temp2, tempsnitt, temp;
  temp0=analogRead(0);
  temp1=analogRead(1);
  temp2=analogRead(2);
  tempsnitt=(temp1+temp2)/2;

  if(Prosessvalg == HIGH){
    temp = tempsnitt;
  }
  else{
    temp = temp0;
  }
  return temp; 
}


double Setpunkt(bool Prosessvalg, int MeskSet, int KokSet){
  double Setpunkt;
  if(Prosessvalg == HIGH){
    Setpunkt = (double)KokSet;
  }
  else{
    Setpunkt = (double)MeskSet;
  }
  return Setpunkt;
}

byte button;
byte oldbutton = 0;
byte buttonpin = 2;


void setup() {
Ethernet.begin(mac, ip, dns, gateway, subnet); 
Serial.begin(9600);
pinMode(pumpePin, OUTPUT);
pinMode(37, OUTPUT);

pinMode(buttonpin, INPUT_PULLUP);

Pumpe=false;


  windowStartTime = millis();

  //initialize the variables we're linked to
  Setpoint = 100;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  
  
byte k;
  for (k=0;k<14;k++) digital_status_input[k] = opc_opread; 
  for (k=0;k<5;k++) analog_status_input[k] = opc_opread; 

  /*
   * OPC Object initialization
   */
  aOPC.setup(listen_port,mac,ip); 

  aOPC.addItem("Prosessvalg",opc_readwrite, opc_bool, prosessvalg);
  aOPC.addItem("Pumpe",opc_readwrite, opc_bool, pumpe);

  aOPC.addItem("Mesktemp",opc_readwrite, opc_int, write_mesktemp);
  aOPC.addItem("KokToppTemp",opc_readwrite, opc_int, write_koktopptemp);
  aOPC.addItem("KokBunnTemp",opc_readwrite, opc_int, write_kokbunntemp);

  aOPC.addItem("MeskSet",opc_readwrite, opc_int, read_MeskSet);
  aOPC.addItem("KokSet",opc_readwrite, opc_int, read_KokSet);

  aOPC.addItem("Setpoint",opc_readwrite, opc_int, write_setpoint);
  aOPC.addItem("Output",opc_readwrite, opc_int, write_output);
  aOPC.addItem("Input",opc_readwrite, opc_int, write_input);
   
}






void loop() {

aOPC.processOPCCommands();

Input = tempvalg(prosessValg);
Setpoint = Setpunkt(prosessValg, MeskSet, KokSet);

button = digitalRead(buttonpin);
 if(button && !oldbutton) // same as if(button == high && oldbutton == low)
 {
   //we have a new button press
   if(Pumpe == false) // if the state is off, turn it on
   {
     //do stuff
     Pumpe = true;
   }
   else // if the state is on, turn it off
   {
     //do stuff
     Pumpe = false; 
   }
   oldbutton = 1;
 }
 else if(!button && oldbutton) // same as if(button == low && oldbutton == high)
 {
   // the button was released
   oldbutton = 0;
 }

if(Pumpe){
  digitalWrite(pumpePin, HIGH);
}
if(Pumpe==false){
  digitalWrite(pumpePin, LOW);
}

  myPID.Compute();

  /************************************************
   * turn the output pin on/off based on pid output
   ************************************************/
  unsigned long now = millis();
  if(now - windowStartTime>WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if(Output > now - windowStartTime) digitalWrite(RelayPin,HIGH);
  else digitalWrite(RelayPin,LOW);
  
  
}







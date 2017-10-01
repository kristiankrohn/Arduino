#include <Countimer.h>
#include <SPI.h>
#include <mcp_can.h>

const int resetpin = 7;
int fbTick, sbTick, tbTick, fobTick;
long now;
long lastblink;
volatile long recievedmessage;
int hours = 1;
int minutes = 0;
int seconds = 0;
int meskevolum = 40;
int skyllevolum = 35;
int mesketid = 60;
int koketid = 60;
int MeskSet = 209;
int MeskSetHB = 2;
int Steg = 0;
int Start = 0;
const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);
unsigned char CANbuf[8];
unsigned char flagRecv = 0;
int resetcount = 0;
int onOffSwitch = 3;
int testRecovery = 4;
int activeIndicator = 5;
int resetIndicator = 6;
bool led = false;
byte button; //Trykknapp for pumpa
byte oldbutton = 1;

void CanBusInit() {
  while (CAN_OK != CAN.begin(CAN_1000KBPS))              // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
}

void GetMessage() {
  while (CAN_MSGAVAIL == CAN.checkReceive()) {
    //Serial.print("Message recieved with ID: ");
    unsigned char len = 0;
    CAN.readMsgBuf(&len, CANbuf);
    int ID = CAN.getCanId();
    //Serial.println(ID);

    //CAN.readMsgBuf(&len, CANbuf);

    if (ID == 0x01) {
      hours = CANbuf[0];
      minutes = CANbuf[1];
      seconds = CANbuf[2];
      Serial.println("Got time");
    }
    else if (ID == 0x02) {
      fbTick = CANbuf[0];
      sbTick = CANbuf[1];
      tbTick = CANbuf[2];
      fobTick = CANbuf[3];
    }
    else if (ID == 0x10) {
      meskevolum = CANbuf[0];
      skyllevolum = CANbuf[1];
      mesketid = CANbuf[2];
      koketid = CANbuf[3];
      MeskSet = CANbuf[4];
      Start = CANbuf[5];
      Steg = CANbuf[6];
      MeskSetHB = CANbuf[7];
      for (int i = 0; i < 8; i++) {
        Serial.println(CANbuf[i]);
      }

    }
    else if (ID == 0x11) {
      Steg = CANbuf[0];
      for (int i = 0; i < 8; i++) {
        Serial.println(CANbuf[i]);
      }

    }
    else {
      //for (int i = 0; i < 8; i++) {
      //Serial.println(CANbuf[i]);
      //}
    }
    //Serial.println("No more messages to read");
    recievedmessage = millis();

  }
}

void restart() {
  Serial.println("initiating resetsequence");
  resetcount++;
  cli();
  digitalWrite(resetpin, LOW);
  delay(200);
  digitalWrite(resetpin, HIGH);
  //delay(5000);
  while ((CAN_MSGAVAIL != CAN.checkReceive()) && (CAN.getCanId() != 0));

  unsigned char stmp[8] = {meskevolum, skyllevolum, mesketid, koketid, MeskSet, Steg, MeskSetHB, 0};
  CAN.sendMsgBuf(0x20, 0, 8, stmp);
  for (int i = 0; i < 8; i++) {
    Serial.println(stmp[i]);
  }
  while ((CAN_MSGAVAIL != CAN.checkReceive()) && (CAN.getCanId() != 0));


  unsigned char stp[8] = {hours, minutes, seconds, Start, fbTick, sbTick, tbTick, fobTick};
  CAN.sendMsgBuf(0x21, 0, 8, stp);
  for (int i = 0; i < 8; i++) {
    Serial.println(stp[i]);
  }
  //delay(10000);
  sei();
  recievedmessage = millis() + 10000;
}

void MCP2515_ISR()
{
  GetMessage();
  //flagRecv = 1;
  //Serial.println("Interrupt!");
}

void resetindication() {
  if (resetcount == 1) {
    digitalWrite(resetIndicator, HIGH);
    led = true;
  }
  else if (resetcount  > 1) {
    if ((now - lastblink) > (1000 / resetcount)) {
      lastblink = now;
      if (led == false) {
        digitalWrite(resetIndicator, HIGH);
        led = true;
      }

      else {
        digitalWrite(resetIndicator, LOW);
        led = false;
      }
    }
  }
  else {
    digitalWrite(resetIndicator, LOW);
  }
}
void activeindication() {
  if (digitalRead(onOffSwitch) == 0) {
    digitalWrite(activeIndicator, HIGH);
  }
  else {
    digitalWrite(activeIndicator, LOW);
    led = false;
  }

}

void manualreset() {
  button = digitalRead(testRecovery);
  if (button && !oldbutton) {
    restart();
    oldbutton = 1;
  }
  else if (!button && oldbutton)
  {
    oldbutton = 0;
  }

}
void setup() {

  digitalWrite(resetpin, HIGH);
  pinMode(resetpin, OUTPUT);
  pinMode(onOffSwitch, INPUT_PULLUP);
  pinMode(testRecovery, INPUT_PULLUP);
  pinMode(resetIndicator, OUTPUT);
  pinMode(activeIndicator, OUTPUT);
  Serial.begin(9600);
  Serial.println("Start init watchdognode");
  digitalWrite(activeIndicator, LOW);
  pinMode(13, OUTPUT);

  CanBusInit();
  attachInterrupt(digitalPinToInterrupt(2), MCP2515_ISR, LOW); // start interrupt
  Serial.println("watchdognode init finished");
  delay(15000);
  now = millis();
  lastblink = now;

}

void loop() {


  now = millis();
  if ((now - recievedmessage) > 5000) {
    //Serial.println(now);
    //Serial.println(recievedmessage);
    if (digitalRead(onOffSwitch) == 0) {
      restart();
    }
    recievedmessage = millis();
  }

  manualreset();
  activeindication();
  resetindication();
}


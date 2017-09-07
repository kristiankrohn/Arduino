#include <Countimer.h>
#include <SPI.h>
#include <mcp_can.h>

const int resetpin = 3;
unsigned long tick;
unsigned long now;
unsigned long recievedmessage;
int hours = 1;
int minutes = 0;
int seconds = 0;
int meskevolum = 40;
int skyllevolum = 35;
int mesketid = 60;
int koketid = 60;
int MeskSet = 67;
int Steg = 0;

const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);
unsigned char CANbuf[8];


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
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    Serial.print("Message recieved with ID: ");
    Serial.println(CAN.getCanId());
    unsigned char len = 0;
    CAN.readMsgBuf(&len, CANbuf);
    recievedmessage = millis();
    if (CAN.getCanId() == 0x01) {
      hours = CANbuf[0];
      minutes = CANbuf[1];
      seconds = CANbuf[2];
    }
    else if (CAN.getCanId() == 0x02) {
      tick = CANbuf[0];
      tick |= CANbuf[1] << 8;
      tick |= CANbuf[2] << 16;
      tick |= CANbuf[3] << 32;
    }
    else if (CAN.getCanId() == 0x10) {
      meskevolum = CANbuf[0];
      skyllevolum = CANbuf[1];
      mesketid = CANbuf[2];
      koketid = CANbuf[3];
      MeskSet = CANbuf[4];
    }
    else if (CAN.getCanId() == 0x11) {
      Steg = CANbuf[0];
    }
    else {
      for (int i = 0; i < 8; i++) {
        Serial.println(CANbuf[i]);
      }
    }

  }
}

void restart(){
  Serial.println("initiating resetsequence");
  digitalWrite(resetpin, LOW);
  delay(100);
  digitalWrite(resetpin, HIGH);
  delay(6000);
  unsigned char stmp[8] = {meskevolum, skyllevolum, mesketid, koketid, MeskSet, Steg, 0, 0};
  CAN.sendMsgBuf(0x20, 0, 8, stmp);
  delay(1000);
  unsigned char stp[8] = {hours, minutes, seconds, 0, 0, 0, 0, 0};
  CAN.sendMsgBuf(0x21, 0, 8, stp);
  delay(10000);  
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Start init sensornode");
  pinMode(resetpin, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(resetpin, HIGH);
  CanBusInit();
  Serial.println("Sensornode init finished");
  delay(15000);
  now = millis();
  recievedmessage = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  
  GetMessage();
  now = millis();
  if((now - recievedmessage) > 5000){
    Serial.println(now);
    Serial.println(recievedmessage);
    restart();
    recievedmessage = millis();
  }
}

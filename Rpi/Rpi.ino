#include <SPI.h>
#include <mcp_can.h>

const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);    
unsigned char CANbuf[8];
unsigned char flagRecv = 0;
const byte interruptPin = 2;
char dataString[50] = {0};
volatile int analog_mesktemp = 0; 
volatile int analog_koktemptopp = 0;
volatile int analog_koktempbunn = 0;

void MCP2515_ISR()
{
  GetMessage();

}

void GetMessage() {
  while (CAN_MSGAVAIL == CAN.checkReceive()) {
    //Serial.print("Message recieved with ID: ");
    unsigned char len = 0;
    CAN.readMsgBuf(&len, CANbuf);
    int ID = CAN.getCanId();
    Serial.println(ID);

    //CAN.readMsgBuf(&len, CANbuf);
    
    if (ID == 0x00) {

      analog_mesktemp = CANbuf[0];
      analog_mesktemp |= CANbuf[1] << 8;
      //mellomstegTom = bool(CANbuf[3]);
      //mesketankTom = bool(CANbuf[2]);
      //Serial.println(analog_mesktemp);
    }
    else if (ID == 0x01) {
      //hours = CANbuf[0];
      //minutes = CANbuf[1];
      //seconds = CANbuf[2];
      
      analog_koktemptopp = CANbuf[4];
      analog_koktemptopp |= CANbuf[5];
      analog_koktempbunn = CANbuf[6];
      analog_koktempbunn |= CANbuf[7];
      
      //Serial.println("Got time");
    }
    /*
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
    */

  }
}

void setup() {
  Serial.begin(9600);              //Starting serial communication
  pinMode(13, OUTPUT);
  while (CAN_OK != CAN.begin(CAN_1000KBPS)) {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  attachInterrupt(digitalPinToInterrupt(interruptPin), MCP2515_ISR, LOW); // start interrupt
}
  
void loop() {
  dataString[0] = analog_mesktemp;
  dataString[2] = analog_koktemptopp;
  dataString[4] = analog_koktempbunn; 
  //Serial.print(analog_mesktemp); 
  //Serial.print(analog_koktemptopp);
  //Serial.print(analog_koktempbunn); 
  GetMessage();
  delay(1000);                  // give the loop some break
}

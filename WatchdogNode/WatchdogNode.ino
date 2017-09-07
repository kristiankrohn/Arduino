#include <Countimer.h>
#include <SPI.h>
#include <mcp_can.h>

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
    for(int i = 0; i < 8; i++){
      Serial.println(CANbuf[i]);
    }
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Start init sensornode");
  CanBusInit();
  Serial.println("Sensornode init finished");
}

void loop() {
  // put your main code here, to run repeatedly:
  GetMessage();
}

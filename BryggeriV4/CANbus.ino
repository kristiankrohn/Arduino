

unsigned char CANbuf[8];
const int SPI_CS_PIN = 49;
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void CanBusInit(){
    while (CAN_OK != CAN.begin(CAN_1000KBPS))              // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
}

void getSensordata() {
  /*
    Wire.requestFrom(9, 4);
    analog_mesktemp = Wire.read();
    analog_mesktemp |= (Wire.read() << 8);
    mellomstegTom = Wire.read();
    mesketankTom = Wire.read();
  */

  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    if (CAN.getCanId() == 0){
      unsigned char len = 0;
      CAN.readMsgBuf(&len, CANbuf);
      analog_mesktemp = CANbuf[0];
      analog_mesktemp |= CANbuf[1] << 8;
      mellomstegTom = bool(CANbuf[2]);
      mesketankTom = bool(CANbuf[3]);
      Serial.println(analog_mesktemp);
    }
  }

}



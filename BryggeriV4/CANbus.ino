

unsigned char CANbuf[8];
const int SPI_CS_PIN = 49;
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

void CanBusInit() {
  while (CAN_OK != CAN.begin(CAN_1000KBPS))              // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
}



void getCANmessage() {
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
    unsigned char len = 0;
    CAN.readMsgBuf(&len, CANbuf);

    if (CAN.getCanId() == 0) {
      
      analog_mesktemp = CANbuf[0];
      analog_mesktemp |= CANbuf[1] << 8;
      mellomstegTom = bool(CANbuf[2]);
      mesketankTom = bool(CANbuf[3]);
      Serial.println(analog_mesktemp);
    }
    else if (CAN.getCanId() == 0x20) {
      Serial.println("First resetmessage received");
      meskevolum = CANbuf[0];
      skyllevolum = CANbuf[1];
      mesketid = CANbuf[2];
      koketid = CANbuf[3];
      MeskSet = CANbuf[4];
      Steg = CANbuf[5];
    }
    else if (CAN.getCanId() == 0x21) {
      Serial.println("Second resetmessage received, and ready to start");
      timer.setCounter(CANbuf[0], CANbuf[1], CANbuf[2], timer.COUNT_DOWN, timerComplete);
      timer.start();
      Start = true;
      screen = 7;
      //if (Steg == (2||3||5||6||10||11||12)){
        //Pumpe = true;
      //}
    }
  }
}

void SendStartBrew() {
  unsigned char stmp[8] = {meskevolum, skyllevolum, mesketid, koketid, MeskSet, 0, 0, 0};
  CAN.sendMsgBuf(0x10, 0, 8, stmp);
}

void SendSteg() {
  unsigned char stmp[8] = {Steg, 0, 0, 0, 0, 0, 0, 0};
  CAN.sendMsgBuf(0x11, 0, 8, stmp);
}

void SendTick() {
  uint8_t fbTick = tick;
  uint8_t sbTick = tick >> 8;
  uint8_t tbTick = tick >> 16;
  uint8_t fobTick = tick >> 32;

  unsigned char stmp[8] = {fbTick, sbTick, tbTick, fobTick, 0, 0, 0, 0};
  CAN.sendMsgBuf(0x02, 0, 8, stmp);
}

void SendTimeandTemp() {
  uint8_t koktoppLb = analog_koktopptemp;
  uint8_t koktoppHb = analog_koktopptemp >> 8;

  uint8_t kokbunnLb = analog_kokbunntemp;
  uint8_t kokbunnHb = analog_kokbunntemp >> 8;

  unsigned char stmp[8] = {timer.getCurrentHours(), timer.getCurrentMinutes(), timer.getCurrentSeconds(), 0, koktoppLb, koktoppHb, kokbunnLb, kokbunnHb};
  CAN.sendMsgBuf(0x01, 0, 8, stmp);
}


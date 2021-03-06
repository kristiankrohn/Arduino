

unsigned char CANbuf[8];
const int SPI_CS_PIN = 49;
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin
int tempSteg;

void CanBusInit() {
  while (CAN_OK != CAN.begin(CAN_1000KBPS))              // init can bus : baudrate = 500k
  {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
  attachInterrupt(digitalPinToInterrupt(18), MCP2515_ISR, FALLING); // start interrupt
}
void MCP2515_ISR()
{
  flagRecv = 1;
  //getCANmessage();
  //Serial.println("Interrupt");
}

void getCANmessage() {
  //Serial.println("Starting reading message");
  while (CAN_MSGAVAIL == CAN.checkReceive()) {
    unsigned char len = 0;
    CAN.readMsgBuf(&len, CANbuf);
    
    if (CAN.getCanId() == 0) {

      analog_mesktemp = CANbuf[0];
      analog_mesktemp |= CANbuf[1] << 8;
      mellomstegTom = bool(CANbuf[3]);
      mesketankTom = bool(CANbuf[2]);
      //Serial.println(analog_mesktemp);
    }
    else if (CAN.getCanId() == 0x20) {
      Serial.println("First resetmessage received");
      meskevolum = CANbuf[0];
      skyllevolum = CANbuf[1];
      mesketid = CANbuf[2];
      koketid = CANbuf[3];
      MeskSet = CANbuf[4];
      tempSteg = CANbuf[5];
      MeskSet |= CANbuf[6] << 8;
      for (int i = 0; i < 8; i++) {
        Serial.println(CANbuf[i]);
      }
    }
    else if (CAN.getCanId() == 0x21) {
      Serial.println("Second resetmessage received, and ready to start");
      for (int i = 0; i < 8; i++) {
        Serial.println(CANbuf[i]);
      }
      tick = CANbuf[4];
      tick |= CANbuf[5] << 8;
      tick |= CANbuf[6] << 16;
      tick |= CANbuf[7] << 32;

      timer.setCounter(CANbuf[0], CANbuf[1], CANbuf[2], timer.COUNT_DOWN, timerComplete);
      timer.start();
      Start = bool(CANbuf[3]);
      Serial.print("Start: ");
      Serial.println(CANbuf[3]);
      Steg = tempSteg;
      if (Start == true) {
        screen = 7;
        if ((Steg == 2) || (Steg == 3) || (Steg == 5) || (Steg == 6) || (Steg == 10) || (Steg == 11) || (Steg == 12)) {
          Pumpe = true;
        }
      }
    }
    else{
      for (int i = 0; i < 8; i++) {
        Serial.println(CANbuf[i], HEX);
      }
    }
  }
  //Serial.println("Finishd reading message");
}

void SendStartBrew() {
  Serial.print(Start);
  Serial.println("Sendig Start Brew");
  int temp = MeskSet;
  int MeskSetHB = temp >>8;
  unsigned char stmp[8] = {meskevolum, skyllevolum, mesketid, koketid, MeskSet, (int)Start, Steg, MeskSetHB};
  CAN.sendMsgBuf(0x10, 0, 8, stmp);
  for (int i = 0; i < 8; i++) {
    Serial.println(stmp[i]);
  }
}

void SendStartSystem(){
  int temp = MeskSet;
  int MeskSetHB = temp >>8;
  unsigned char stmp[8] = {meskevolum, skyllevolum, mesketid, koketid, MeskSet, (int)Start, Steg, MeskSetHB};
  CAN.sendMsgBuf(0x10, 0, 8, stmp);
}

void SendSteg() {
  Serial.println("Sending Steg");
  unsigned char stmp[8] = {Steg, 0, 0, 0, 0, 0, 0, 0};
  CAN.sendMsgBuf(0x11, 0, 8, stmp);
  for (int i = 0; i < 8; i++) {
    Serial.println(stmp[i]);
  }
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

  //Serial.println("Sending time and temp");
  uint8_t koktoppLb = analog_koktopptemp;
  uint8_t koktoppHb = analog_koktopptemp >> 8;

  uint8_t kokbunnLb = analog_kokbunntemp;
  uint8_t kokbunnHb = analog_kokbunntemp >> 8;

  unsigned char stmp[8] = {timer.getCurrentHours(), timer.getCurrentMinutes(), timer.getCurrentSeconds(), 0, koktoppLb, koktoppHb, kokbunnLb, kokbunnHb};
  CAN.sendMsgBuf(0x01, 0, 8, stmp);

}




void getResetData(){
  if (CAN_MSGAVAIL == CAN.checkReceive()) {
  
  }
}

void SendStartBrew(){  
  unsigned char stmp[8] = {meskevolum, skyllevolum, mesketid, koketid, MeskSet, 0, 0, 0};
  CAN.sendMsgBuf(0x10, 0, 8, stmp);
}

void SendSteg(){  
  unsigned char stmp[8] = {Steg, 0, 0, 0, 0, 0, 0, 0};
  CAN.sendMsgBuf(0x11, 0, 8, stmp);
}

void SendTick(){
  uint8_t fbTick = tick;
  uint8_t sbTick = tick >> 8;
  uint8_t tbTick = tick >> 16;
  uint8_t fobTick = tick >> 32;

  unsigned char stmp[8] = {fbTick, sbTick, tbTick, fobTick, 0,0,0,0};
  CAN.sendMsgBuf(0x02, 0, 8, stmp);
}

void SendTimeandTemp(){
  uint8_t koktoppLb = analog_koktopptemp; 
  uint8_t koktoppHb = analog_koktopptemp >> 8;
  
  uint8_t kokbunnLb = analog_kokbunntemp;
  uint8_t kokbunnHb = analog_kokbunntemp >> 8;
  
  unsigned char stmp[8] = {timer.getCurrentHours(), timer.getCurrentMinutes(), timer.getCurrentSeconds(), 0, koktoppLb, koktoppHb, kokbunnLb, kokbunnHb};
  CAN.sendMsgBuf(0x01, 0, 8, stmp);
}


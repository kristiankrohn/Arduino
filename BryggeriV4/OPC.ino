

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
/*
IPAddress ip(10, 0, 0, 100);
IPAddress gateway(10, 0, 0, 1);
IPAddress dns_server(10, 0, 0, 1);
IPAddress subnet(255, 255, 255, 0);
*/

  IPAddress ip(192, 168, 0, 100);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress dns_server(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);

const int listen_port = 80;

OPCEthernet aOPC;
opcOperation digital_status_input[14], analog_status_input[6];




void runOPC(){
  aOPC.processOPCCommands();
}

void OPCinit(){
  Ethernet.begin(mac, ip, dns, gateway, subnet);
  byte k;
  for (k = 0; k < 14; k++) digital_status_input[k] = opc_opread;
  for (k = 0; k < 5; k++) analog_status_input[k] = opc_opread;

  aOPC.setup(listen_port, mac, ip);
  aOPC.addItem("Pumpe", opc_readwrite, opc_bool, pumpe);
  aOPC.addItem("Start", opc_readwrite, opc_bool, startRW);
  aOPC.addItem("Steg", opc_readwrite, opc_int, Step);
  aOPC.addItem("CurrentMins", opc_readwrite, opc_int, CurrentMins);
  aOPC.addItem("CurrentHours", opc_readwrite, opc_int, CurrentHours);
  aOPC.addItem("Mesktemp", opc_readwrite, opc_int, write_mesktemp);
  aOPC.addItem("KokToppTemp", opc_readwrite, opc_int, write_koktopptemp);
  aOPC.addItem("KokBunnTemp", opc_readwrite, opc_int, write_kokbunntemp);
  aOPC.addItem("Meskevolum", opc_readwrite, opc_int, meskevolumRW);
  aOPC.addItem("Skyllevolum", opc_readwrite, opc_int, skyllevolumRW);
  aOPC.addItem("Mesketid", opc_readwrite, opc_int, mesketidRW);
  aOPC.addItem("Koketid", opc_readwrite, opc_int, koketidRW);
  aOPC.addItem("Avrenningstid", opc_readwrite, opc_int, avrenningstidRW);
  aOPC.addItem("MeskSet", opc_readwrite, opc_int, read_MeskSet);
  aOPC.addItem("Setpoint", opc_readwrite, opc_int, write_setpoint);
  aOPC.addItem("Output", opc_readwrite, opc_int, write_output);
  aOPC.addItem("Input", opc_readwrite, opc_int, write_input);
  aOPC.addItem("Koketankvolum", opc_readwrite, opc_int, koketankvolumRW);
  aOPC.addItem("Lokk", opc_readwrite, opc_bool, lokkRW);
}


bool pumpe(const char *itemID, const opcOperation opcOP, const bool value) {
  static bool pumpeValue = true;
  if (opcOP == opc_opwrite) {
    pumpeValue = value;

    if (pumpeValue)
      Pumpe = true;
    else
      Pumpe = false;
  }
  else
    return Pumpe;
}

bool startRW(const char *itemID, const opcOperation opcOP, const bool value) {

  if (opcOP == opc_write) {
    Start = value;
  }
  else
    return Start;
}

int CurrentMins(const char *itemID, const opcOperation opcOP, const int value) {
  return (int)timer.getCurrentMinutes();
}

int CurrentHours(const char *itemID, const opcOperation opcOP, const int value) {
  return (int)timer.getCurrentHours();
}

int Step(const char *itemID, const opcOperation opcOP, const int value) {
  if (opcOP == opc_write) {
    Steg = value;
  }
  else
    return Steg;
}

int write_mesktemp(const char *itemID, const opcOperation opcOP, const int value) {
  return analog_mesktemp;
}

int write_koktopptemp(const char *itemID, const opcOperation opcOP, const int value) {
  return analog_koktopptemp;
}

int write_kokbunntemp(const char *itemID, const opcOperation opcOP, const int value) {
  return analog_kokbunntemp;
}

int meskevolumRW(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    meskevolum = value;
  }
  else {
    return meskevolum;
  }
}

int skyllevolumRW(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    skyllevolum = value;
  }
  else {
    return skyllevolum;
  }
}

int mesketidRW(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    mesketid = value;
  }
  else {
    return mesketid;
  }
}

int koketidRW(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    koketid = value;
  }
  else {
    return koketid;
  }
}

int avrenningstidRW(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    avrenningstid = value;
  }
  else {
    return avrenningstid;
  }
}

int read_MeskSet(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    MeskSet = value;
  }
  else {
    return MeskSet;
  }
}

int write_setpoint(const char *itemID, const opcOperation opcOP, const int value) {
  return Setpoint;
}

int write_output(const char *itemID, const opcOperation opcOP, const int value) {
  return Output;
}

int write_input(const char *itemID, const opcOperation opcOP, const int value) {
  return Input;
}

int koketankvolumRW(const char *itemID, const opcOperation opcOP, const int value) {

  if (opcOP == opc_write) {
    koketankvolum = value;
  }
  else {
    return koketankvolum;
  }
}

bool lokkRW(const char *itemID, const opcOperation opcOP, const bool value) {
  static bool lokkValue = true;
  if (opcOP == opc_opwrite) {
    lokkValue = value;

    if (lokkValue)
      Lokk = true;
    else
      Lokk = false;
    EEPROM.write(0, Lokk);
  }
  else
    return Lokk;
}



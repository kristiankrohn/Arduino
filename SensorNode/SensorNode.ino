#include <Wire.h>
#include <mcp_can.h>

const int SPI_CS_PIN = 9;
MCP_CAN CAN(SPI_CS_PIN);                                    // Set CS pin

int mellomsteg = 6;
int mesketank = 7;
int mesketemp;
bool mellomstegTom = false;
bool mesketankTom = false;

void sendI2c() {
  Wire.write(mesketemp);
  Wire.write(mesketemp >> 8);
  Wire.write(mellomstegTom);
  Wire.write(mesketankTom);
}

void setup() {

  Wire.begin(9);
  Serial.begin(9600);
  pinMode(mellomsteg, INPUT_PULLUP);
  pinMode(mesketank, INPUT_PULLUP);
  pinMode(13, OUTPUT);
  Wire.onRequest(sendI2c);

  while (CAN_OK != CAN.begin(CAN_1000KBPS)) {
    Serial.println("CAN BUS Shield init fail");
    Serial.println(" Init CAN BUS Shield again");
    delay(100);
  }
  Serial.println("CAN BUS Shield init ok!");
  delay(5000);
}

void loop() {

  mesketemp = analogRead(1);
  mellomstegTom = digitalRead(mellomsteg);
  mesketankTom = digitalRead(mesketank);
  //Serial.println(mesketemp);
  
  //Serial.print("Mellomsteg :");
  //Serial.println(mellomstegTom);
  //Serial.print("Mesketank :");
  //Serial.println(mesketankTom);
  
  int mesketempMSB = mesketemp >> 8;
  unsigned char stmp[8] = {mesketemp, mesketempMSB, mellomstegTom, mesketankTom, 4, 5, 6, 7};
  // send data:  id = 0x00, standrad frame, data len = 8, stmp: data buf
  CAN.sendMsgBuf(0x00, 0, 8, stmp);
  
  if (mellomstegTom) {
    digitalWrite(13, HIGH);
  }
  else {
    digitalWrite(13, LOW);
  }
  delay(200);
}

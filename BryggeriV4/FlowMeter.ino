const byte flowMeter = 19; // Må være enten pin 2, 3, 18, 19, 20, 21
unsigned long oldtick;
void FlowMeterInit() {
  pinMode(flowMeter, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowMeter), flowtick, RISING);
}

void flowtick() { //Interrupt Service Routine for flowmeter
  tick++;
  man_tick++;
}

void flowfunction() {
  unsigned long tickdifference = tick - oldtick;
  flowperminute = (tickdifference * 60) * flowmeterkonstant;
  flowperminute = constrain(flowperminute, 0, 100);
  oldtick = tick;
}

const byte flowMeter = 19; // Må være enten pin 2, 3, 18, 19, 20, 21

void FlowMeterInit(){
  pinMode(flowMeter, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(flowMeter), flowtick, RISING);
}

void flowtick() { //Interrupt Service Routine for flowmeter
  tick++;
  man_tick++;
}

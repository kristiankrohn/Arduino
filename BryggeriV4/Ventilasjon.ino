int ventilasjonrele = 24;
int ventilasjonknapp = 23;

void init_ventilasjon(){
  pinMode(ventilasjonrele, OUTPUT);
  pinMode(ventilasjonknapp, INPUT_PULLUP);
}

void ventilasjon(){
  if (digitalRead(ventilasjonknapp) == 0){
    digitalWrite(ventilasjonrele, LOW);
  }
  else{
    digitalWrite(ventilasjonrele, HIGH);
  }
}



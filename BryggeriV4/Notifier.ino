int notifierPin = 33;
bool notify = false;

void notifierInit(){
  pinMode(notifierPin, OUTPUT);
  digitalWrite(notifierPin, HIGH);

}

void notifier(){
  if (alarm||notify){
    digitalWrite(notifierPin, LOW);
  }
  else if(Steg == 4){
    digitalWrite(notifierPin, LOW);
  }
  else if(Steg == 14){
    digitalWrite(notifierPin, LOW);
  }
  else{
    digitalWrite(notifierPin, HIGH);
  }
}

void notifierSet(){
  notify = true;
}

void notifierReset(){
  notify = false;
}



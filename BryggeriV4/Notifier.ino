int notifierPin = 33;

void notifierInit(){
  pinMode(notifierPin, OUTPUT);
  digitalWrite(notifierPin, HIGH);

}

void notifier(){
  if(Steg == 4){
    digitalWrite(notifierPin, LOW);
  }
  else if(Steg == 14){
    digitalWrite(notifierPin, LOW);
  }
  else{
    digitalWrite(notifierPin, HIGH);
  }
}


#include<kicker.h>


void Kicker::setup(){
    pinMode(K,OUTPUT);
    pinMode(C,OUTPUT);
    digitalWrite(C,HIGH);
    digitalWrite(K,LOW);
}


int Kicker::run(int kick_){
  if(kick_ == 1){
    if(kick_flag == 0){
      kick_flag = 1;
      kick_time.reset();
    }
  }

  if(kick_flag == 1){
    if(kick_time.read_ms() < 10){
      digitalWrite(C,LOW);
    }
    else if(kick_time.read_ms() < 60){
      digitalWrite(K,HIGH);
      digitalWrite(led,HIGH);
    }
    else if(kick_time.read_ms() < 70){
      digitalWrite(K,LOW);
      digitalWrite(led,LOW);
    }
    else if(kick_time.read_ms() < 80){
      digitalWrite(C,HIGH);
    }
    else{
      kick_flag = 0;
    }
  }
    return kick_flag;
}



int Kicker::stop(){
    digitalWrite(K,LOW);
    digitalWrite(C,HIGH);
    kick_flag = 0;
    kick_time.reset();
    return kick_flag;
}
#include<line.h>
#include<angle.h>


LINE::LINE(){
  // Serial6.begin(9600);
}

void LINE::begin(){
  Serial6.begin(57600);
}


int LINE::getLINE_Vec(float x, float y, int num) { //ラインのベクトル(距離,角度)を取得する関数
  this->dis_X = float(x / 100.0);
  this->dis_Y = float(y / 100.0);
  this->dis = sqrt(x*x+y*y);
  this->ang = degrees(atan2(y,x));
  this->num = num;

  if(num == 0){
    LINE_on = 0;
  }
  else{
    LINE_on = 1;
  }
  return LINE_on;
}


int LINE::switchLineflag(angle linedir){
  linedir.to_range(-45,false);
  line_flag = 0;
  for(int i = 0; i < 4; i++){  //角度を四つに区分して、それぞれどの区分にいるか判定するよ
    if(-45 +(i * 90) < linedir.degree && linedir.degree < 45 +(i * 90)){  //それ以外の三つの区分(右、後ろ、左で判定してるよ)
      line_flag = i + 1;
    }
  }
  
  return line_flag;
}


double line_switch(int,double,int);


float LINE::decideGoang(angle linedir,int line_flag){
  float goang = 0;
  linedir.to_range(-15,false);
  for(int i = 0; i < 12; i++){  //角度を12つに区分して、それぞれどの区分にいるか判定する
    if(-15 +(i * 30) < linedir.degree && linedir.degree < 15 +(i * 30)){  //時計回りにどの区分にいるか判定してるよ
      goang = line_switch(i,linedir.degree,line_flag);
    }
  }
  return goang;
}


double line_switch(int i,double ang,int line_flag){  //ラインを踏みこしてるときの処理とか判定とか書いてあるよ
  if(i == 11 || i <= 1){
    if(line_flag == 3){
      return 0.0;
    }
  }
  else if(2 <= i && i <= 4){
    if(line_flag == 4){
      return 90.0;
    }
  }
  else if(5 <= i && i <= 7){
    if(line_flag == 1){
      return 180.0;
    }
  }
  else if(8 <= i && i <= 10){
    if(line_flag == 2){
      return -90.0;
    }
  }

  double goang = (i * 30.0)- 180.0;


  return goang;
}




void LINE::print(){
  Serial.print(" 個数 : ");
  Serial.print(num);
  Serial.print(" 角度 : ");
  Serial.print(ang); //ラインのベクトルを表示
  Serial.print(" 距離 : ");
  Serial.print(dis); //ラインのベクトルを表示
  Serial.print("  X : ");
  Serial.print(dis_X); //ラインのベクトルを表示
  Serial.print("  Y : ");
  Serial.print(dis_Y); //ラインのベクトルを表示
}
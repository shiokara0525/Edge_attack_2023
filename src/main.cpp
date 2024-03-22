#include <Arduino.h>
#include<Adafruit_NeoPixel.h>
#include<ac.h>
#include<ball.h>
#include<line.h>
#include<motor_a.h>
#include<MA.h>
#include<timer.h>
#include<Cam.h>
#include<kicker.h>
#include<BLDC.h>
#include<OLED_a.h>

oled_attack OLED;
BALL ball;
LINE line;
AC ac;
motor_attack MOTOR;
Kicker kicker;
BLDC dribbler;

timer Timer;
timer Main;
int M_time;
timer L_;

int A = 0;
int B = 999;
int c = 0;
int ang_180 = 210;
int ang_90 = 170;
int ang_45 = 105;
int ang_10 = 40;
int far_th = 130;
int go_val = 240;
int go_val_back = 255;
int back_flag = 0;
int print_flag = 1;// 1だったらシリアルプリントする
int cam_front_on = 0;
int CFO_B = 999;
int Gang;
void OLED_moving();
void goang_set();

int go_flag = 0;

const int Tact_Switch[3] = {38,37,36};
int ac_val;
int GVal;

timer CFO_t;

//======================================================カメラ======================================================//

int goal_color = 1;  //青が0 黄色が1
Cam cam_front(3);
Cam cam_back(4);

//======================================================スタートスイッチ======================================================//

int LED = 13;
int toogle_f;
int toogle_P = 27;
int Target_dir;

//======================================================ライン======================================================//

int Line_flag = 0;
int line_flag_old = 0;
int Line_target_dir;
int L_time;

//======================================================関数たち======================================================/

void setup() {
  Serial.begin(9600);
  pinMode(LED,OUTPUT);
  ball.begin();
  line.begin();
  ac.setup();
  cam_front.begin();
  cam_back.begin();
  kicker.setup();
  dribbler.setup();
  dribbler.run();
  delay(200);
  dribbler.stop();
  if(goal_color == 0){
    cam_front.color = 0;  //青が0 黄色が1
    cam_back.color = 1;  //青が0 黄色が1
  }
  else if(goal_color == 1){
    cam_front.color = 1;  //青が0 黄色が1
    cam_back.color = 0;  //青が0 黄色が1
  }
  OLED.setup();
  OLED.OLED();
  go_val = OLED.val_max;
  goal_color = OLED.color;
  if(goal_color == 0){
    cam_front.color = 0;  //青が0 黄色が1
    cam_back.color = 1;  //青が0 黄色が1
  }
  else if(goal_color == 1){
    cam_front.color = 1;  //青が0 黄色が1
    cam_back.color = 0;  //青が0 黄色が1
  }
  goang_set();
  Target_dir = ac.dir_n;
  go_val = OLED.val_max;
  
}

void loop() {
  Main.reset();
  ball.getBallposition();
  cam_front.getCamdata();
  cam_back.getCamdata();
  int line_flag = line.getLINE_Vec();  //ラインセンサの入力
  angle go_ang(ball.ang,true);         //進む角度のオブジェクト

  float AC_val = 100;                  //姿勢制御の出力
  int max_val = go_val;                //進む出力
  float target = Target_dir;           //目標角度


  int AC_flag = 0;                     //0だったら絶対的な角度とる 1だったらゴール向く
  int kick_ = 0;                       //0だったらキックしない 1だったらキック
  int M_flag = 1;                      //1だったら動き続ける 0だったら止まる
  int dribbler_flag = 0;               //ドリブラーのオンオフ
  back_flag = 0;                       //ラインから逃げるときのフラグ


  //----------------------------------------------------------データの処理----------------------------------------------------------//

  c = 0;

  if(cam_front_on == 1){
  }
  else{
    CFO_B = 0;
  }

  if(line_flag == 0){
    if(line_flag_old != line_flag){
      if(5 <= Line_flag && Line_flag <= 7){
        if(30 < abs(ball.ang) && abs(ball.ang) <= 85){
          c = 1;
          A = 25;
        }
        else if(85 < abs(ball.ang) && abs(ball.ang) < 120){
          c = 1;
          A = 26;
        }
      }
    }
  }


  if(A == 25){
    if(30 < abs(ball.ang) && abs(ball.ang) < 100){
      c = 1;
    }
    else{
      c = 1;
      A = 26;
    }
    if(line_flag == 1){
      c = 0;
    }
  }
  if(A == 26){
    if(60 < abs(ball.ang) && abs(ball.ang) < 120){
      c = 1;
    }
    if(line_flag == 1){
      c = 0;
    }
  }


  if(c == 0){
    if(line_flag == 1){
      A = 20;
    }
    else{
      if(line.side_flag != 0){
        A = 21;
      }
      else{
        if(ball.flag == 1){
          if(1 <= ball.ball_get){
            A = 11;
          }
          else{
            A = 10;
          }
        }
        else{
          A = 5;
        }
      }
    }
  }


  //----------------------------------------------------------動きの決定----------------------------------------------------------//


  if(A == 5){  //ボールがない時止まる
    if(A != B){
      B = A;
    }
    M_flag = 0;
  }



  if(A == 10){  //回り込むやつ
    if(A != B){
      go_flag = 0;
      if(B == 20 || B == 21){
        if(line.ang_old < 0){
          if(-60 < ball.ang && ball.ang < 0){
            go_flag = 1;
          }
        }
        else{
          if(0 < ball.ang && ball.ang < 60){
            go_flag = 1;
          }
        }
      }
      B = A;
      Timer.reset();
    }
    int ang_180_ = ang_180;
    int ang_90_ = ang_90;
    int ang_45_ = ang_45;
    int ang_10_ = ang_10;
    if(40 < cam_front.Size){
      ang_10_ = 45;
      ang_45_ = 90;
      ang_90_ = 180;
    }

    if(45 < abs(ball.ang)){
      go_flag = 0;
    }

    if(abs(ball.ang) < 10){
      // go_ang = ang_10_ / 10.0 * ball.ang;
      go_ang = 0;
      dribbler_flag = 1;
    }
    else if(abs(ball.ang) < 45){
      if(ball.far < 178){
        ang_10_ = 90;
      }
      go_ang = ((ang_45_ - ang_10_) / 35.0 * (abs(ball.ang) - 10) + ang_10_);
      // dribbler_flag = 1;
      max_val -= 40;
    }
    else if(abs(ball.ang) < 90){
      go_ang = ((ang_90_ - ang_45_) / 45.0 * (abs(ball.ang) - 45) + ang_45_);
    }
    else{
      go_ang = ((ang_180_ - ang_90) / 90.0 * (abs(ball.ang) - 90) + ang_90_);
    }

    go_ang = go_ang.degree * (ball.ang < 0 ? -1 : 1);  //角度の正負を元に戻す

    if(go_flag == 1){
      go_ang = ball.ang;
    }
  }



  if(A == 11){  //ボール持ってるとき前進するやつ
    if(A != B){
      B = A;
      Timer.reset();
      CFO_t.reset();
    }
    cam_front_on = 0;
    dribbler_flag = 1;

    if(cam_front.on == 1){
      if(abs(cam_front.ang) < 30 && 15 < cam_front.Size){
        cam_front_on = 1;
        go_ang = 0;
        AC_flag = 1;
        // dribbler_flag = 0;
      }
      else if(abs(cam_front.ang) < 50){
        go_ang = 0;
        AC_flag = 1;
      }
      else{
        go_ang = cam_front.ang * 1.0;
      }
    }
    else{
      go_ang = 0;
    }

    if(cam_front_on == 1){
      max_val -= 15;
      if(cam_front_on != CFO_B){
        CFO_B = cam_front_on;
        CFO_t.reset();
      }

      if(200 < CFO_t.read_ms()){
        kick_ = 1;
      }
      if(40 < cam_front.Size){
        kick_ = 1;
      }
    }
    else if(cam_front_on == 0){
      CFO_B = 0;
    }
  }



  if(A == 12){
    if(A != B){
      B = A;
      Timer.reset();
    }
    M_flag = 0;
    dribbler_flag = 1;
  }



  if(A == 20){  //ラインから逃げるやつ
    angle line_ang(line.ang,true);
    if(A != B){
      B = A;
      Line_target_dir = ac.dir_n;
      Line_flag = line.switchLineflag(line_ang);
      Timer.reset();
    }
    back_flag = 1;
    // target = Line_target_dir;
    go_ang = line.decideGoang(line_ang,Line_flag);
  }



  if(A == 21){  //サイド読んでるとき逃げるやつ
    if(A != B){
      B = A;
      Timer.reset();
    }
    if(line.side_flag == 1){
      go_ang = -90;
    }
    else if(line.side_flag == 2){
      go_ang = 90;
    }
    else if(line.side_flag == 3){
      go_ang = 180;
    }
    else if(line.side_flag == 4){
      go_ang = 0;
    }
  }



  if(A == 25){  //後ろのライン読んだとき前に進むやつ
    if(A != B){
      B = A;
      Timer.reset();
    }
    go_ang = 0;
  }



  if(A == 26){  //後ろのライン読んだとき横に進むやつ
    if(A != B){
      B = A;
    }
    if(ball.ang < 0){
      go_ang = -90;
    }
    else{
      go_ang = 90;
    }
  }



  if(A == 27){
    if(A != B){
      B = A;
      Timer.reset();
    }
    if(Timer.read_ms() < 100){
      go_ang = 180;
    }
    else{
      M_flag = 0;
    }
  }


  //----------------------------------------------------------出力(ここで行ってるのはフラグの回収のみ)----------------------------------------------------------//

  ac.dir_target = target;
  if(AC_flag == 0){
    AC_val = ac.getAC_val();
  }
  else if(AC_flag == 1){
    AC_val = ac.getCam_val(-cam_front.ang);
  }

  kicker.run(kick_);

  if(dribbler_flag == 1){
    dribbler.run();
  }
  else{
    dribbler.stop();
  }

  if(back_flag == 1){
    max_val = go_val_back;
  }

  if(M_flag == 1){
    MOTOR.moveMotor_0(go_ang,max_val,AC_val,0);
  }
  else if(M_flag == 0){
    MOTOR.motor_0();
  }

  digitalWrite(LED,cam_front_on);


  if(print_flag == 1){
    Serial.print(" | A : ");
    Serial.print(A);
    // Serial.print(" | AC_val : ");
    // Serial.print(AC_val);
    Serial.print(" | goang : ");
    Serial.print(go_ang.degree);
    // Serial.print(" | ");
    // ball.print();
    // Serial.print(" | CFO : ");
    // Serial.print(CFO_t.read_ms());
    // Serial.print(" | dribller_flag : ");
    // Serial.print(dribbler_flag);
    // Serial.print(" | ");
    // line.print();
    Serial.print(" | ");
    line.print_2();
    // Serial.print(" | ");
    // ac.print();
    Serial.print(" | ");
    cam_front.print();
    Serial.print(" | ac : ");
    Serial.print(AC_val);
    // Serial.print(" | ");
    // Serial.print(L_time);
    // Serial.print(" | ");
    // Serial.print(M_time);
    Serial.println();
  }

  if(digitalReadFast(Tact_Switch[1]) == LOW){
    dribbler.stop();
    MOTOR.motor_0();
    kicker.stop();
    OLED.OLED();
    if(goal_color == 0){
      cam_front.color = 0;  //青が0 黄色が1
      cam_back.color = 1;  //青が0 黄色が1
    }
    else if(goal_color == 1){
      cam_front.color = 1;  //青が0 黄色が1
      cam_back.color = 0;  //青が0 黄色が1
    }
    go_val = OLED.val_max;
    Target_dir = ac.dir_n;
    goang_set();
  }

  if(MOTOR.NoneM_flag == 1){
    OLED_moving();
  }
  ac_val = AC_val;
  M_time = Main.read_us();
  line_flag_old = line_flag;
  Gang = go_ang.degree;
  GVal = max_val;
}


void OLED_moving(){
  //OLEDの初期化
  OLED.display.display();
  OLED.display.clearDisplay();

  //テキストサイズと色の設定
  OLED.display.setTextSize(1);
  OLED.display.setTextColor(WHITE);
  
  OLED.display.setCursor(0,0);  //1列目
  OLED.display.println("Gang");  //現在向いてる角度
  OLED.display.setCursor(30,0);
  OLED.display.println(":");
  OLED.display.setCursor(36,0);
  OLED.display.println(Gang);    //現在向いてる角度を表示

  OLED.display.setCursor(0,10);  //2列目
  OLED.display.println("Gval");  //この中に変数名を入力
  OLED.display.setCursor(30,10);
  OLED.display.println(":");
  OLED.display.setCursor(36,10);
  OLED.display.println(GVal);    //この中に知りたい変数を入力a

  OLED.display.setCursor(0,20); //3列目 
  OLED.display.println("Bang");  //この中に変数名を入力
  OLED.display.setCursor(30,20);
  OLED.display.println(":");
  OLED.display.setCursor(36,20);
  OLED.display.println(ball.ang);    //この中に知りたい変数を入力

  OLED.display.setCursor(0,30); //4列目
  OLED.display.println("A");  //この中に変数名を入力
  OLED.display.setCursor(30,30);
  OLED.display.println(":");
  OLED.display.setCursor(36,30);
  OLED.display.println(A);    //この中に知りたい変数を入力

  OLED.display.setCursor(0,40); //5列目
  OLED.display.println("B_get");  //この中に変数名を入力
  OLED.display.setCursor(30,40);
  OLED.display.println(":");
  OLED.display.setCursor(36,40);
  OLED.display.println(ball.ball_get);    //この中に知りたい変数を入力

  OLED.display.setCursor(0,50); //6列目
  OLED.display.println("time");  //この中に変数名を入力
  OLED.display.setCursor(30,50);
  OLED.display.println(":");
  OLED.display.setCursor(36,50);
  OLED.display.println(M_time);    //この中に知りたい変数を入力
}



void goang_set(){
  ang_45 = OLED.check_val[0];
  ang_90 = OLED.check_val[1];
  ang_180 = OLED.check_val[2];
}



void serialEvent3(){
  // Serial.print("sawa1");
  uint8_t reBuf[6];
  if(Serial3.available() < 6){
    return;
  }

  for(int i = 0; i < 6; i++){
    reBuf[i] = Serial3.read();
  }
  while(Serial3.available()){
    Serial3.read();
  }

  if(reBuf[0] == 38 && reBuf[5] == 37){
    for(int i = 0; i < 4; i++){
      cam_front.data_byte[i] = reBuf[i+1];
    }
  }

  // for(int i = 0; i < 6; i++){
  //   Serial.print(" ");
  //   Serial.print(reBuf[i]);
  // }
  // Serial.println();
}



void serialEvent4(){
  // Serial.print("sawa2");
  uint8_t reBuf[6];
  if(Serial4.available() < 6){
    return;
  }

  for(int i = 0; i < 6; i++){
    reBuf[i] = Serial4.read();
  }
  while(Serial4.available()){
    Serial4.read();
  }

  if(reBuf[0] == 38 && reBuf[5] == 37){
    for(int i = 0; i < 4; i++){
      cam_back.data_byte[i] = reBuf[i+1];
    }
  }

  // for(int i = 0; i < 6; i++){
  //   Serial.print(" ");
  //   Serial.print(reBuf[i]);
  // }
  // Serial.println();
}



void serialEvent6(){
  L_time = L_.read_us();
  // Serial.println(" sawa3 ");
  uint8_t read[6];
  int n = 1;
  if(Serial6.available() < 6){
    return;
  }
  read[0] = Serial6.read();
  if(read[0] != 38){
    return;
  } 
  while(0 < Serial6.available()){
    if(n < 6){
      read[n] = Serial6.read();
    }
    else{
      Serial6.read();
    }
    n++;
  }

  if(read[0] == 38 && read[5] == 37){
    line.data_byte[0] = read[1];
    line.data_byte[1] = read[2];
    line.data_byte[2] = read[3];
    line.data_byte[3] = read[4];
  }
  else{
    // Serial.print(" Error!! ");
  }
  // for(int i = 0; i < 6; i++){
  //   Serial.print(read[i]);
  //   Serial.print(" ");
  // }
  // Serial.println();
  L_.reset();
}



void serialEvent8(){
  uint8_t read[8];
  unsigned int contain[4];
  int x,y;
  if(Serial8.available() < 8){
    return;
  }
  read[0] = Serial8.read();
  if(read[0] != 0xFF){
    return;
  }
  for(int i = 1; i < 8; i++){
    read[i] = Serial8.read();
  }
  while(Serial8.available()){
    Serial8.read();
  }

  if(read[0] == 0xFF && read[7] == 0xAA){
    contain[0] = (uint16_t(read[1]) << 8);
    contain[1] = (uint16_t(read[2]));
    x = int16_t(contain[0] | contain[1]);
    contain[2] = (uint16_t(read[3]) << 8);
    contain[3] = (uint16_t(read[4]));
    y = int16_t(contain[2] | contain[3]);
    ball.ball_x.demandAve(x);
    ball.ball_y.demandAve(y);
    ball.get_resister_1(read[5]);
    ball.get_resister_2(read[6]);
  }

  // for(int i = 0; i < 8; i++){
  //   Serial.print(" ");
  //   Serial.print(read[i]);
  // }
  // Serial.println();
}
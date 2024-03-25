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
timer start_t;
timer A_24_t;

int Mode = 0;
int Mode_old = 999;

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
int back_count = 0;
int Gang;
int setup_flag = 0;
void OLED_moving();
void goang_set();

int go_flag = 0;

const int Tact_Switch[3] = {38,37,36};
int ac_val;
int GVal;
int M_F;

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
  Mode = 0;
}

void loop() {
  if(Mode == 0){
    if(Mode != Mode_old){
      Mode_old = Mode;
      OLED.display.display();
      OLED.display.clearDisplay();

      OLED.timer_OLED.reset(); //タイマーのリセット(OLED用)
      OLED.toogle = digitalRead(OLED.Toggle_Switch);
    }
    OLED.OLED();
  }
  else if(Mode == 1){

  }

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
    Serial.print(" | ");
    line.print();
    // Serial.print(" | ");
    // line.print_2();
    // Serial.print(" | ");
    // ac.print();
    Serial.print(" | ");
    cam_front.print();
    // Serial.print(" | ac : ");
    // Serial.print(AC_val);
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
    goal_color = OLED.color;
    if(goal_color == 0){
      cam_front.color = 0;  //青が0 黄色が1
      cam_back.color = 1;  //青が0 黄色が1
    }
    else if(goal_color == 1){
      cam_front.color = 1;  //青が0 黄色が1
      cam_back.color = 0;  //青が0 黄色が1
    }
    go_val = OLED.val_max;
    goang_set();
    OLED.display.fillScreen(BLACK);
    OLED.display.display();
    start_t.reset();
  }

  if(MOTOR.NoneM_flag == 1){
    OLED_moving();
  }
  ac_val = AC_val;
  M_time = Main.read_us();
  line_flag_old = line_flag;
  Gang = go_ang.degree;
  GVal = max_val;
  M_F = M_flag;
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
  OLED.display.println("M_F");  //この中に変数名を入力
  OLED.display.setCursor(30,40);
  OLED.display.println(":");
  OLED.display.setCursor(36,40);
  OLED.display.println(M_F);    //この中に知りたい変数を入力

  OLED.display.setCursor(0,50); //6列目
  OLED.display.println("B_c");  //この中に変数名を入力
  OLED.display.setCursor(30,50);
  OLED.display.println(":");
  OLED.display.setCursor(36,50);
  OLED.display.println(back_count);    //この中に知りたい変数を入力
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
#include <Arduino.h>
#include<Servo.h>
#include<Cam.h>
#include<ac.h>
#include<ball.h>
#include<line.h>
#include<motor_a.h>
#include<timer.h>

BALL ball;
int A = 0;
int B = 999;
timer Timer;
int val = 170;
AC ac;
int LED = 13;
timer t_loop;

motor_attack MOTOR;

// Servo esc;
// #define MAX_SIGNAL 2000  //PWM信号における最大のパルス幅[マイクロ秒]
// #define MIN_SIGNAL 1000  //PWM信号における最小のパルス幅[マイクロ秒]
// #define ESC_PIN 33  //ESCへの出力ピン
// float volume = 1100.0;  //可変抵抗の値を入れる変数
// char message[50];  //シリアルモニタへ表示する文字列を入れる変数
// void BLDC_work();
// void BLDC_begin();

LINE line;
int x = 0;
int y = 0;
int num = 0;
int line_first_dir = 0;
int ball_get;

int line_A = 0;
int line_B = 999;
int Line_flag = 0;
int Line_c = 0;
timer Line_timer;

const int ang_180 = 230;
const int ang_90 = 160;
const int ang_30 = 90;
const int ang_10 = 10;
const int far_th = 130;

int toogle_f;
int toogle_P = 27;
void Switch();

Cam cam_front(4);
Cam cam_back(3);
int cam_flag = 0;

int goal_color = 1; //青が0 黄色が1

int dr_p = 33;


const int C = 32;
const int K = 31;
int kick_flag = 0;
void kick();

void setup() {
  Serial.begin(9600);
  ball.begin();
  line.begin();
  cam_back.begin();
  cam_front.begin();
  ac.setup();
  pinMode(LED,OUTPUT);
  if(goal_color == 0){
    cam_front.color = 0;  //青が0 黄色が1
    cam_back.color = 1;  //青が0 黄色が1
  }
  else if(goal_color == 1){
    cam_front.color = 1;  //青が0 黄色が1
    cam_back.color = 0;  //青が0 黄色が1
  }

  pinMode(K,OUTPUT);
  pinMode(C,OUTPUT);
  digitalWrite(C,HIGH);
  // MOTOR.Moutput(4,-175);
  digitalWrite(K,LOW);
  Switch();
}



void loop() {
  //================================================状況get================================================//
  angle go_ang(0,true);
  int AC_val = 100;
  int go_val = val;

  ball.getBallposition();
  line.getLINE_Vec();
  ac.dir_target = ac.first;
  ball.ball_get = ball_get;   //ボールをキャッチしているかどうかのフラグ
  int C = 0;                  //1は継続条件を満たしている 0は継続条件を満たしていない
  int motor_flag = 1;         //1は動く 0は動かない
  int AC_flag = 1;            // 1は角度で制御 0はカメラで制御
  int dribbler_flag = 0;

  //================================================状況判断================================================//

  if(A == 40 || A == 41 || A == 42){
    C = 1;
  }

  if(C == 1){  //継続条件のほう
    if(A == 40){
      if(abs(ball.ang) < 30 || 150 < abs(ball.ang) || line.LINE_on == 1 || 500 < Timer.read_ms()){
        C = 0;
      }
    }
    else if(A == 41){
      if(2000 < Timer.read_ms() || line.LINE_on == 1){
        C = 0;
      }
    }
    else if(A == 42){
      if(95 < abs(ball.ang)){
        A = 40;
      }
    }
  }

  if(C == 0){  //継続条件じゃないほう
    if(line.LINE_on){
      A = 20;
      line_A = 1;
      if(line_A != line_B){
        line_B = line_A;
        Line_timer.reset();
        line_first_dir = ac.dir_n;
      }
      ac.dir_target = line_first_dir;
    }
    else{
      A = 10;
      line_A = 0;
      if(ball.flag == 1){
        if(ball.ball_get == 1 && abs(ball.ang) < 10){
          A = 11;
        }
        else{
          A = 10;
        }
      }
      else{
        A = 5;
      }

      if(line_A != line_B){
        Line_c = 0;
        Line_timer.reset();
        if(Line_flag == 3){
          if((60 < abs(ball.ang) && abs(ball.ang) < 120)){
            if(cam_front.Size < 20 || 50 < cam_back.Size){
              A = 40;
            }
            if(cam_back.on == 0 && cam_front.Size < 20){
              // A = 42;
            }
          }
        }
        if(Line_flag == 1){
          if((ball.ang) < 10 && (cam_front.on == 0 || 25 < abs(cam_front.ang))){
            A = 41;
          }
        }
        line_B = line_A;
      }
      if(line.side_flag == 4 && line.line_flag == 1 && Line_timer.read_ms() < 100){
        A = 22;
      }
      // if(line.side_flag == 1 || line.side_flag == 2){
      //   A = 21;
      // }
    }
  }

  //================================================動作設定================================================//

  if(A == 5){  //ボールがないときのやつ
    MOTOR.motor_0();
    ball.getBallposition();
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
  }

  if(A == 10){  //回り込むやつ
    if(A != B){
      B = A;
    }
    int ang_180_ = ang_180;
    int ang_90_ = ang_90;
    int ang_30_ = ang_30;
    int ang_10_ = ang_10;

    if(ball.ball_get == 0 && (25 < abs(ball.ang) && abs(ball.ang) < 45)){
      go_val = 120;
    }
    if(abs(ball.ang) < 10){
      go_ang = ang_10 / 10.0 * ball.ang;
    }
    else if(abs(ball.ang) < 30){
      go_ang = ((ang_30_ - ang_10_) / 20.0 * (abs(ball.ang) - 10) + ang_10_);
    }
    else if(abs(ball.ang) < 90){
      go_ang = ((ang_90_ - ang_30_) / 60.0 * (abs(ball.ang) - 30) + ang_30_);
    }
    else{
      go_ang = ((ang_180_ - ang_90) / 90.0 * (abs(ball.ang) - 90) + ang_90_);
    }

    go_ang = go_ang.degree * (ball.ang < 0 ? -1 : 1);
  }


  if(A == 11){  //ボールを捕捉して前進するやつ
    if(A != B){
      B = A;
      Timer.reset();
    }

    dribbler_flag = 1;
    AC_flag = 0;
    go_ang = 0;
    go_val = 170;

    if(kick_flag == 0 && 200 < Timer.read_ms() && ball.ball_get == 1){
      kick();
      Timer.reset();
      kick_flag = 1;
    }
    if(kick_flag == 1 && 500 < Timer.read_ms() && ball.ball_get == 1){
      kick();
      Timer.reset();
    }
  }


  if(A == 20){  //ラインから逃げるやつ
    angle line_ang(line.ang,true);
    if(A != B){
      B = A;
      Line_flag = line.switchLineflag(line_ang);
      if(Line_c == 1){
        Line_flag = 1;
      }
    }
    go_ang = line.decideGoang(line_ang,Line_flag);
  }



  if(A == 21){
    if(A != B){
      B = A;
    }
    if(line.side_flag == 1){
      if(abs(ball.ang) < 90){
        go_ang = 0;
      }
      else{
        go_ang = 180;
      }
    }
    else if(line.side_flag == 2){
      if(abs(ball.ang) < 90){
        go_ang = 0;
      }
      else{
        go_ang = 180;
      }
    }
  }


  if(A == 22){
    if(A != B){
      B = A;
    }
    go_ang = 180;
    Line_c = 1;
  }


  if(A == 40){ //ラインが後ろにある時、横にびゃーっていくやつ
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


  if(A == 41){  //後ろ行ってぽん
    if(A != B){
      B = A;
      Timer.reset();
    }
    dribbler_flag = 1;
    go_val = 100;
    if(Timer.read_ms() < 1000){
      motor_flag = 0;
    }
    go_ang = 180;
  }



  if(A == 42){
    if(A != B){
      B = A;
    }
    go_ang = 0 - ac.dir;
  }

  //================================================出力================================================//


  if(AC_flag == 1){
    AC_val = ac.getAC_val();
  }
  else{
    if(cam_front.on == 1){
      AC_val = ac.getCam_val(cam_front.ang);
      MOTOR.motor_ac(AC_val);
    }
    else{
      AC_val = ac.getAC_val();
    }
  }

  if(motor_flag == 1){
    MOTOR.moveMotor_0(go_ang,go_val,AC_val,0);
  }
  else{
    MOTOR.motor_0();
  }

  if(dribbler_flag == 1){
    MOTOR.Moutput(4,200);
  }
  else{
    MOTOR.Moutput(4,0);
  }
  Serial.print(" A : ");
  Serial.print(A);
  Serial.print(" | ");
  Serial.print(" go_ang : ");
  Serial.print(go_ang.degree);
  Serial.print(" | ");
  // Serial.print(dribbler_flag);
  // Serial.print(" | ");
  // line.print();
  // Serial.print(" | ");
  cam_front.print();
  Serial.print(" | ");
  cam_back.print();
  Serial.print(" | ");
  // ac.print();
  // Serial.print(" | ");
  ball.print();
  // Serial.print(" time : ");
  // Serial.print(t_loop.read_us());
  Serial.println();
  // t_loop.reset();

  if(toogle_f != digitalRead(toogle_P)){
    MOTOR.motor_0();
    MOTOR.Moutput(4,0);
    Switch();
    A = 0;
  }
}


void Switch(){
  digitalWrite(LED,HIGH);
  toogle_f = digitalRead(toogle_P);
  delay(100);
  while(digitalRead(toogle_P) == toogle_f);
  digitalWrite(LED,LOW);
  ac.setup_2();
  toogle_f = digitalRead(toogle_P);
  delay(100);
  while(digitalRead(toogle_P) == toogle_f);
  toogle_f = digitalRead(toogle_P);
}


void kick(){
  // esc.writeMicroseconds(1000);
  digitalWrite(C,LOW);
  delay(10);
  digitalWrite(K,HIGH);
  digitalWrite(LED,HIGH);
  delay(10);
  digitalWrite(K,LOW);
  digitalWrite(LED,LOW);
  delay(10);
  digitalWrite(C,HIGH);
  // MOTOR.Moutput(4,-200);
}


void BLDC_begin(){
  // esc.attach(ESC_PIN);  //ESCへの出力ピンをアタッチします
  // esc.writeMicroseconds(MAX_SIGNAL);  //ESCへ最大のパルス幅を指示します
  // digitalWrite(LED,HIGH);
  // delay(2000);
  // esc.writeMicroseconds(MIN_SIGNAL);  //ESCへ最小のパルス幅を指示します
  // digitalWrite(LED,LOW);
  // delay(2000);
}



void BLDC_work(){
  // esc.writeMicroseconds(1150);
}



void serialEvent3(){
  uint8_t reBuf[5];
  if(Serial3.available() < 5){
    return;
  }

  for(int i = 0; i < 5; i++){
    reBuf[i] = Serial3.read();
  }
  while(Serial3.available()){
    Serial3.read();
  }

  if(reBuf[0] == 38 && reBuf[4] == 37){
    if(reBuf[3] == 0){
      cam_back.on = 0;
    }
    else{
      if(cam_back.color == reBuf[1]){
        cam_back.on = 1;
        cam_back.Size = reBuf[3];
        cam_back.ang = -(reBuf[2] - 127);
      }
      else{
        cam_back.on = 0;
      }
    }
  }

  for(int i = 0; i < 5; i++){
    // Serial.print(reBuf[i]);
    // Serial.print(" ");
  }
  // Serial.println();
  // Serial.print("sawa");
}



void serialEvent4(){
  uint8_t reBuf[5];
  if(Serial4.available() < 5){
    return;
  }

  for(int i = 0; i < 5; i++){
    reBuf[i] = Serial4.read();
  }
  while(Serial4.available()){
    Serial4.read();
  }

  if(reBuf[0] == 38 && reBuf[4] == 37){
    if(reBuf[3] == 0){
      cam_front.on = 0;
    }
    else{
      if(cam_front.color == reBuf[1]){
        cam_front.on = 1;
        cam_front.Size = reBuf[3];
        cam_front.ang = -(reBuf[2] - 127);
      }
      else{
        cam_front.on = 0;
      }
    }
  }
  else{
  }
}



void serialEvent6(){
  // Serial.print(" sawa ");
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
}



void serialEvent8(){
  int n;
  int x,y;
  word revBuf_word[7];
  byte revBuf_byte[7];
  //受信データ数が、一定時間同じであれば、受信完了としてデータ読み出しを開始処理を開始する。
  //受信データあり ※6バイト以上になるまでまつ
  if(Serial8.available()>= 7){
    //---------------------------
    //受信データをバッファに格納
    //---------------------------
    n = 0;
    while(Serial8.available()>0 ){ //受信データがなくなるまで読み続ける
      //6バイト目まではデータを格納、それ以上は不要なデータであるため捨てる。
      if(n < 7){
        revBuf_byte[n] = Serial8.read();   //revBuf_byte[n] = Serial2.read();
      }
      else{
        Serial8.read(); //Serial2.read();  //読みだすのみで格納しない。
      }
      n++;
    }
    //---------------------------
    //データの中身を確認
    //---------------------------
    //データの先頭、終了コードあることを確認
    if((revBuf_byte[0] == 0xFF ) && ( revBuf_byte[6] == 0xAA )){
    //いったんWORD型（16bitデータ）としてから、int16_tとする。
      revBuf_word[0] = (uint16_t(revBuf_byte[1])<< 8);//上位8ビットをbyteから、Wordに型変換して格納　上位桁にするため8ビットシフト
      revBuf_word[1] = uint16_t(revBuf_byte[2]);//下位8ビットをbyteから、Wordに型変換して格納      
      x = int16_t(revBuf_word[0]|revBuf_word[1]);//上位8ビット、下位ビットを合成（ビットのORを取ることで格納する。）
      // ※int ではなく　int16_t　にすることが必要。intだけだと、32ビットのintと解釈されマイナス値がマイナスとみなされなくなる、int16_tは、16ビット指定の整数型変換
      revBuf_word[2] = (uint16_t(revBuf_byte[3])<< 8);//上位8ビットをbyteから、Wordに型変換して格納　上位桁にするため8ビットシフト
      revBuf_word[3] = uint16_t(revBuf_byte[4]);//下位8ビットをbyteから、Wordに型変換して格納      
      y = int16_t(revBuf_word[2]|revBuf_word[3]);//上位8ビット、下位ビットを合成（ビットのORを取ることで格納する。）
      // ※int ではなく　int16_t　にすることが必要。intだけだと、32ビットのintと解釈されマイナス値がマイナスとみなされなくなる、int16_tは、16ビット指定の整数型変換
      ball_get = revBuf_byte[5];

      x = ball.ball_x.demandAve(x);
      y = ball.ball_y.demandAve(y);
    }
    else{
      // printf("ERR_REV");
    }
  }
}
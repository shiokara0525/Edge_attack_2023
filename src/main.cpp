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
int val = 175;
AC ac;
int LED = 13;

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
int ball_get;

int line_A = 0;
int line_B = 999;
int Line_flag = 0;

const int ang_180 = 230;
const int ang_90 = 160;
const int ang_30 = 90;
const int ang_10 = 10;

int toogle_f;
int toogle_P = 27;
void Switch();

Cam cam_front(4);
Cam cam_back(3);
int cam_flag = 0;

int goal_color = 0; //青が0 黄色が1

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
  angle go_ang(0,true);
  int AC_val = ac.getAC_val();
  int go_val = val;

  ball.getBallposition();
  line.getLINE_Vec();
  ball.ball_get = ball_get;
  int C = 0;
  int motor_flag = 1;
  if(A == 40 || A == 41){
    C = 1;
  }

  if(C == 1){
    if(A == 40){
      if(abs(ball.ang) < 30 || 150 < abs(ball.ang) || line.LINE_on == 1){
        C = 0;
      }
    }
    else if(A == 41){
      if(1500 < Timer.read_ms() || line.LINE_on == 1){
        C = 0;
      }
    }
  }

  if(C == 0){
    if(line.LINE_on){
      A = 20;
      line_A = 1;
      if(line_A != line_B){
        line_B = line_A;
      }
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
        if(Line_flag == 3){
          if((60 < abs(ball.ang) && abs(ball.ang) < 120) && (cam_front.Size < 15 || 50 < cam_back.Size)){
            A = 40;
          }
        }
        if(Line_flag == 1){
          if(ball.ball_get == 1){
            // A = 41;
          }
        }
        line_B = line_A;
      }
    }
  }

  if(A == 5){
    MOTOR.motor_0();
    ball.getBallposition();
    digitalWrite(LED,HIGH);
    delay(100);
    digitalWrite(LED,LOW);
    delay(100);
  }

  if(A == 10){
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
      go_ang = ((ang_30_ - ang_10_) / 20.0 * (abs(ball.ang) - 10) + ang_10_)  * ball.ang / abs(ball.ang);
    }
    else if(abs(ball.ang) < 90){
      go_ang = ((ang_90_ - ang_30_) / 60.0 * (abs(ball.ang) - 30) + ang_30_) * ball.ang / abs(ball.ang);
    }
    else{
      go_ang = ((ang_180_ - ang_90_) / 90.0 * (abs(ball.ang) - 90) + ang_90_) * ball.ang / abs(ball.ang);
    }
  }


  if(A == 11){
    if(A != B){
      B = A;
      Timer.reset();
    }
    AC_val = ac.getCam_val(cam_front.ang);
    A = 90;
    go_ang = 0;
    go_val = 150;
    if(kick_flag == 0 && 300 < Timer.read_ms() && ball.ball_get == 1){
      kick();
      Timer.reset();
      kick_flag = 1;
    }
    if(kick_flag == 1 && 500 < Timer.read_ms() && ball.ball_get == 1){
      kick();
      Timer.reset();
    }
  }


  if(A == 20){
    angle line_ang(line.ang,true);
    if(A != B){
      B = A;
      Line_flag = line.switchLineflag(line_ang);
      MOTOR.motor_0();
      delay(50);
    }
    go_ang = line.decideGoang(line_ang,Line_flag);
    AC_val = ac.getAC_val();
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
    if(Timer.read_ms() < 300){
      motor_flag = 0;
    }
    else{
      go_ang = 180 - ac.dir;
    }
  }

  if(motor_flag == 1){
    MOTOR.moveMotor_0(go_ang,go_val,AC_val,0);
  }
  line.print();
  Serial.println();

  if(toogle_f != digitalRead(toogle_P)){
    MOTOR.motor_0();
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
      cam_front.on = 0;
    }
    else{
      if(cam_back.color == reBuf[1]){
        cam_back.on = 1;
        cam_back.Size = reBuf[3];
        cam_back.ang = -(reBuf[2] - 127);
      }
    }
  }
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
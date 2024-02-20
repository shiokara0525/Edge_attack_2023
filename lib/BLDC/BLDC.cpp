#include<BLDC.h>


void BLDC::setup(){
  esc.attach(ESC_PIN);  //ESCへの出力ピンをアタッチします
  Serial.println("Writing maximum output.");
  esc.writeMicroseconds(MAX_SIGNAL);  //ESCへ最大のパルス幅を指示します
  Serial.println("Wait 2 seconds.");
  delay(2000);
  Serial.println("Writing minimum output");
  esc.writeMicroseconds(MIN_SIGNAL);  //ESCへ最小のパルス幅を指示します
  Serial.println("Wait 2 seconds. Then motor starts");
  delay(2000);
}



void BLDC::run(){
  esc.writeMicroseconds(SIGNAL);  // パルス幅 `volume` のPWM信号を送信する
}



void BLDC::stop(){
  esc.writeMicroseconds(MIN_SIGNAL);  // パルス幅 `volume` のPWM信号を送信する
}
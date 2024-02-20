#include<Arduino.h>
#include<Servo.h>

#define MAX_SIGNAL 2000  //PWM信号における最大のパルス幅[マイクロ秒]
#define MIN_SIGNAL 1000  //PWM信号における最小のパルス幅[マイクロ秒]
#define ESC_PIN 33  //ESCへの出力ピン
#define SIGNAL 1800

class BLDC{
    public:
        void run();
        void stop();
        void setup();
    private:
        Servo esc;  //Servoオブジェクトを作成する．今回はESCにPWM信号を送るので，`esc`と命名している．
};
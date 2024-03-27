#pragma once


#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h>
#include <Encoder.h>
#include <timer.h>
#include<line.h>
#include<ball.h>
#include<ac.h>
#include<Cam.h>
#include<motor_a.h>
#include<Arduino.h>
#include<kicker.h>

extern AC ac;
extern Cam cam_front;
extern Cam cam_back;
extern BALL ball;
extern LINE line;
extern motor_attack MOTOR;
extern Kicker kicker;
extern int Target_dir;

/*------------------------------------------------------------------------------------------------------------*/

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3C for 128x64, 0x3D for 128x32
#define NUMFLAKES     10 // Number of snowflakes in the animation example
#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16

class oled_attack{
    public:
    Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    const int bluetooth = 11;
    timer timer_OLED;
    unsigned int address = 0x00;  //EEPROMのアドレス
    int toogle = 0;  //トグルスイッチの値を記録（トグルを引くときに使う）
    int goDir;  //loop関数ないしか使えないangle go_ang.degressの値をぶち込んでグローバルに使うために作った税
    int end_flag = 0;

    void setup();
    void OLED();
    void start();
    void end();
    void display_main();
    void display_start();
    void display_selectColor();
    void display_waitStart();
    void set_Line_Threshold();
    void display_Line();
    void set_getBall_Threshold();
    void display_Ball();
    void set_MotorVal();
    void set_Avaliable();
    void display_Cam();
    void display_getBall();
    void Kick_test();

    void check_TactSwitch();
    int Button_selectCF = 0;  //コートの方向を決めるときに特殊なことをするので、セレクト変数を変えときますぜよ
    int Sentor_A = 0;
    int Left_A = 0;
    int Right_A = 0;

    int Left;
    int Sentor;
    int Right;
    timer Left_t;
    timer Sentor_t;
    timer Right_t;

    int A = 0;
    int B = 999;  //ステート初期化のための変数
    int aa = 0;  //タクトスイッチのルーレット状態防止用変数

    int flash_OLED = 0;  //ディスプレイの中で白黒点滅させたいときにつかう
    int OLED_select = 1;  //スイッチが押されたときにどこを選択しているかを示す変数(この数字によって選択画面の表示が変化する)
    int Button_select = 0;  //スイッチが押されたときにどこを選択しているかを示す変数(この数字によってexitかnextかが決まる)


    int check_select = 0;
    int check_flag = 0;
    int check_val[6];

    const int Tact_Switch[3] = {38,37,36};
    const int Toggle_Switch = 27;  //スイッチのピン番号
    int RA_size;
    int val_max;
    int color;
    int Ball_th;
    int A_D = 1;
};
/*------------------------------------------------------------------------------------------------------------*/
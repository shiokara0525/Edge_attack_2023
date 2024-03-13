#include<ball.h>

BALL::BALL(){
    ball_x.setLenth(1);
    ball_y.setLenth(1);
    ball_get_val_1.setLenth(50);
    ball_get_val_2.setLenth(50);
    far__.setLenth(500);
}


void BALL::begin(){
    Serial8.begin(115200);
}


int BALL::getBallposition(){
    float x = ball_x.returnAve();
    float y = ball_y.returnAve();
    get_val = get_1 + get_2;

    if(x == 0 && y == 0){
        flag = 0;
        return flag;
    }
    else{
        flag = 1;
    }
    x *= 0.05;
    y *= 0.05;
    if(0 < x){
        x_pos = 130 - x;
    }
    else{
        x_pos = -130 - x;
    }
    if(0 < y){
        y_pos = 130 - y;
    }
    else{
        y_pos = -130 - y;
    }
    if(150 < abs(x)){
        x = (x < 0 ? -150 : 150);
    }
    if(150 < abs(y)){
        y = (y < 0 ? -150 : 150);
    }

    if(50 < get_1 && 50 < get_2 && (52 < get_1 || 52 < get_2) && 105 < get_val){
        ball_get = 1;
    }
    else{
        ball_get = 0;
    }
    ang = degrees(atan2(y,x));
    far = sqrt(x_pos*x_pos + y_pos*y_pos);
    dx = (far - far_old);
    far_ = far__.demandAve(far);
    x_pos = x;
    y_pos = y;
    far_old = far;
    return flag;
}


void BALL::get_resister_1(int n){
    get_1 = ball_get_val_1.demandAve(n);
}


void BALL::get_resister_2(int n){
    get_2 = ball_get_val_2.demandAve(n);
}


void BALL::print(){
    Serial.print(" ang : ");
    Serial.print(ang);
    Serial.print(" far : ");
    Serial.print(far_);
    // Serial.print(" x : ");
    // Serial.print(x_pos);
    // Serial.print(" y : ");
    // Serial.print(y_pos);
    // Serial.print(" dx : ");
    // Serial.print(dx);
    Serial.print(" get_val : ");
    Serial.print(get_val);
    Serial.print(" get : ");
    Serial.print(ball_get);
    Serial.print(" get_1 : ");
    Serial.print(get_1);
    Serial.print(" get_2 : ");
    Serial.print(get_2);
    Serial.print(" flag : ");
    Serial.print(flag);
}
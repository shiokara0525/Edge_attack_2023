#include<motor_a.h>



motor_attack::motor_attack(){
  for(int i = 0; i < 5; i++){
    pinMode(PWM_p[i][0],OUTPUT);
    pinMode(PWM_p[i][1],OUTPUT);
  }
  for(int i = 0; i < 4; i++){
    Motor[i].setLenth(motor_max);
  }
  analogWriteFrequency(3,90000);
    //モーターのピンと行列式に使う定数の設定
}




void motor_attack::moveMotor_L(angle ang,int val,double ac_val,LINE line){  //モーター制御する関数
  double g = 0;                //モーターの最終的に出る最終的な値の比の基準になる値
  double h = 0;
  double Mval[4] = {0,0,0,0};  //モーターの値×4
  double max_val = val;        //モーターの値の上限値
  double mval_x = cos(ang.radians);  //進みたいベクトルのx成分
  double mval_y = sin(ang.radians);  //進みたいベクトルのy成分
  
  max_val -= ac_val;  //姿勢制御とその他のモーターの値を別に考えるために姿勢制御の値を引いておく
  
  for(int i = 0; i < 4; i++){
    Mval[i] = -mSin[i] *(mval_x + line.dis_X * line_val)  + mCos[i] *(mval_y + line.dis_Y * line_val);

    if(abs(Mval[i]) > g){  //絶対値が一番高い値だったら
      g = abs(Mval[i]);    //一番大きい値を代入
    }
  }

  for(int i = 0; i < 4; i++){  //移動平均求めるゾーンだよ
    Mval[i] /= g;  //モーターの値を制御(常に一番大きい値が1になるようにする)

    Mval[i] = Motor[i].demandAve(Mval[i]);

    if(abs(Mval[i]) > h){  //絶対値が一番高い値だったら
      h = abs(Mval[i]);    //一番大きい値を代入
    }
  }

  for(int i = 0; i < 4; i++){  //モーターの値を計算するところだよ
    
    if(i == 0 || i == 3){
      Mval[i] = Mval[i] / h * max_val;  //モーターの値を計算(進みたいベクトルの値と姿勢制御の値を合わせる)
    }
    else{
      Mval[i] = Mval[i] / h * max_val + ac_val * 1.3;  //モーターの値を計算(進みたいベクトルの値と姿勢制御の値を合わせる)
    }
    Moutput(i,Mval[i]);
  }
}


void motor_attack::moveMotor_0(angle ang,int val,double ac_val,int flag){
  double g = 0;                //モーターの最終的に出る最終的な値の比の基準になる値
  double h = 0;
  double Mval[4] = {0,0,0,0};  //モーターの値×4
  double max_val = val;        //モーターの値の上限値
  double mval_x = cos(ang.radians);  //進みたいベクトルのx成分
  double mval_y = sin(ang.radians);  //進みたいベクトルのy成分
  
  max_val -= ac_val;  //姿勢制御とその他のモーターの値を別に考えるために姿勢制御の値を引いておく
  
  for(int i = 0; i < 4; i++){
    Mval[i] = -mSin[i] * mval_x + mCos[i] * mval_y; //モーターの回転速度を計算(行列式で管理)
    
    if(abs(Mval[i]) > g){  //絶対値が一番高い値だったら
      g = abs(Mval[i]);    //一番大きい値を代入
    }
  }

  for(int i = 0; i < 4; i++){  //移動平均求めるゾーンだよ
    Mval[i] /= g;  //モーターの値を制御(常に一番大きい値が1になるようにする)

    Mval[i] = Motor[i].demandAve(Mval[i]);

    if(abs(Mval[i]) > h){  //絶対値が一番高い値だったら
      h = abs(Mval[i]);    //一番大きい値を代入
    }
  }

  for(int i = 0; i < 4; i++){  //モーターの値を計算するところだよ
    if(flag == 0){
      Mval[i] = Mval[i] / h * max_val + ac_val;  //モーターの値を計算(進みたいベクトルの値と姿勢制御の値を合わせる)
    }
    else if(flag == 1){
      if(i == 0 || i == 3){
        Mval[i] = Mval[i] / h * max_val;  //モーターの値を計算(進みたいベクトルの値と姿勢制御の値を合わせる)
      }
      else{
        Mval[i] = Mval[i] / h * max_val + ac_val * 1.7;  //モーターの値を計算(進みたいベクトルの値と姿勢制御の値を合わせる)
      }
    }
    Moutput(i,Mval[i]);
  }
}


void motor_attack::motor_ac(float ac_val){
  for(int i = 0; i < 4; i++){
    Moutput(i,ac_val);
  }
}


void motor_attack::motor_0(){  //モーターの値を0にする関数
  for(int i = 0; i < 4; i++){
    Moutput(i,0);
  }
}



float motor_attack::Moutput(int i,float Mval){
  if(0 < Mval){
    analogWrite(PWM_p[i][0],0);
    analogWrite(PWM_p[i][1],abs(Mval));
  }
  else if(Mval < 0){
    analogWrite(PWM_p[i][0],abs(Mval));
    analogWrite(PWM_p[i][1],0);
  }
  else if(Mval == 0){
    analogWrite(PWM_p[i][0],0);
    analogWrite(PWM_p[i][1],0);
  }
  return Mval;
}
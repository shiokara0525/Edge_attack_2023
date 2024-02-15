#pragma once

#include<timer.h>
#include<MA.h>

class Cam{
    private:
        int B = 999;
        int A = 0;
        int F = 0;
    public:
        Cam(int);
        int data_byte[4];
        int getCamdata();
        float ang;
        float Size;
        int on = 0;
        int color = 0;
        int senter;
        MA ang_;
        void print();
        void begin();
};
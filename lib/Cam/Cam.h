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
        void begin();
        float ang;
        float Size;
        void print();
        int on = 0;
        int color = 0;
        MA csize;
};
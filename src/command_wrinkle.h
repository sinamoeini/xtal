#ifdef Command_Style
    CommandStyle(Command_wrinkle,wrinkle)
#else
#ifndef __xtal__command_wrinkle__
#define __xtal__command_wrinkle__

#include <stdio.h>
#include "init.h"
class Command_wrinkle: protected InitPtrs
{
private:
    int N;
    type0 m,no,l,w,r,theta;
    
    type0 energy();
    type0 force();
    void solve();
    void md();
    
    type0* x;
    type0* x0;
    type0* xx0;
    type0* f;
    type0* f0;
    type0* h;
    
    type0 r1(type0,type0);
    type0 r2(type0,type0);
    void test();
    
    int max_iter;
    type0 norm,slope,tol;
protected:
public:
    Command_wrinkle(Xtal*,int,char**);
    ~Command_wrinkle();
};
#endif
#endif

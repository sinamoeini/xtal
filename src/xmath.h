#ifndef __xtal__xmath__
#define __xtal__xmath__
#include <stdio.h>
#include "init.h"
class XMath: protected InitPtrs
{
private:
protected:
public:
    XMath(Xtal*);
    ~XMath();
    void square2lo_tri(type0**,type0**);
    type0 solve_sinx_ov_x(type0);
    type0 bracket(type0((type0)),type0,type0);
    
};
#endif

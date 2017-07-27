#ifdef Command_Style
    CommandStyle(Command_dislocation,dislocation)
#else
#ifndef __xtal__command_dislocation__
#define __xtal__command_dislocation__
#include "init.h"
#include <stdio.h>
class Command_dislocation: protected InitPtrs
{
private:
    type0 nu,b_edge,b_screw,y_scale,x_scale,ux_scale,uy_scale,uz_scale;
    void disp(type0,type0,type0&,type0&);

    void disp(type0,type0,type0&);
    void disp(type0,type0,type0&,type0&,type0&);
    int n[3];
    int n_edge,n_screw;
    void correct_s(type0&);
protected:
public:
    Command_dislocation(Xtal*,int,char**);
    ~Command_dislocation();
    
};
#endif
#endif

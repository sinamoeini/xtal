#ifdef Command_Style
    CommandStyle(Command_sine_arc,sine_arc)
#else

#ifndef __xtal__command_sine_arc__
#define __xtal__command_sine_arc__

#include <stdio.h>
#include "init.h"
class Command_sine_arc: protected InitPtrs
{
private:
    type0 elliptic_e(type0);
    type0 elliptic_e(type0,type0);
    type0 solve(type0);
    inline void mid_map(type0,type0,type0
     ,type0,type0,type0&,type0&);
protected:
public:
    Command_sine_arc(Xtal*,int,char**);
    ~Command_sine_arc();
};

#endif
#endif

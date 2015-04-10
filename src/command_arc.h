#ifdef Command_Style
    CommandStyle(Command_arc,arc)
#else
#ifndef __xtal__command_arc__
#define __xtal__command_arc__

#include <stdio.h>
#include "init.h"
class Command_arc: protected InitPtrs
{
private:

    inline void mid_map(type0,type0,type0
    ,type0,type0,type0&,type0&);
    
    type0 r1(type0,type0);
    type0 r2(type0,type0);
    type0 n1(type0,type0);
    type0 n2(type0,type0);
    
    type0 l,w;
protected:
public:
    Command_arc(Xtal*,int,char**);
    ~Command_arc();
};


#endif
#endif

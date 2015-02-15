#ifdef Command_Style
    CommandStyle(Command_cfg2box,cfg2box)
#else
#ifndef __xtal__command_cfg2box__
#define __xtal__command_cfg2box__
#include "init.h"
#include <stdio.h>
class Command_cfg2box: protected InitPtrs
{
private:
protected:
public:
    Command_cfg2box(Xtal*,int,char**);
    ~Command_cfg2box();
    
};

#endif
#endif

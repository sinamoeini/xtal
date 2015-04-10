#ifdef Command_Style
    CommandStyle(Command_vasp2box,vasp2box)
#else
#ifndef __xtal__command_vasp2box__
#define __xtal__command_vasp2box__
#include "init.h"
#include <stdio.h>
class Command_vasp2box: protected InitPtrs
{
private:
protected:
public:
    Command_vasp2box(Xtal*,int,char**);
    ~Command_vasp2box();
    
};

#endif
#endif 

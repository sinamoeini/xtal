#ifdef Command_Style
    CommandStyle(Command_rotate,rotate)
#else
#ifndef __xtal__command_rotate__
#define __xtal__command_rotate__
#include "init.h"
#include <stdio.h>
class Command_rotate: protected InitPtrs
{
private:
protected:
public:
    Command_rotate(Xtal*,int,char**);
    ~Command_rotate();
    
};
#endif
#endif

#ifdef Command_Style
    CommandStyle(Command_mul,mul)
#else
#ifndef __xtal__command_mul__
#define __xtal__command_mul__
#include "init.h"
#include <stdio.h>
class Command_mul: protected InitPtrs
{
private:
protected:
public:
    Command_mul(Xtal*,int,char**);
    ~Command_mul();
    
};
#endif
#endif

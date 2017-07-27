#ifdef Command_Style
    CommandStyle(Command_cp,cp)
#else
#ifndef __xtal__command_cp__
#define __xtal__command_cp__
#include "init.h"
#include <stdio.h>
class Command_cp: protected InitPtrs
{
private:
protected:
public:
    Command_cp(Xtal*,int,char**);
    ~Command_cp();
    
};
#endif
#endif

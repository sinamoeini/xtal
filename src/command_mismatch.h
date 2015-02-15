#ifdef Command_Style
    CommandStyle(Command_mismatch,mismatch)
#else
#ifndef __xtal__command_mismatch__
#define __xtal__command_mismatch__
#include "init.h"
#include <stdio.h>
class Command_mismatch: protected InitPtrs
{
private:
protected:
public:
    Command_mismatch(Xtal*,int,char**);
    ~Command_mismatch();
    
};
#endif
#endif

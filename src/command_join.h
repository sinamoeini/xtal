#ifdef Command_Style
    CommandStyle(Command_join,join)
#else
#ifndef __xtal__command_join__
#define __xtal__command_join__
#include "init.h"
#include <stdio.h>
class Command_join: protected InitPtrs
{
private:
protected:
public:
    Command_join(Xtal*,int,char**);
    ~Command_join();
    
};

#endif
#endif

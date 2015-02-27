#ifdef Command_Style
    CommandStyle(Command_add_region,add_region)
#else
#ifndef __xtal__command_add_region__
#define __xtal__command_add_region__
#include "init.h"
#include <stdio.h>
class Command_add_region: protected InitPtrs
{
private:
protected:
public:
    Command_add_region(Xtal*,int,char**);
    ~Command_add_region();
    
};

#endif
#endif
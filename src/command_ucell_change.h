#ifdef Command_Style
    CommandStyle(Command_ucell_change,ucell_change)
#else
#ifndef __xtal__command_ucell_change__
#define __xtal__command_ucell_change__

#include <stdio.h>
#include "init.h"
class Command_ucell_change: protected InitPtrs
{
private:
protected:
public:
    Command_ucell_change(Xtal*,int,char**);
    ~Command_ucell_change();
    
};

#endif
#endif

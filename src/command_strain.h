#ifdef Command_Style
    CommandStyle(Command_strain,strain)
#else
#ifndef __xtal__command_strain__
#define __xtal__command_strain__

#include <stdio.h>
#include <stdio.h>
#include "init.h"
class Command_strain: protected InitPtrs
{
private:
protected:
public:
    Command_strain(Xtal*,int,char**);
    ~Command_strain();
};
#endif
#endif

#ifdef Command_Style
    CommandStyle(Command_add_vac,add_vac)
#else
#ifndef __xtal__command_add_vac__
#define __xtal__command_add_vac__
#include "init.h"
#include <stdio.h>
class Command_add_vac: protected InitPtrs
{
private:
protected:
public:
    Command_add_vac(Xtal*,int,char**);
    ~Command_add_vac();
    
};
#endif
#endif

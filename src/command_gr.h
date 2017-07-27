#ifdef Command_Style
    CommandStyle(Command_gr,gr)
#else
#ifndef __xtal__command_gr__
#define __xtal__command_gr__
#include "init.h"
#include <stdio.h>
class Command_gr: protected InitPtrs
{
private:
protected:
public:
    Command_gr(Xtal*,int,char**);
    ~Command_gr();
    
};
#endif
#endif

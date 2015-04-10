#ifdef Command_Style
    CommandStyle(Command_fix_id,fix_id)
#else
#ifndef __xtal__command_fix_id__
#define __xtal__command_fix_id__

#include "init.h"
#include <stdio.h>
class Command_fix_id: protected InitPtrs
{
private:
    char* idof;
    int* id_list;
protected:
public:
    Command_fix_id(Xtal*,int,char**);
    ~Command_fix_id();
    
};

#endif
#endif

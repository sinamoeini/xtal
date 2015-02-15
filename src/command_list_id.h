#ifdef Command_Style
    CommandStyle(Command_list_id,list_id)
#else
#ifndef __xtal__command_list_id__
#define __xtal__command_list_id__
#include <stdio.h>
#include "init.h"
class Command_list_id: protected InitPtrs
{
private:
protected:
public:
    Command_list_id(Xtal*,int,char**);
    ~Command_list_id();
    
};
#endif
#endif

#ifdef Command_Style
    CommandStyle(Command_box_prop,box_prop)
#else
#ifndef __xtal__command_box_prop__
#define __xtal__command_box_prop__
#include "init.h"
#include <stdio.h>
class Command_box_prop: protected InitPtrs
{
private:
protected:
public:
    Command_box_prop(Xtal*,int,char**);
    ~Command_box_prop();
    
};

#endif
#endif

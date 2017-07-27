#ifdef Command_Style
    CommandStyle(Command_rm_thickness,rm_thickness)
#else
#ifndef __xtal__command_rm_thickness__
#define __xtal__command_rm_thickness__
#include "init.h"
#include <stdio.h>
class Command_rm_thickness: protected InitPtrs
{
private:
protected:
public:
    Command_rm_thickness(Xtal*,int,char**);
    ~Command_rm_thickness();
    
};
#endif
#endif

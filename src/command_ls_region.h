#ifdef Command_Style
    CommandStyle(Command_ls_region,ls_region)
#else
#ifndef __xtal__command_ls_region__
#define __xtal__command_ls_region__
#include "init.h"
#include <stdio.h>
class Command_ls_region: protected InitPtrs
{
private:
protected:
public:
    Command_ls_region(Xtal*,int,char**);
    ~Command_ls_region();
    
};
#endif
#endif 

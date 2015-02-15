#ifdef Command_Style
    CommandStyle(Command_ls_box,ls_box)
#else
#ifndef __xtal__command_ls_box__
#define __xtal__command_ls_box__
#include "init.h"
#include <stdio.h>
class Command_ls_box: protected InitPtrs
{
private:
protected:
public:
    Command_ls_box(Xtal*,int,char**);
    ~Command_ls_box();
    
};
#endif
#endif

#ifdef Command_Style
    CommandStyle(Command_box2cfg,box2cfg)
#else
#ifndef __xtal__command_box2cfg__
#define __xtal__command_box2cfg__
#include "init.h"
#include <stdio.h>
class Command_box2cfg: protected InitPtrs
{
private:
protected:
public:
    Command_box2cfg(Xtal*,int,char**);
    ~Command_box2cfg();
    
};

#endif
#endif

#ifdef Command_Style
    CommandStyle(Command_help,help)
#else
#ifndef __xtal__command_help__
#define __xtal__command_help__
#include "init.h"
#include <stdio.h>
class Command_help: protected InitPtrs
{
private:
protected:
public:
    Command_help(Xtal*,int,char**);
    ~Command_help();
    
};

#endif 
#endif 

#ifdef Command_Style
    CommandStyle(Command_move,move)
#else
#ifndef __xtal__command_move__
#define __xtal__command_move__
#include "init.h"
#include <stdio.h>
class Command_move: protected InitPtrs
{
private:
protected:
public:
    Command_move(Xtal*,int,char**);
    ~Command_move();
    
};
#endif
#endif
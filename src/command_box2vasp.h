#ifdef Command_Style
    CommandStyle(Command_box2vasp,box2vasp)
#else
#ifndef __xtal__command_box2vasp__
#define __xtal__command_box2vasp__
#include "init.h"
#include <stdio.h>
class Command_box2vasp: protected InitPtrs
{
private:
protected:
public:
    Command_box2vasp(Xtal*,int,char**);
    ~Command_box2vasp();
    
};
#endif
#endif

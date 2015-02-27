#ifdef Command_Style
    CommandStyle(Command_del_atoms,del_atoms)
#else
#ifndef __xtal__command_del_atoms__
#define __xtal__command_del_atoms__
#include <stdio.h>
#include "init.h"
class Command_del_atoms: protected InitPtrs
{
private:
    int include;
    int seed;
    type0 rand_gen();
protected:
public:
    Command_del_atoms(Xtal*,int,char**);
    ~Command_del_atoms();
    
};
#endif
#endif

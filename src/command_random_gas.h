#ifdef Command_Style
    CommandStyle(Command_random_gas,random_gas)
#else
#ifndef __xtal__command_random_gas__
#define __xtal__command_random_gas__
#include "init.h"
#include <stdio.h>
class Command_random_gas: protected InitPtrs
{
private:
    int seed;
    type0 bond;
    void rand_gen(type0**&,type0*,type0*);
    type0 rand_gen();
protected:
public:
    Command_random_gas(Xtal*,int,char**);
    ~Command_random_gas();
};
#endif
#endif

#ifndef __xtal__box2lammps__
#define __xtal__box2lammps__

#include <stdio.h>
#include "init.h"
class Box2LAMMPS :protected InitPtrs
{
private:
    FILE* lammpsfile;

    
    
protected:
public:
    
    Box2LAMMPS(Xtal*,char*,char*);
    ~Box2LAMMPS();
    
    
    
};

#endif

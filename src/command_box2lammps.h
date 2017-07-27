#ifdef Command_Style
    CommandStyle(Command_box2lammps,box2lammps)
#else
#ifndef __xtal__command_box2lammps__
#define __xtal__command_box2lammps__
#include "init.h"
#include <stdio.h>
class Command_box2lammps: protected InitPtrs
{
private:
protected:
public:
    Command_box2lammps(Xtal*,int,char**);
    ~Command_box2lammps();
    
};

#endif
#endif

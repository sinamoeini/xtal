#include "command_box2lammps.h"
#include "error.h"
#include "memory.h"
#include "box_collection.h"
#include "box2lammps.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_box2lammps::Command_box2lammps(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    
    if(narg!=3)
    {
        error->warning("box2lammps command needs 2 arguments\n"
        "SYNTAX: box2lammps box file");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    
    if(ibox<0)
    {
        error->warning("%s box not found",arg[1]);
        return;
    }
    
    class Box2LAMMPS* box2lammps=new Box2LAMMPS(xtal,arg[1],arg[2]);
    
    delete box2lammps;
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_box2lammps::~Command_box2lammps()
{
}


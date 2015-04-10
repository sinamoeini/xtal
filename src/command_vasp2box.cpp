#include "command_vasp2box.h"
#include "error.h"
#include "memory.h"
#include "box_collection.h"
#include "vasp2box.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_vasp2box::Command_vasp2box(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    if(narg!=3)
    {
        error->warning("cfg2box command needs 2 arguments\n"
                       "SYNTAX: cfg2box box file.cfg");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    if(ibox>=0)
    {
        box_collection->del(arg[1]);
    }
    box_collection->add(arg[1]);
    
    
    xtal->error_flag=0;
    
    class VASP2Box* vasp2box=new VASP2Box(xtal,arg[1],arg[2]);
    delete vasp2box;
    
    if(xtal->error_flag==-1)
    {
        box_collection->del(arg[1]);
        xtal->error_flag=0;
    }
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_vasp2box::~Command_vasp2box()
{   
}


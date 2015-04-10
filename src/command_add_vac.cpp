#include "command_add_vac.h"
#include "error.h"
#include "memory.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_add_vac::Command_add_vac(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    if(narg!=4)
    {
        error->warning("add_vac command needs 3 arguments\n"
        "SYNTAX: add_vac box dim vaccuum_thickness");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
    
    type0 depth;
    int dim;
    if(strcmp(arg[2],"x")==0)
    {
        dim=0;
    }
    else if(strcmp(arg[2],"y")==0)
    {
        dim=1;
    }
    else if(strcmp(arg[2],"z")==0)
    {
        dim=2;
    }
    else
    {
        error->warning("invalid dimension: %s",arg[2]);
        return;
    }
    depth=atof(arg[3]);
    
    if(depth<=0.0)
    {
        error->warning("thickness of vaccuum cannot be less than 0.0");
        return;
    }
    
    
    box_collection->boxes[ibox]->add_vac(dim,depth);
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_add_vac::~Command_add_vac()
{
}

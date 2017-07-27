#include "command_rm_thickness.h"
#include "error.h"
#include "memory.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_rm_thickness::Command_rm_thickness(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    if(narg!=5)
    {
        error->warning("rm_thickness command needs 4 arguments\n"
        "SYNTAX: rm_thickness box dim s_lo s_hi");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
    

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
    
    type0 s_lo,s_hi;
    
    s_lo=atof(arg[3]);
    s_hi=atof(arg[4]);
    
    if(s_lo<0.0 || s_lo>1.0)
    {
        error->warning("s_lo must be between 0.0 & 1.0");
        return;
    }
    if(s_hi<0.0 || s_hi>1.0)
    {
        error->warning("s_hi must be between 0.0 & 1.0");
        return;
    }
    if(s_lo>=s_hi)
    {
        error->warning("s_hi must be greater than s_lo");
        return;
    }

    
    box_collection->boxes[ibox]->rm_frac(dim,s_lo,s_hi);
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_rm_thickness::~Command_rm_thickness()
{
}

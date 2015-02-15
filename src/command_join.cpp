#include "command_join.h"
#include "error.h"
#include "memory.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_join::Command_join(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    
    if(narg!=5)
    {
        error->warning("join command needs 4 arguments\n"
        "SYNTAX: join box box_0 box_1 dim");
        return;
    }
    
    int ibox0=box_collection->find(arg[2]);
    int ibox1=box_collection->find(arg[3]);
    if(ibox0<0)
    {
        error->warning("%s box not found",arg[2]);
        return;
    }
    if(ibox1<0)
    {
        error->warning("%s box not found",arg[3]);
        return;
    }
    int dim;
    if(strcmp(arg[4],"x")==0)
    {
        dim=0;
    }
    else if(strcmp(arg[4],"y")==0)
    {
        dim=1;
    }
    else if(strcmp(arg[4],"z")==0)
    {
        dim=2;
    }
    else
    {
        error->warning("invalid dimension: %s",arg[4]);
        return;
    }
    
    box_collection->join(arg[1],arg[2],arg[3],dim);
    

    
    
    
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_join::~Command_join()
{
}


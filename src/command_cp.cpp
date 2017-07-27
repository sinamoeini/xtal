#include "command_cp.h"
#include "memory.h"
#include "error.h"
#include "box2cfg.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_cp::Command_cp(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{

    if(narg!=3)
    {
        error->warning("cp command needs 2 arguments\n"
        "SYNTAX: cp src_box des_box");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
    
    
    int jbox=box_collection->find(arg[2]);
    if(jbox>=0)
    {
        if(jbox!=ibox)
            box_collection->del(jbox);
        else
            return;
    }
    
    box_collection->cp(arg[1],arg[2]);
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_cp::~Command_cp()
{
}



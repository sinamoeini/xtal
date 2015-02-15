#include "command_box2cfg.h"
#include "error.h"
#include "memory.h"
#include "box_collection.h"
#include "box2cfg.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_box2cfg::Command_box2cfg(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    
    if(narg!=3)
    {
        error->warning("cfg2box command needs 2 arguments\n"
        "SYNTAX: box2cfg box file.cfg");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    
    if(ibox<0)
    {
        error->warning("%s box not found",arg[1]);
        return;
    }
    
    box_collection->add(arg[1]);
    
    class Box2CFG* box2cfg=new Box2CFG(xtal,arg[1],arg[2]);
    
    delete box2cfg;
    
    
    
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_box2cfg::~Command_box2cfg()
{
}


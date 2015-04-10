#include "command_box2vasp.h"
#include "error.h"
#include "memory.h"
#include "box_collection.h"
#include "box2vasp.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_box2vasp::Command_box2vasp(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    
    if(narg!=3)
    {
        error->warning("box2vasp command needs 2 arguments\n"
            "SYNTAX: box2vasp box file");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    
    if(ibox<0)
    {
        error->warning("%s box not found",arg[1]);
        return;
    }
    /*
    
    if(!box_collection->boxes[ibox]->dof_xst)
    {
        error->warning("box %s does not have dof set",arg[1]);
        return;
    }
    */
    
    class Box2VASP* box2vasp=new Box2VASP(xtal,arg[1],arg[2]);
    
    delete box2vasp;
    
    
    
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_box2vasp::~Command_box2vasp()
{
}



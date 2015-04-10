#include "command_mul.h"
#include "memory.h"
#include "error.h"
#include "box2cfg.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_mul::Command_mul(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{

    if(narg!=5)
    {
        error->warning("mul command needs 4 arguments\n"
        "SYNTAX: mul box N_x N_y N_z");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
    
    int* n;
    CREATE1D(n,3);
    n[0]=atoi(arg[2]);
    n[1]=atoi(arg[3]);
    n[2]=atoi(arg[4]);
    
    for(int i=0;i<3;i++)
    {
        if(n[0]<=0)
        {
            error->warning("mul argumets should be larger than 0");
            delete [] n;
            return;
        }
    }
    
    
    box_collection->boxes[ibox]->mul(n);
    delete [] n;
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_mul::~Command_mul()
{
}



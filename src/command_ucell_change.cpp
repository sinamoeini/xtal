#include "command_ucell_change.h"
#include "box_collection.h"
#include "memory.h"
#include "error.h"
#include <math.h>
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_ucell_change::Command_ucell_change(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    /*
    error->warning("ucell_change not ready yet");
    return;
    */
    
    if(narg!=5)
    {
        error->warning("ucell_change command needs 4 arguments\n"
        "SYNTAX: ucell_change box [H00,H01,H02] [H10,H11,H12] [H20,H21,H22]");
        return;
    }

    
    int ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
        
    type0** u;
    CREATE2D(u,3,3);
    
    int tmp0,tmp1,tmp2;
    for(int i=0;i<3;i++)
    {
        if(sscanf(arg[2+i],"[%d,%d,%d]",&tmp0,&tmp1,&tmp2)==3)
        {
            u[i][0]=static_cast<type0>(tmp0);
            u[i][1]=static_cast<type0>(tmp1);
            u[i][2]=static_cast<type0>(tmp2);
        }
        else
        {
            error->warning("H_%d should be of the format: [H_%d0,H_%d1,H_%d2]",i,i,i,i);
            for(int i=0;i<3;i++)
                delete [] u[i];
            delete [] u;
            return;
            
        }
        
    }
    
    //suppose that u is set
    type0 det_u=M3DET(u);
    if(det_u<=0)
    {
        error->warning("determinat of new H is negative");
        for(int i=0;i<3;i++)
            delete [] u[i];
        delete [] u;
        return;
    }
    
    box_collection->boxes[ibox]->ucell_chang(u);
    
    for(int i=0;i<3;i++)
        delete [] u[i];
    delete [] u;
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_ucell_change::~Command_ucell_change()
{
}


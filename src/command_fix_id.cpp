#include "command_fix_id.h"
#include "error.h"
#include "memory.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_fix_id::Command_fix_id(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    int ibox,nfatms;
    
    ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
    
    
    CREATE1D(idof,3);
    idof[0]=idof[1]=idof[2]='0';
    
    if(strcmp(arg[2],"x")==0)
    {
        idof[0]='1';
    }
    else if(strcmp(arg[2],"y")==0)
    {
        idof[1]='1';
    }
    else if(strcmp(arg[2],"z")==0)
    {
        idof[2]='1';
    }
    else if(strcmp(arg[2],"xy")==0)
    {
        idof[0]=idof[1]='1';
    }
    else if(strcmp(arg[2],"yx")==0)
    {
        idof[0]=idof[1]='1';
    }
    else if(strcmp(arg[2],"yz")==0)
    {
        idof[2]=idof[1]='1';
    }
    else if(strcmp(arg[2],"zy")==0)
    {
        idof[2]=idof[1]='1';
    }
    else if(strcmp(arg[2],"zx")==0)
    {
        idof[2]=idof[0]='1';
    }
    else if(strcmp(arg[2],"xz")==0)
    {
        idof[2]=idof[0]='1';
    }
    else if(strcmp(arg[2],"xyz")==0)
    {
        idof[0]=idof[1]=idof[2]='1';
    }
    else if(strcmp(arg[2],"xzy")==0)
    {
        idof[0]=idof[1]=idof[2]='1';
    }
    else if(strcmp(arg[2],"yzx")==0)
    {
        idof[0]=idof[1]=idof[2]='1';
    }
    else if(strcmp(arg[2],"yxz")==0)
    {
        idof[0]=idof[1]=idof[2]='1';
    }
    else if(strcmp(arg[2],"zxy")==0)
    {
        idof[0]=idof[1]=idof[2]='1';
    }
    else if(strcmp(arg[2],"zyx")==0)
    {
        idof[0]=idof[1]=idof[2]='1';
    }
    else
    {
        error->warning("unknown dof %s\n",arg[2]);
        delete [] idof;
        return;
    }
    
    
    CREATE1D(id_list,narg-3);

    for(int i=3;i<narg;i++)
        id_list[i-3]=atoi(arg[i]);
    
    nfatms=narg-3;

    Box* box=box_collection->boxes[ibox];
    for(int i=0;i<nfatms;i++)
    {
        if(id_list[i]<0 || id_list[i]>box->natms)
        {
            error->warning("atom id must be between 0 and %d\n",box->natms);
            delete [] id_list;
            delete [] idof;
            return;
        }
    }
    
    box->id_correction();
    
    if(box->dof_xst==0)
    {
        CREATE1D(box->dof,3*box->natms);
        for(int i=0;i<3*box->natms;i++)
            box->dof[i]='0';
        box->dof_xst=1;
    }
    
    for(int i=0;i<nfatms;i++)
        memcpy(&box->dof[3*id_list[i]],idof,3*sizeof(char));
    
    delete [] id_list;
    delete [] idof;
    
    
    
    
}
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_fix_id::~Command_fix_id()
{
}

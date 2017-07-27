#include "command_move.h"
#include "memory.h"
#include "error.h"
#include "box2cfg.h"
#include "box_collection.h"
#include "region_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_move::Command_move(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    
    if(narg!=6 && narg!=7)
    {
        error->warning("move command needs 5 arguments\n"
        "SYNTAX: move x box dx dy dz\n"
        "or\n"
        "SYNTAX: move s box ds_x ds_y ds_z\n"
        "or\n"
        "SYNTAX: move region x box dx dy dz\n"
        "or\n"
        "SYNTAX: move region s box ds_x ds_y ds_z\n");
        return;
    }
    int iarg=1;
    
    Region* region=NULL;
    if(narg==7)
    {
        int iregion=region_collection->find(arg[1]);
        if(iregion<0)
        {
            error->warning("region %s not found",arg[1]);
            return;
        }
        region=region_collection->regions[iregion];
        iarg++;
    }
    
    
    
    
    int mode;
    if(strcmp(arg[iarg],"s")==0)
        mode=0;
    else if(strcmp(arg[iarg],"x")==0)
        mode=1;
    else
    {
        error->warning("displacement mode %s is invalid (s or x)",arg[iarg]);
        return;
    }
    
    iarg++;
    int ibox=box_collection->find(arg[iarg]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[iarg]);
        return;
    }
    
    iarg++;
    type0* dx;
    type0* ds;
    CREATE1D(dx,3);
    CREATE1D(ds,3);
    dx[0]=atof(arg[iarg++]);
    dx[1]=atof(arg[iarg++]);
    dx[2]=atof(arg[iarg++]);
    
    ds[0]=ds[1]=ds[2]=0.0;

    Box* box=box_collection->boxes[ibox];
   
    if(mode==1)
    {
        for(int i=0;i<3;i++)
            for(int j=0;j<3;j++)
                ds[i]+=box->B[j][i]*dx[j];
    }
    else
        for(int i=0;i<3;i++)
            ds[i]=dx[i];
    

    for(int i=0;i<3;i++)
    {
        while (ds[i]<0.0)
            ds[i]++;
        while (ds[i]>=1.0)
            ds[i]--;
    }
    
    if(region==NULL)
    {
        for(int i=0;i<box->natms;i++)
        {
            for(int j=0;j<3;j++)
            {
                box->s[3*i+j]+=ds[j];
                if( box->s[3*i+j]>=1.0)
                    box->s[3*i+j]--;
            }
        }
    }
    else
    {
        type0* s=box->s;
        for(int i=0;i<box->natms;i++)
        {
            if(region->belong(box->H,&s[3*i]))
            {
                for(int j=0;j<3;j++)
                {
                    box->s[3*i+j]+=ds[j];
                    if( box->s[3*i+j]>=1.0)
                        box->s[3*i+j]--;
                }
            }
        }
        

    }
    
        

    delete [] dx;
    delete [] ds;
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_move::~Command_move()
{
}

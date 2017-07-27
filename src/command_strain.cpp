#include "command_strain.h"
#include "memory.h"
#include "error.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_strain::Command_strain(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    int ibox;
    type0 det;
    if(narg!=8)
    {
        error->warning("rotate command needs at least 7 arguments\n"
            "SYNTAX: strain box e[0][0] e[1][1] e[2][2] e[2][1] e[2][0] e[1][0]\n");
        return;
    }
    
    
    
    ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
    
    
    type0** F;
    CREATE2D(F,3,3);
    type0** G;
    CREATE2D(G,3,3);
    
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
        {
            G[i][j]=0.0;
            
            if(i==j)
                F[i][j]=1.0;
            else
                F[i][j]=0.0;
        }

    F[0][0]+=atof(arg[2]);
    F[1][1]+=atof(arg[3]);
    F[2][2]+=atof(arg[4]);
    F[2][1]+=atof(arg[5]);
    F[2][0]+=atof(arg[6]);
    F[1][0]+=atof(arg[7]);

    
    
    
    
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            for(int k=0;k<3;k++)
                G[i][j]+=F[i][k]*box_collection->boxes[ibox]->H[j][k];
    
    
    
    det=M3DET(G);
    
    if(det<=0.0)
    {
        error->warning("determinant after transformation should be greater than zero");
        for(int i=0;i<3;i++)
        {
            delete [] F[i];
            delete [] G[i];
        }
        delete [] F;
        delete [] G;
        return;
    }

    
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            box_collection->boxes[ibox]->H[i][j]=G[i][j];
    
    for(int i=0;i<3;i++)
    {
        delete [] F[i];
        delete [] G[i];
    }
    delete [] F;
    delete [] G;
    
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_strain::~Command_strain()
{
}
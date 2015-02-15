#include <math.h>
#include "command_mismatch.h"
#include "error.h"
#include "memory.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_mismatch::Command_mismatch(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    if(narg!=6)
    {
        error->warning("mismatch command needs 5 arguments\n"
        "SYNTAX: mismatch box_0 box_1 N_x N_y N_z");
        return;
    }
    
    int ibox0=box_collection->find(arg[1]);
    if(ibox0<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
    int ibox1=box_collection->find(arg[2]);
    if(ibox1<0)
    {
        error->warning("box %s not found",arg[2]);
        return;
    }
    
    int* n0;
    
    CREATE1D(n0,3);
    for(int i=0;i<3;i++)
        n0[i]=atoi(arg[3+i]);
    
    for(int i=0;i<3;i++)
    {
        if(n0[i]<=0)
        {
            error->warning("for mismatch n[%d] should be greater than 0",i);
            delete [] n0;
            return;
        }
            
    }

    int* n1;
    CREATE1D(n1,3);

    type0* eps_0;
    type0* eps_1;
    CREATE1D(eps_0,3);
    CREATE1D(eps_1,3);
    
    
    type0** H0=box_collection->ret_H(arg[1]);
    type0** H1=box_collection->ret_H(arg[2]);
    type0 tmp,tmp0,tmp1;
    for(int i=0;i<3;i++)
    {
        tmp0=tmp1=0.0;
        for(int j=0;j<3;j++)
        {
            tmp0+=H0[i][j]*H0[i][j];
            tmp1+=H1[i][j]*H1[i][j];
        }
        n1[i]=static_cast<int>(sqrt(tmp0/tmp1)*static_cast<type0>(n0[i])+0.5);
        tmp=0.0;
        for(int j=0;j<3;j++)
        {
            tmp+=(static_cast<type0>(n0[i])*H0[i][j]+static_cast<type0>(n1[i])*H1[i][j])
            *(static_cast<type0>(n0[i])*H0[i][j]+static_cast<type0>(n1[i])*H1[i][j])*0.25;
            
        }
    
        tmp=sqrt(tmp);
        eps_0[i]=tmp/(sqrt(tmp0)*static_cast<type0>(n0[i]))-1.0;
        eps_1[i]=tmp/(sqrt(tmp1)*static_cast<type0>(n1[i]))-1.0;
        
    }
    
    int natms0=box_collection->ret_natms(arg[1])*n0[0]*n0[1]*n0[2];
    int natms1=box_collection->ret_natms(arg[2])*n1[0]*n1[1]*n1[2];
    
    printf("\n");
    printf("%s: %d atoms\n",arg[1],natms0);
    for(int i=0;i<2;i++)
        printf("%d\u00D7",n0[i]);
    printf("%d\n",n0[2]);
    printf("epsilon: ");
    for(int i=0;i<2;i++)
        printf("%lf ",eps_0[i]);
    printf("%lf\n",eps_0[2]);
    
    printf("\n");
    
    printf("%s: %d atoms\n",arg[2],natms1);
    for(int i=0;i<2;i++)
        printf("%d\u00D7",n1[i]);
    printf("%d\n",n1[2]);
    printf("epsilon: ");
    for(int i=0;i<2;i++)
        printf("%lf ",eps_1[i]);
    printf("%lf\n",eps_1[2]);
    
    printf("\n");
    
    delete [] eps_0;
    delete [] eps_1;
    delete [] n1;
    delete [] n0;
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_mismatch::~Command_mismatch()
{
}

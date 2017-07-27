#include "command_random_gas.h"
#include "error.h"
#include "memory.h"
#include "box_collection.h"
#include "gr.h"
#include <math.h>
#define RNG_M 2147483647
#define RNG_MI 1.0/2147483647
#define RNG_A 16807
#define RNG_Q 127773
#define RNG_R 2836
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_random_gas::Command_random_gas(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    Box* box=box_collection->boxes[box_collection->find(arg[1])];
    int itype=box->add_type(atof(arg[3]),arg[2]);
    bond=atof(arg[4]);
    int N=atoi(arg[5]);
    seed=atoi(arg[6]);
    
    int natms=box->natms;
    type0* s;//=box->s;
    int* type;
    //GROW(s,3*natms,3*(2*N+natms));
    //GROW(type,natms,2*N+natms);
    
    int new_natms=natms+2*N;
    type=new int[natms+2*N];
    s=new type0[(natms+2*N)*3];
    memcpy(type,box->type,natms*sizeof(int));
    memcpy(s,box->s,3*natms*sizeof(type0));
    delete [] box->s;
    delete [] box->type;
    box->s=s;
    box->type=type;
    box->natms=new_natms;
    
    int j=natms;
    for(int i=0;i<N;i++)
    {
        rand_gen(box->B,s+3*j,s+3*(j+1));
        type[j]=itype;
        type[j+1]=itype;
        j+=2;
    }
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_random_gas::~Command_random_gas()
{
}
/*--------------------------------------------
 uniform number generator between 0.0 and 1.0
 --------------------------------------------*/
type0 Command_random_gas::rand_gen()
{
    int first,second,rnd;

    first = seed/RNG_Q;
    second = seed%RNG_Q;
    rnd = RNG_A*second-RNG_R*first;
    if (rnd > 0)
        seed=rnd;
    else
        seed=rnd+RNG_M;
    return seed*RNG_MI;
}
/*--------------------------------------------
 uniform number generator between 0.0 and 1.0
 --------------------------------------------*/
void Command_random_gas::rand_gen(type0**& B,type0* s0,type0* s1)
{
    
    for(int i=0;i<3;i++)
        s0[i]=rand_gen();
    
    type0 phi=M_PI*rand_gen();
    type0 theta=2.0*M_PI*rand_gen();
    
    s1[0]=cos(theta)*sin(phi)*bond;
    s1[1]=sin(theta)*sin(phi)*bond;
    s1[2]=cos(phi)*bond;

    
    s1[0]=B[0][0]*s1[0]+B[1][0]*s1[1]+B[2][0]*s1[2];
    s1[1]=B[1][1]*s1[1]+B[2][1]*s1[2];
    s1[2]*=B[2][2];
    
    
    for(int i=0;i<3;i++)
    {
        s1[i]=s0[i]+s1[i];
        
        while(s1[i]<0.0)
            s1[i]++;
        while(s1[i]>=1.0)
            s1[i]--;
    }
    
    
}


#include "xmath.h"
#include "memory.h"
#include <cmath>
#define SOLVE_TOL 1.0e-10
/*--------------------------------------------
 constructor
 --------------------------------------------*/
XMath::XMath(Xtal* xtal): InitPtrs(xtal)
{
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
XMath::~XMath()
{
}
/*--------------------------------------------
 autogrid the domain
 --------------------------------------------*/
void XMath::square2lo_tri(type0** H_old
,type0** H_new)
{
    type0** Q;
    CREATE2D(Q,3,3);
    type0 H0H0;
    type0 H0H1;
    H0H0=H0H1=0.0;
    for(int i=0;i<3;i++)
    {
        H0H0+=H_old[0][i]*H_old[0][i];
        H0H1+=H_old[0][i]*H_old[1][i];
    }
    Q[0][0]=H_old[0][0];
    Q[0][1]=H_old[0][1];
    Q[0][2]=H_old[0][2];
    Q[1][0]=H0H0*H_old[1][0]-H0H1*H_old[0][0];
    Q[1][1]=H0H0*H_old[1][1]-H0H1*H_old[0][1];
    Q[1][2]=H0H0*H_old[1][2]-H0H1*H_old[0][2];
    Q[2][0]=H_old[0][1]*H_old[1][2]-H_old[0][2]*H_old[1][1];
    Q[2][1]=H_old[0][2]*H_old[1][0]-H_old[0][0]*H_old[1][2];
    Q[2][2]=H_old[0][0]*H_old[1][1]-H_old[0][1]*H_old[1][0];
    for(int i=0;i<3;i++)
    {
        H0H0=0.0;
        for(int j=0;j<3;j++)
            H0H0+=Q[i][j]*Q[i][j];
        
        H0H1=sqrt(H0H0);
        for(int j=0;j<3;j++)
            Q[i][j]/=H0H1;
    }
    
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            H_new[i][j]=0.0;
    
    for(int i=0;i<3;i++)
        for(int j=0;j<i+1;j++)
        {
            for(int k=0;k<3;k++)
                H_new[i][j]+=H_old[i][k]*Q[j][k];
        }
    
    for(int i=0;i<3;i++)
        delete [] Q[i];
    delete [] Q;
}
/*--------------------------------------------
 solve sin(x)/x=y
 make sure 0<y<1
 --------------------------------------------*/
type0 XMath::solve_sinx_ov_x(type0 y)
{
    type0 err,a,b,c;
    
    b=M_PI;
    a=0.0;
    c=0.5*(a+b);
    
    
    err=sin(c)/c-y;
    while (fabs(err)>SOLVE_TOL)
    {
        if(err>0.0)
            a=c;
        else
            b=c;
        c=0.5*(a+b);
        
        err=sin(c)/c-y;
        if(isnan(err))
            err=-y;
        
    }
       
    return c;
}
/*--------------------------------------------
 use bracket to solve 
 f(a) should be greater than 0.0
 f(b) should be less than 0.0
 --------------------------------------------*/
type0 XMath::bracket(type0(f(type0)),type0 a,type0 b)
{
    type0 c,fc;
    
    c=0.5*(a+b);
    fc=(*f)(c);
    
    while (fabs(a-b)<SOLVE_TOL)
    {
        if(fc>0.0)
            a=c;
        if(fc<0.0)
            b=c;
        c=0.5*(a+b);
        fc=(*f)(c);
    }
    
    return c;
}





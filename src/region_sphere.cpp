#include "region_sphere.h"
#include "memory.h"
#include "error.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Region_sphere::Region_sphere(Xtal* xtal
,int narg,char** arg):Region(xtal)
{
    type0 tmp0,tmp1,tmp2;
    alloc=0;
    if(narg!=4)
    {
        error->warning("region sphere needs 3 arguments\n"
        "SYNTAX: sphere name (c0,c1,c2) radius");
        xtal->error_flag=-1;
        return;
        
    }
    if(atof(arg[3])<0.0)
    {
        error->warning("redius of sphere cannot "
        "be less than 0.0");
        xtal->error_flag=-1;
        return;
    }
    
    
    int lngth=static_cast<int>(strlen(arg[1]))+1;
    CREATE1D(region_name,lngth);
    CREATE1D(x,3);
    alloc=1;
    memcpy(region_name,arg[1],lngth);

    if(sscanf(arg[2],"(%lf,%lf,%lf)",&tmp0,&tmp1,&tmp2)==3)
    {
        x[0]=tmp0;
        x[1]=tmp1;
        x[2]=tmp2;
    }
    else
    {
        error->warning("c should be of the format: (c0,c1,c2)");
        xtal->error_flag=-1;
        return;
    }
    
    r=atof(arg[3]);
    
    rsq=r*r;
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Region_sphere::~Region_sphere()
{
    if(alloc)
    {
        delete [] region_name;
        delete [] x;
    }
}
/*--------------------------------------------
 belong
 --------------------------------------------*/
inline int Region_sphere::belong(type0** H,type0* s)
{
    
    x0=H[0][0]*s[0]+H[1][0]*s[1]+H[2][0]*s[2]-x[0];
    x1=H[1][1]*s[1]+H[2][1]*s[2]-x[1];
    x2=H[2][2]*s[2]-x[2];
    
    if(x0*x0+x1*x1+x2*x2<rsq)
        return 1;
    else
        return 0;
    
}


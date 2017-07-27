
#include "region_ellipsoid.h"
#include "memory.h"
#include "error.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Region_ellipsoid::Region_ellipsoid(Xtal* xtal
,int narg,char** arg):Region(xtal)
{
    type0 tmp0,tmp1,tmp2;
    alloc=0;
    if(narg!=4)
    {
        error->warning("region ellipsoid needs 3 arguments\n"
        "SYNTAX: ellipsoid name (c0,c1,c2) (a0,a1,a2)");
        xtal->error_flag=-1;
        return;
        
    }

    
    
    int lngth=static_cast<int>(strlen(arg[1]))+1;
    CREATE1D(region_name,lngth);
    CREATE1D(x,3);
    CREATE1D(asq_inv,3);
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
    
    
    tmp0=tmp1=tmp2=1.0;
    if(sscanf(arg[3],"(%lf,%lf,%lf)",&tmp0,&tmp1,&tmp2)==3)
    {
        
        asq_inv[0]=tmp0;
        asq_inv[1]=tmp1;
        asq_inv[2]=tmp2;
        
        for(int i=0;i<3;i++)
            if(asq_inv[i]<=0.0)
            {
                error->warning("a[%d] should be greater than 0.0",i);
                xtal->error_flag=-1;
                return;
            }
        
        asq_inv[0]=1.0/(asq_inv[0]*asq_inv[0]);
        asq_inv[1]=1.0/(asq_inv[1]*asq_inv[1]);
        asq_inv[2]=1.0/(asq_inv[2]*asq_inv[2]);
        
    }
    else if(sscanf(arg[3],"(inf,%lf,%lf)",&tmp1,&tmp2)==2)
    {
        asq_inv[0]=tmp0;
        asq_inv[1]=tmp1;
        asq_inv[2]=tmp2;
        
        for(int i=0;i<3;i++)
            if(asq_inv[i]<=0.0)
            {
                error->warning("a[%d] should be greater than 0.0",i);
                xtal->error_flag=-1;
                return;
            }
        
        asq_inv[0]=0.0;
        asq_inv[1]=1.0/(asq_inv[1]*asq_inv[1]);
        asq_inv[2]=1.0/(asq_inv[2]*asq_inv[2]);
    }
    else if(sscanf(arg[3],"(%lf,inf,%lf)",&tmp0,&tmp2)==2)
    {
        asq_inv[0]=tmp0;
        asq_inv[1]=tmp1;
        asq_inv[2]=tmp2;
        
        for(int i=0;i<3;i++)
            if(asq_inv[i]<=0.0)
            {
                error->warning("a[%d] should be greater than 0.0",i);
                xtal->error_flag=-1;
                return;
            }
        
        asq_inv[0]=1.0/(asq_inv[0]*asq_inv[0]);
        asq_inv[1]=0.0;
        asq_inv[2]=1.0/(asq_inv[2]*asq_inv[2]);
    }
    else if(sscanf(arg[3],"(%lf,%lf,inf)",&tmp0,&tmp1)==2)
    {
        asq_inv[0]=tmp0;
        asq_inv[1]=tmp1;
        asq_inv[2]=tmp2;
        
        for(int i=0;i<3;i++)
            if(asq_inv[i]<=0.0)
            {
                error->warning("a[%d] should be greater than 0.0",i);
                xtal->error_flag=-1;
                return;
            }
        
        asq_inv[0]=1.0/(asq_inv[0]*asq_inv[0]);
        asq_inv[1]=1.0/(asq_inv[1]*asq_inv[1]);
        asq_inv[2]=0.0;
    }
    else if(sscanf(arg[3],"(%lf,inf,inf)",&tmp0)==1)
    {
        asq_inv[0]=tmp0;
        asq_inv[1]=tmp1;
        asq_inv[2]=tmp2;
        
        for(int i=0;i<3;i++)
            if(asq_inv[i]<=0.0)
            {
                error->warning("a[%d] should be greater than 0.0",i);
                xtal->error_flag=-1;
                return;
            }
        
        asq_inv[0]=1.0/(asq_inv[0]*asq_inv[0]);
        asq_inv[1]=0.0;
        asq_inv[2]=0.0;
    }
    else if(sscanf(arg[3],"(inf,%lf,inf)",&tmp1)==1)
    {
        asq_inv[0]=tmp0;
        asq_inv[1]=tmp1;
        asq_inv[2]=tmp2;
        
        for(int i=0;i<3;i++)
            if(asq_inv[i]<=0.0)
            {
                error->warning("a[%d] should be greater than 0.0",i);
                xtal->error_flag=-1;
                return;
            }
        
        asq_inv[0]=0.0;
        asq_inv[1]=1.0/(asq_inv[1]*asq_inv[1]);
        asq_inv[2]=0.0;
    }
    else if(sscanf(arg[3],"(inf,inf,%lf)",&tmp2)==1)
    {
        asq_inv[0]=tmp0;
        asq_inv[1]=tmp1;
        asq_inv[2]=tmp2;
        
        for(int i=0;i<3;i++)
            if(asq_inv[i]<=0.0)
            {
                error->warning("a[%d] should be greater than 0.0",i);
                xtal->error_flag=-1;
                return;
            }
        
        asq_inv[0]=0.0;
        asq_inv[1]=0.0;
        asq_inv[2]=1.0/(asq_inv[2]*asq_inv[2]);
    }
    else
    {
        error->warning("a should be of the format: (a0,a1,a2)");
        xtal->error_flag=-1;
        return;
    }
    

}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Region_ellipsoid::~Region_ellipsoid()
{
    if(alloc)
    {
        delete [] region_name;
        delete [] x;
        delete [] asq_inv;
    }
}
/*--------------------------------------------
 belong
 --------------------------------------------*/
inline int Region_ellipsoid::belong(type0** H,type0* s)
{
    
    x0=H[0][0]*s[0]+H[1][0]*s[1]+H[2][0]*s[2]-x[0];
    x1=H[1][1]*s[1]+H[2][1]*s[2]-x[1];
    x2=H[2][2]*s[2]-x[2];
    
    if(x0*x0*asq_inv[0]+x1*x1*asq_inv[1]+x2*x2*asq_inv[2]<1.0)
        return 1;
    else
        return 0;
    
}


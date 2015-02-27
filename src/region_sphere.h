#ifdef Region_Style
    RegionStyle(Region_sphere,sphere)
#else
#ifndef __xtal__region_sphere__
#define __xtal__region_sphere__

#include <stdio.h>
#include "region.h"
class Region_sphere :public Region
{
private:
    type0* x;
    type0 r;
    type0 rsq;
    type0 x0,x1,x2;
    int alloc;
protected:
public:
    
    Region_sphere(Xtal*,int,char**);
    ~Region_sphere();
    int belong(type0**,type0*);
    
    
};
#endif
#endif

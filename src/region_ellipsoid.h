#ifdef Region_Style
    RegionStyle(Region_ellipsoid,ellipsoid)
#else
#ifndef __xtal__region_ellipsoid__
#define __xtal__region_ellipsoid__

#include <stdio.h>
#include "region.h"
class Region_ellipsoid :public Region
{
private:
    type0* x;
    type0* asq_inv;
    type0 x0,x1,x2;
    int alloc;
protected:
public:
    
    Region_ellipsoid(Xtal*,int,char**);
    ~Region_ellipsoid();
    int belong(type0**,type0*);
    
    
};
#endif
#endif

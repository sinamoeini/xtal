#ifdef Region_Style
    RegionStyle(Region_block,block)
#else
#ifndef __xtal__region_block__
#define __xtal__region_block__
#include <stdio.h>
#include "region.h"
class Region_block :public Region
{
private:
    type0* bond;
    int* bond_st;
    type0 x,y,z;
    int alloc;
protected:
public:
    
    Region_block(Xtal*,int,char**);
    ~Region_block();
    int belong(type0**,type0*);
    
    
};
#endif 
#endif

#ifndef __xtal__region__
#define __xtal__region__
#include <stdio.h>
#include "init.h"
class Region :protected InitPtrs
{
private:
protected:
public:
    
    Region(Xtal*);
    virtual ~Region();
    virtual inline int belong(type0**,type0*)=0;
    char* region_name;
    
};

#endif /* defined(__xtal__region__) */

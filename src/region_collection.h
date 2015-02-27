#ifndef __xtal__region_collection__
#define __xtal__region_collection__
#include "init.h"
#include "region.h"
#include <stdio.h>
class RegionCollection:protected InitPtrs
{
private:
    void del(int);
    
    
protected:
public:
    RegionCollection(Xtal*);
    ~RegionCollection();
    
    
    //symbol based functions
    int find(char*);
    void del(char*);
    int add_cmd(int,char**);

    
    Region** regions;
    int no_regions;
};
#endif /* defined(__xtal__region_collection__) */

#ifndef __xtal__box2cfg__
#define __xtal__box2cfg__

#include <stdio.h>
#include "init.h"
class Box2CFG :protected InitPtrs
{
private:
    FILE* cfgfile;

    
    
protected:
public:
    
    Box2CFG(Xtal*,char*,char*);
    ~Box2CFG();
    
    
    
};

#endif /* defined(__xtal__box2cfg__) */

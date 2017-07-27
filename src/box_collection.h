#ifndef __xtal__box_collection__
#define __xtal__box_collection__
#include <stdio.h>
#include "init.h"
#include "box.h"
class BoxCollection:protected InitPtrs
{
private:
protected:
public:
    BoxCollection(Xtal*);
    ~BoxCollection();
    
    
    //symbol based functions
    int find(char*);
    int add(char*);
    int cp(char*,char*);
    void del(char*);
    int add_unsafe();
    void del(int);

    Box** boxes;
    int no_boxes;
};
#endif

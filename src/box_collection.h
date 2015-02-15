#ifndef __xtal__box_collection__
#define __xtal__box_collection__
#include <stdio.h>
#include "init.h"
#include "box.h"
class BoxCollection:protected InitPtrs
{
private:

    int add_unsafe();
    void del(int);

protected:
public:
    BoxCollection(Xtal*);
    ~BoxCollection();
    
    
    //symbol based functions
    int find(char*);
    int add(char*);
    void del(char*);
    
    void join(char*,char*,char*,int);
    void add_vac(char*,int,type0);
    void mul(char*,int*);
    int add_type(char*,type0,char*);
    void add_atoms(char*,int,int*,type0*);
    
    
    int ret_no_types(char*);
    char** ret_atom_names(char*);
    type0* ret_mass(char*);
    
    type0** ret_H(char*);
    type0** ret_B(char*);
    
    int ret_natms(char*);
    type0* ret_s(char*);
    int* ret_type(char*);
    
    Box** boxes;
    int no_boxes;
};
#endif

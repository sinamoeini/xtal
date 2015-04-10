#ifndef __xtal__box__
#define __xtal__box__

#include <stdio.h>
#include "init.h"
class Box:protected InitPtrs
{
private:
protected:
public:
    int dof_xst;
    char* box_name;
    
    int no_types;
    char** atom_names;
    type0* mass;
    int* type_count;
    
    type0** H;
    type0** B;
    
    int natms;
    type0* s;
    int* type;
    char* dof;
    
    
    Box(Xtal*);
    Box(Xtal*,char*);
    void join(Box*,Box*,int);
    ~Box();
    int add_type(type0,char*);
    int find_type(char*);
    void add_atoms(int,int*,type0*);
    void add_atoms(int,int*,type0*,char*);
    void del_atoms(int,int*);
    void change_name(char*);
    void add_name(char*);
    void mul(int*);
    void add_vac(int,type0);
    void ucell_chang(type0**);
    void id_correction();
    void del_type(int);
};

#endif

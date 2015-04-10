#include <math.h>
#include "box_collection.h"
#include "memory.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
BoxCollection::BoxCollection(Xtal* xtal):InitPtrs(xtal)
{
    no_boxes=0;
}
/*--------------------------------------------
 desstructor
 --------------------------------------------*/
BoxCollection::~BoxCollection()
{
    
    for(int ibox=0;ibox<no_boxes;ibox++)
    {
        
        delete boxes[ibox];
    }

    if(no_boxes)
        delete [] boxes;
    
    no_boxes=0;
    
}
/*--------------------------------------------
 add a box
 --------------------------------------------*/
int BoxCollection::add(char* name)
{
    Box** new_boxs;
    CREATE1D(new_boxs,no_boxes+1);
    for(int i=0;i<no_boxes;i++)
        new_boxs[i]=boxes[i];

    new_boxs[no_boxes]=new Box(xtal,name);
    
    if(no_boxes)
        delete [] boxes;
    
    boxes=new_boxs;
    no_boxes++;
    return no_boxes-1;
}
/*--------------------------------------------
 add a box without a name 
 name should be added later!!!!!
 --------------------------------------------*/
int BoxCollection::add_unsafe()
{
    Box** new_boxs;
    CREATE1D(new_boxs,no_boxes+1);
    for(int i=0;i<no_boxes;i++)
        new_boxs[i]=boxes[i];
    
    new_boxs[no_boxes]=new Box(xtal);

    if(no_boxes)
        delete [] boxes;
    boxes=new_boxs;
    no_boxes++;
    return no_boxes-1;
}
/*--------------------------------------------
 add a box
 --------------------------------------------*/
int BoxCollection::find(char* name)
{
    for(int i=0;i<no_boxes;i++)
    {
        if(strcmp(name,boxes[i]->box_name)==0)
            return i;
    }
    return -1;
}
/*--------------------------------------------
 add a box
 --------------------------------------------*/
void BoxCollection::del(int ibox)
{
    
    delete boxes[ibox];
    
    Box** new_boxs;
    CREATE1D(new_boxs,no_boxes-1);
    for(int i=0;i<ibox;i++)
        new_boxs[i]=boxes[i];
    for(int i=ibox+1;i<no_boxes;i++)
        new_boxs[i-1]=boxes[i];
    
    delete [] boxes;
    
    boxes=new_boxs;
    no_boxes--;
}
/*--------------------------------------------
 add a box
 --------------------------------------------*/
void BoxCollection::del(char* name)
{
    int ibox=find(name);
    del(ibox);
}
/*--------------------------------------------
 multiply a box
 --------------------------------------------*/
/*
void BoxCollection::mul(char* box,int* n)
{
    boxes[find(box)]->mul(n);
}
void BoxCollection::add_vac(char* box,int dim
,type0 thickness)
{
    boxes[find(box)]->add_vac(dim,thickness);
}
int BoxCollection::add_type(char* box,type0 m
,char* name)
{
    return boxes[find(box)]->add_type(m,name);
}
void BoxCollection::add_atoms(char* box,int no
,int* type_buff,type0* s_buff)
{
    boxes[find(box)]->add_atoms(no,type_buff,s_buff);
}
int BoxCollection::ret_no_types(char* box)
{
    return boxes[find(box)]->no_types;
}
char** BoxCollection::ret_atom_names(char* box)
{
    return boxes[find(box)]->atom_names;
}
type0* BoxCollection::ret_mass(char* box)
{
    return boxes[find(box)]->mass;
}
type0** BoxCollection::ret_H(char* box)
{
    return boxes[find(box)]->H;
}
type0** BoxCollection::ret_B(char* box)
{
    return boxes[find(box)]->B;
}
int BoxCollection::ret_natms(char* box)
{
    return boxes[find(box)]->natms;
}
type0* BoxCollection::ret_s(char* box)
{
    return boxes[find(box)]->s;
}
int* BoxCollection::ret_type(char* box)
{
    return boxes[find(box)]->type;
}

*/
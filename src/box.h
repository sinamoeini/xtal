#ifndef __xtal__box__
#define __xtal__box__

#include <stdio.h>
#include "init.h"

class Box:protected InitPtrs
{
private:
    void add_image(int,type0);
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
    int nghosts;
    type0* s;
    int* type;
    char* dof;
    
    
    Box(Xtal*);
    Box(Xtal*,char*);
    Box(Box&);
    
    void chng_name(char*);
    void join(Box*,Box*,int);
    void join(Box*,Box*);
    ~Box();
    int add_type(type0,char*);
    int find_type(char*);
    void add_atoms(int,int*,type0*);
    void add_atoms(int,int*,type0*,char*);
    void del_atoms(int,int*);
    void del_atoms();
    void change_name(char*);
    void add_name(char*);
    void mul(int*);
    void add_vac(int,int,type0);
    void rm_frac(int,type0,type0);
    void ucell_chang(type0**);
    void id_correction();
    void del_type(int);
    
    void add_ghost(bool(&)[3],type0(&)[3]);

    void del_ghost();
};






template<typename T>
class Arr
{
private:
    T*& arr;
    int& sz;
    int length;
    int grth;
    void sz_chk(int& n)
    {
        if(sz+n<length)
            return;
        
        int length_=sz+n+grth;
        T* arr_=new T[length_];
        memcpy(arr_,arr,sz*sizeof(T));
        delete [] arr;
        arr=arr_;
        length=length_;
    }
protected:
public:
    Arr(T*& arr_,int& sz_,int grow):
    arr(arr_),
    sz(sz_)
    {
        length=sz;
        grth=grow;
        if(sz==0) arr=NULL;
    }
    
    
    ~Arr()
    {
        if(sz==length)
            return;
        
        T* arr_=new T[sz];
        memcpy(arr_,arr,sz*sizeof(T));
        delete [] arr;
        arr=arr_;
    }
    
    void operator()(T*& _arr ,int _arr_sz)
    {
        sz_chk(_arr_sz);
        memcpy(arr+sz,_arr,_arr_sz*sizeof(T));
        sz+=_arr_sz;
    }
    
    void operator()(T& elem)
    {
        int i=1;
        sz_chk(i);
        arr[sz]=elem;
        sz++;
    }
    
    template<int N>
    void operator()(T (&v) [N])
    {
        T* v_=v;
        operator()(v_,N);
    }
    
};








#endif

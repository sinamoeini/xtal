#ifndef __xtal__cfg2box__
#define __xtal__cfg2box__
#include <stdio.h>
#include "init.h"
class CFG2Box :protected InitPtrs
{
private:
    FILE* cfgfile;
    char* file_name;
    
    
    type0** H0;
    type0** eta;
    type0** eta_sq;
    type0** trns;
    type0** H_x;
    type0** Ht;
    type0* sq;
    type0* b;
    
    type0* s_buff;
    int* type_buff;
    
    char* line;
    
    type0 basic_length;
    
    int entry_count;
    int ext_cfg;
    int header_cmplt;
    int vel_chk;
    int atom_cmplt;
    int last_type;
    int curr_id;
    
    void read_header();
    void set_box();
    void M3sqroot(type0**,type0**);
    void read_atom();
    void alloc();
    void dealloc();
    
    int tmp_natms;
    int ibox;
    
    
protected:
public:
    
    CFG2Box(Xtal*,char*,char*);
    ~CFG2Box();
    //char* box;
    
};



#endif 

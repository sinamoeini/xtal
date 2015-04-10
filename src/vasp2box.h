
#ifndef __xtal__vasp2box__
#define __xtal__vasp2box__
#include <stdio.h>
#include "init.h"
class VASP2Box :protected InitPtrs
{
private:
    type0** H0;
    type0** B0;
    char* line;
    char* file_name;
    type0 basic_length;
    int read_line(FILE*);
    void alloc();
    void dealloc();
    type0 find_mass(char*);
    int natms_tmp;
    type0* s_tmp;
    int* type_tmp;
    char* dof_tmp;
    int dof_exist;
protected:
public:
    
    VASP2Box(Xtal*,char*,char*);
    ~VASP2Box();
    
};

#endif

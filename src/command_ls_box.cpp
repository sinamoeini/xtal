#include "command_ls_box.h"
#include "error.h"
#include "xtal.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_ls_box::Command_ls_box(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    char** cmds=xtal->cmds_lst;
    int no_cmds=xtal->no_cmds;
    int no_vals=xtal->no_vals;
    
    for(int i=no_cmds;i<no_cmds+no_vals;i++)
        printf("%s\n",cmds[i]);
    

}
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_ls_box::~Command_ls_box()
{
}

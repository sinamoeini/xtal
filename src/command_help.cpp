#include "command_help.h"
#include "error.h"
#include "xtal.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_help::Command_help(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    char** cmds=xtal->cmds_lst;
    int no_cmds=xtal->no_cmds;
    
    printf("list of commands:\n");
    for(int i=0;i<no_cmds;i++)
        printf("  -%s\n",cmds[i]);

}
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_help::~Command_help()
{
    
}
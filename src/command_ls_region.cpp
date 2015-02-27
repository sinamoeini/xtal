#include "command_ls_region.h"
#include "error.h"
#include "region_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_ls_region::Command_ls_region(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    printf("\n");
    for(int i=0;i<region_collection->no_regions;i++)
        printf("%s\n",region_collection->regions[i]->region_name);
    printf("\n");    
    
}
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_ls_region::~Command_ls_region()
{
}

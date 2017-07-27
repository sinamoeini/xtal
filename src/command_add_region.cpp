#include "command_add_region.h"
#include "region_collection.h"
#include "error.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_add_region::Command_add_region(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    if(narg<3)
    {
        error->warning("add_region needs at least 3 arguments\n"
        "SYNTAX: it varies dependent on the style of region");
        return;
    }
    int iregion=region_collection->find(arg[2]);
    
    if(iregion!=-1)
        region_collection->del(arg[2]);
    
    region_collection->add_cmd(narg-1,&arg[1]);
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_add_region::~Command_add_region()
{
}

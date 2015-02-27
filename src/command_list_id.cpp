#include <string>
#include <stdlib.h>
#include <math.h>
#include "command_list_id.h"
#include "box_collection.h"
#include "region_collection.h"
#include "error.h"
#include "memory.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_list_id::Command_list_id(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    if(narg!=4)
    {
        error->warning("list_id command needs 3 arguments\n"
        "SYNTAX: list_id box region id_file");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }

    int iregion=region_collection->find(arg[2]);
    if(iregion<0)
    {
        error->warning("region %s not found",arg[2]);
        return;
    }
    

    box_collection->boxes[ibox]->id_correction();
    int natms=box_collection->boxes[ibox]->natms;
    type0** H=box_collection->boxes[ibox]->H;
    type0* s=box_collection->boxes[ibox]->s;
    
    int* id_lst;
    CREATE1D(id_lst,natms);
    
    


    int icomp=0;
    int no=0;
    for(int i=0;i<natms;i++)
    {
        if(region_collection->regions[iregion]->belong(H,&s[icomp]))
            id_lst[no++]=i;
        icomp+=3;
    }

    FILE* idfile;
    idfile=fopen(arg[3],"w");
    
    fprintf(idfile,"%d\n",no);
    for(int i=0;i<no;i++)
        fprintf(idfile,"%d\n",id_lst[i]);
    fclose(idfile);
    
    if(natms)
        delete [] id_lst;
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_list_id::~Command_list_id()
{
}



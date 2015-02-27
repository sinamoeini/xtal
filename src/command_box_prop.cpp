#include "command_box_prop.h"
#include "box_collection.h"
#include "error.h"
#include "memory.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_box_prop::Command_box_prop(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    if(narg==1)
    {
        error->warning("box_prop command needs atleast 1 argument\n"
        "SYNTAX: box_prop box0 box1 ...");
        return;
    }
    int no_boxes=narg-1;
    int* box_list;
    CREATE1D(box_list,no_boxes);
    
    for(int i=0;i<no_boxes;i++)
    {
        box_list[i]=box_collection->find(arg[i+1]);
        if(box_list[i]<0)
        {
            if(no_boxes)
                delete [] box_list;
            error->warning("box %s not found",arg[i+1]);
            return;
        }
    }
    int no_types;
    int ibox;
    for(int i=0;i<no_boxes;i++)
    {
        ibox=box_list[i];
        printf("%s:\n",box_collection->boxes[ibox]->box_name);
        printf("%d atoms:",box_collection->boxes[ibox]->natms);
        no_types=box_collection->boxes[ibox]->no_types;
        for(int j=0;j<no_types-1;j++)
            printf(" %d %s,",box_collection->boxes[ibox]->type_count[j]
            ,box_collection->boxes[ibox]->atom_names[j]);
        
        printf(" %d %s\n",box_collection->boxes[ibox]->type_count[no_types-1]
               ,box_collection->boxes[ibox]->atom_names[no_types-1]);
        
        
        printf("  |%lf %lf %lf|\n"
        ,box_collection->boxes[ibox]->H[0][0]
        ,box_collection->boxes[ibox]->H[0][1]
        ,box_collection->boxes[ibox]->H[0][2]);
        printf("H=|%lf %lf %lf|\n"
        ,box_collection->boxes[ibox]->H[1][0]
        ,box_collection->boxes[ibox]->H[1][1]
        ,box_collection->boxes[ibox]->H[1][2]);
        printf("  |%lf %lf %lf|\n"
        ,box_collection->boxes[ibox]->H[2][0]
        ,box_collection->boxes[ibox]->H[2][1]
        ,box_collection->boxes[ibox]->H[2][2]);
        printf("\n");
    }
    
    
    if(no_boxes)
        delete [] box_list;
    
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_box_prop::~Command_box_prop()
{
}
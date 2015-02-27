#include "command_ls_box.h"
#include "error.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_ls_box::Command_ls_box(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{

    printf("\n");
    for(int i=0;i<box_collection->no_boxes;i++)
        printf("%s\n",box_collection->boxes[i]->box_name);
    printf("\n");

}
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_ls_box::~Command_ls_box()
{
}

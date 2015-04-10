#include "box2vasp.h"
#include "memory.h"
#include "error.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Box2VASP::Box2VASP(Xtal* xtal,char* box,char* name):InitPtrs(xtal)
{
    
    FILE* poscar;
    poscar=fopen(name,"w");
    
    int ibox=box_collection->find(box);
    
    int dof_xst=box_collection->boxes[ibox]->dof_xst;
    int no_types=box_collection->boxes[ibox]->no_types;
    char** atom_names=box_collection->boxes[ibox]->atom_names;
    int* type_count=box_collection->boxes[ibox]->type_count;
    
    
    type0** H=box_collection->boxes[ibox]->H;
    
    int natms=box_collection->boxes[ibox]->natms;
    type0* s=box_collection->boxes[ibox]->s;
    int* type=box_collection->boxes[ibox]->type;
    char* dof=box_collection->boxes[ibox]->dof;
    
    
    
    fprintf(poscar,"%s\n",box);
    fprintf(poscar,"%23.16lf\n",1.0);
    
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
            fprintf(poscar,"%23.16lf",H[i][j]);
        fprintf(poscar,"\n");
    }
    
    
    for(int i=0;i<no_types;i++)
        fprintf(poscar,"%6s",atom_names[i]);
    fprintf(poscar,"\n");
    
    for(int i=0;i<no_types;i++)
        fprintf(poscar,"%6d",type_count[i]);
    fprintf(poscar,"\n");


    if(dof_xst)
        fprintf(poscar,"Selective dynamics\n");
    
    fprintf(poscar,"Direct\n");
    
    for(int itype=0;itype<no_types;itype++)
    {
        for(int iatm=0;iatm<natms;iatm++)
        {
            if(type[iatm]==itype)
            {
                for(int i=0;i<3;i++)
                    fprintf(poscar,"%20.16lf",s[iatm*3+i]);
                
                if(dof_xst)
                    for(int i=0;i<3;i++)
                    {
                        if(dof[3*iatm+i]=='0')
                            fprintf(poscar,"   T");
                        else
                            fprintf(poscar,"   F");
                    }
                
                fprintf(poscar,"\n");
            }
        }
    }
    
    
    fclose(poscar);
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Box2VASP::~Box2VASP()
{
}
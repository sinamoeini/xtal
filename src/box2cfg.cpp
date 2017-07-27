#include <math.h>
#include "box2cfg.h"
#include "memory.h"
#include "error.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Box2CFG::Box2CFG(Xtal* xtal,char* box,char* name):InitPtrs(xtal)
{
    cfgfile=fopen(name,"w");

    int ibox=box_collection->find(box);
    
    
    int no_types=box_collection->boxes[ibox]->no_types;
    char** atom_names=box_collection->boxes[ibox]->atom_names;
    type0* mass=box_collection->boxes[ibox]->mass;
    
    
    type0** H=box_collection->boxes[ibox]->H;
    
    int natms=box_collection->boxes[ibox]->natms;
    type0* s=box_collection->boxes[ibox]->s;
    int* type=box_collection->boxes[ibox]->type;
    
    
    
    
    fprintf(cfgfile,"Number of particles = %d\n",natms);
    fprintf(cfgfile,"A = 1 Angstrom (basic length-scale)\n");
    
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            fprintf(cfgfile,"H0(%d,%d) = %20.16lf A\n",i+1,j+1,H[i][j]);
    
    fprintf(cfgfile,".NO_VELOCITY.\n");
    fprintf(cfgfile,"entry_count = %d\n",3);
    
    
    

    int icomp;
    for(int itype=0;itype<no_types;itype++)
    {
        
        fprintf(cfgfile,"%lf \n%s \n",mass[itype],atom_names[itype]);
        icomp=0;
        for(int iatm=0;iatm<natms;iatm++)
        {
            if(type[iatm]==itype)
                fprintf(cfgfile,"%18.16lf %18.16lf %18.16lf\n",s[icomp],s[icomp+1],s[icomp+2]);
                
            icomp+=3;
        }
    }
    
    fclose(cfgfile);
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Box2CFG::~Box2CFG()
{
}


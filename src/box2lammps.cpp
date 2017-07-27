#include <math.h>
#include "box2lammps.h"
#include "memory.h"
#include "error.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Box2LAMMPS::Box2LAMMPS(Xtal* xtal,char* box,char* name):InitPtrs(xtal)
{
    lammpsfile=fopen(name,"w");

    int ibox=box_collection->find(box);
    
    
    int no_types=box_collection->boxes[ibox]->no_types;
    char** atom_names=box_collection->boxes[ibox]->atom_names;
    type0* mass=box_collection->boxes[ibox]->mass;
    
    
    type0** H=box_collection->boxes[ibox]->H;
    
    int natms=box_collection->boxes[ibox]->natms;
    type0* s=box_collection->boxes[ibox]->s;
    int* type=box_collection->boxes[ibox]->type;
    
    fprintf(lammpsfile,"#");
    for(int i=0;i<no_types;i++)
        fprintf(lammpsfile," %s(%lf)",atom_names[i],mass[i]);
    fprintf(lammpsfile,"\n");


    fprintf(lammpsfile,"\n");
    fprintf(lammpsfile,"%d atoms\n",natms);

    
    fprintf(lammpsfile,"\n");
    fprintf(lammpsfile,"%d atom types\n",no_types);
    
    bool triclinic=false;
    if(H[1][0]!=0.0)
        triclinic=true;
    if(H[2][0]!=0.0)
        triclinic=true;
    if(H[2][1]!=0.0)
        triclinic=true;
    
    fprintf(lammpsfile,"\n");
    fprintf(lammpsfile,"%lf %lf xlo xhi\n",0.0,H[0][0]);
    fprintf(lammpsfile,"%lf %lf ylo yhi\n",0.0,H[1][1]);
    fprintf(lammpsfile,"%lf %lf zlo zhi\n",0.0,H[2][2]);
    if(triclinic) fprintf(lammpsfile,"%lf %lf %lf xy xz yz\n",H[1][0],H[2][0],H[2][1]);
    
    
    fprintf(lammpsfile,"\n");
    fprintf(lammpsfile,"Masses\n");
    fprintf(lammpsfile,"\n");
    for(int i=0;i<no_types;i++)
        fprintf(lammpsfile,"%d %lf\n",i+1,mass[i]);
    
    type0 tmp;
    fprintf(lammpsfile,"\n");
    fprintf(lammpsfile,"Atoms\n");
    fprintf(lammpsfile,"\n");
    for(int i=0;i<natms;i++)
    {
        fprintf(lammpsfile,"%d %d",i+1,type[i]+1);
        for(int j=0;j<3;j++)
        {
            tmp=0.0;
            for(int k=0;k<3;k++)
                tmp+=H[k][j]*s[3*i+k];
            fprintf(lammpsfile," %lf",tmp);
        }
        fprintf(lammpsfile,"\n");
    }
    
    fclose(lammpsfile);
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Box2LAMMPS::~Box2LAMMPS()
{
}


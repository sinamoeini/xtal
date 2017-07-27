#include "command_gr.h"
#include "error.h"
#include "memory.h"
#include "box_collection.h"
#include "gr.h"
#include <math.h>
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_gr::Command_gr(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    if(narg!=8)
    {
        error->warning("gr command needs 7 arguments\n"
        "SYNTAX: gr box dims cutoff k_cutoff nbins file_name file_name");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
    
    bool dims[3];
    dims[0]=dims[1]=dims[2]=false;
    for(int i=0;i<strlen(arg[2]);i++)
    {
        if(arg[2][i]=='x')
        {
            dims[0]=true;
            continue;
        }
        if(arg[2][i]=='y')
        {
            dims[1]=true;
            continue;
        }
        
        if(arg[2][i]=='z')
        {
            dims[2]=true;
            continue;
        }
        
        error->warning("invalid dimension: %c",arg[2][i]);
        return;
    }
    type0 cutoff=atof(arg[3]);
    if(cutoff<=0.0)
    {
        error->warning("cutoff cannot be less than or equal to 0.0");
        return;
    }
    
    type0 k_cutoff=atof(arg[4])*2.0*M_PI;
    if(k_cutoff<=0.0)
    {
        error->warning("k_cutoff cannot be less than or equal to 0.0");
        return;
    }
    
    int no_bins=atoi(arg[5]);
    if(no_bins<=0)
    {
        error->warning("number of bins cannot be less than or equal to 0");
        return;
    }
    
    
    FILE* fp0=fopen(arg[6],"w");
    FILE* fp1=fopen(arg[7],"w");
    
    Box* box=box_collection->boxes[ibox];
    type0*** gr=NULL;
    type0*** sr=NULL;
    Gr gr_(xtal,gr,sr,box,dims,cutoff,k_cutoff,no_bins);
    
    int no_types=box->no_types;
    type0 dr=cutoff/static_cast<type0>(no_bins);
    type0 dk=k_cutoff/static_cast<type0>(no_bins);
    type0 r=0.5*dr,k=0.0;
    
    
    
    fprintf(fp0,"r");
    for(int ityp=0;ityp<no_types;ityp++)
        for(int jtyp=0;jtyp<ityp+1;jtyp++)
            fprintf(fp0," %s-%s",box->atom_names[ityp],box->atom_names[jtyp]);
    fprintf(fp0,"\n");
    
    
    
    
    for(int i=0;i<no_bins;i++,r+=dr)
    {
        fprintf(fp0,"%lf",r);
        for(int ityp=0;ityp<no_types;ityp++)
            for(int jtyp=0;jtyp<ityp+1;jtyp++)
                fprintf(fp0," %lf",gr[ityp][jtyp][i]);
        
        fprintf(fp0,"\n");
    }
    
    fclose(fp0);
    
    
    
    
    
    
    fprintf(fp1,"k");
    for(int ityp=0;ityp<no_types;ityp++)
        for(int jtyp=0;jtyp<ityp+1;jtyp++)
            fprintf(fp1," %s-%s",box->atom_names[ityp],box->atom_names[jtyp]);
    fprintf(fp1,"\n");
    
    
    
    
    for(int i=0;i<no_bins;i++,k+=dk)
    {
        fprintf(fp1,"%lf",k/(2.0*M_PI));
        for(int ityp=0;ityp<no_types;ityp++)
            for(int jtyp=0;jtyp<ityp+1;jtyp++)
                fprintf(fp1," %lf",sr[ityp][jtyp][i]);
        
        fprintf(fp1,"\n");
    }
    
    fclose(fp1);
    
    
    
    delete [] **gr;
    delete [] *gr;
    delete [] gr;
    
    
    delete [] **sr;
    delete [] *sr;
    delete [] sr;
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_gr::~Command_gr()
{
}

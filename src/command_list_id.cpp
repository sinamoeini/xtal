#include <string>
#include <stdlib.h>
#include <math.h>
#include "command_list_id.h"
#include "box_collection.h"
#include "error.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_list_id::Command_list_id(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    if(narg!=7)
    {
        error->warning("list_id command needs 6 arguments\n"
        "SYNTAX: list_id box s/x dim lower_bound upper_bound id_file");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }

    // 1 for s -1 for x
    int s_or_x=0;
    
    if(strcmp(arg[2],"s")==0)
        s_or_x=1;
    else if(strcmp(arg[2],"x")==0)
        s_or_x=-1;
    else
    {
        error->warning("unknown component",arg[2]);
        return;
    }
    int dim;
    if(strcmp(arg[3],"x")==0)
    {
        dim=0;
    }
    else if(strcmp(arg[3],"y")==0)
    {
        dim=1;
    }
    else if(strcmp(arg[3],"z")==0)
    {
        dim=2;
    }
    else
    {
        error->warning("invalid dimension: %s",arg[3]);
        return;
    }
    type0 lwr_bnd=atof(arg[4]);
    type0 uppr_bnd=atof(arg[5]);
    
    if(s_or_x==1)
    {
        if(lwr_bnd>=1.0 || lwr_bnd<0.0)
        {
            error->warning("lower bound for s should be between 0.0 and 1.0");
            return;
        }
        if(uppr_bnd>=1.0 || uppr_bnd<0.0)
        {
            error->warning("upper bound for s should be between 0.0 and 1.0");
            return;
        }
        if(lwr_bnd>=uppr_bnd)
        {
            error->warning("upper bound for s should be greater than lower bond");
            return;
        }
        
    }
    else
    {
        type0** B=box_collection->ret_B(arg[1]);
        type0 max_depth=1.0/sqrt(B[0][dim]*B[0][dim]+B[1][dim]*B[1][dim]+B[2][dim]*B[2][dim]);
        
        if(lwr_bnd>=max_depth || lwr_bnd<0.0)
        {
            error->warning("lower bound for x should be between 0.0 and %lf",max_depth);
            return;
        }
        if(uppr_bnd>=max_depth || uppr_bnd<0.0)
        {
            error->warning("upper bound for x should be between 0.0 and %lf",max_depth);
            return;
        }
        if(lwr_bnd>=uppr_bnd)
        {
            error->warning("upper bound for s should be greater than lower bond");
            return;
        }
        lwr_bnd/=max_depth;
        uppr_bnd/=max_depth;
    }

    
    
    int natms=box_collection->ret_natms(arg[1]);
    type0* s=box_collection->ret_s(arg[1]);
    
    
    FILE* idfile;
    idfile=fopen(arg[6],"w");

    int icomp=0;
    int no=0;
    for(int i=0;i<natms;i++)
    {
        if(lwr_bnd<=s[icomp+dim]&&s[icomp+dim]<uppr_bnd)
            no++;
        icomp+=3;
    }

    fprintf(idfile,"%d\n",no);
    icomp=0;
    
    for(int i=0;i<natms;i++)
    {
        if(lwr_bnd<=s[icomp+dim]&&s[icomp+dim]<uppr_bnd)
            fprintf(idfile,"%d\n",i);
        icomp+=3;
    }
    fclose(idfile);
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_list_id::~Command_list_id()
{
}



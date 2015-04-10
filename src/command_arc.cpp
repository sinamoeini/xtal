#include "command_arc.h"
#include "xmath.h"
#include "region_collection.h"
#include "box_collection.h"
#include "memory.h"
#include "error.h"
#include <cmath>
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_arc::Command_arc(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    
    
    
    if(narg!=9 )
    {
        error->warning("add_region needs 6 arguments\n"
                       "SYNTAX: arc box prim_dim normal_dim lo hi delta");
        return;
    }
    
    
    
    int p,n,ibox;
    //width of the arc
    type0 h,w_ov_l,h_max;
    //number of the unit cells along the arc
    int no,no_pad;
    type0 c_s;
    
    type0 tmp0;
    
    
    
    ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
    
    if(strcmp(arg[2],"x")==0)
    {
        p=0;
    }
    else if(strcmp(arg[2],"y")==0)
    {
        p=1;
    }
    else if(strcmp(arg[2],"z")==0)
    {
        p=2;
    }
    else
    {
        error->warning("invalid dimension for primary axes: %s",arg[2]);
        return;
    }
    
    if(strcmp(arg[3],"x")==0)
    {
        n=0;
    }
    else if(strcmp(arg[3],"y")==0)
    {
        n=1;
    }
    else if(strcmp(arg[3],"z")==0)
    {
        n=2;
    }
    else
    {
        error->warning("invalid dimension for normal axes: %s",arg[3]);
        return;
    }
    
    if(n==p)
    {
        error->warning("normal axes and primary axes cannot be the same");
        return;
    }
    
    no=atoi(arg[4]);
    if(no<1)
    {
        error->warning("number of unit cell should be greater than 1");
        return;
    }
    no_pad=atoi(arg[5]);
    
    if(no_pad<0)
    {
        error->warning("number of pad unit cells should be greater than 0");
        return;
    }
    
    
    w_ov_l=atof(arg[6]);
    
    h_max=atof(arg[7]);
    c_s=atof(arg[8]);
    
    if(c_s<0.0 || c_s>=1.0)
    {
        error->warning("c_s should be between 0.0 & 1.0");
        return;
    }
    

    /*
    
    if(w_ov_l/(1.0+eps) <=0.0 || w_ov_l/(1.0+eps)>=1.0)
    {
        error->warning("w/l must be between 2/pi & 1.0 (%lf)",w_ov_l/(1.0+eps));
        return;
    }
    
    */
    
    
    Box* box=box_collection->boxes[ibox];
    int natms=box->natms;
    int no_types=box->no_types;
    type0* s=box->s;
    char* dof=box->dof;
    int* type=box->type;
    type0** H=box->H;
    type0** B=box->B;
    type0 H_p_norm,B_n_norm,B_n_norm_inv;
    
    
    H_p_norm=sqrt(H[p][0]*H[p][0]+H[p][1]*H[p][1]+H[p][2]*H[p][2]);
    B_n_norm=sqrt(B[0][n]*B[0][n]+B[1][n]*B[1][n]+B[2][n]*B[2][n]);
    B_n_norm_inv=1.0/B_n_norm;
    
    l=static_cast<type0>(no)*H_p_norm;
    w=l*w_ov_l;
    h=2.0*r2(0.25,1.0-w_ov_l);
    
    
    if(h+B_n_norm_inv>h_max)
    {
        error->warning("h_max must be greater than %lf",h+B_n_norm_inv);
        return;
    }
    
    
    
    type0 r_p,r_n;
    type0 t,delta_n,delta_p,traj_p,traj_n;
    type0 e_p;
    type0* e_n;
    CREATE1D(e_n,3);
    
    
    
    r_p=H_p_norm/(w+2.0*H_p_norm*static_cast<type0>(no_pad));
    r_n=1.0/(h_max*B_n_norm);
    
    for(int i=0;i<3;i++)
    {
        e_n[i]=0.0;
        for(int j=0;j<3;j++)
            e_n[i]+=B[j][i]*B[j][n];
    }
    
    tmp0=1.0/sqrt(e_n[n]);
    
    for(int i=0;i<3;i++)
        e_n[i]*=tmp0;
    
    e_n[n]*=r_n;
    e_n[p]*=r_p;
    
    for(int i=0;i<3;i++)
    {
        B[i][n]*=r_n;
        B[i][p]*=r_p;
    }
    
    int iiatm;
    int natms_new=(no+2*no_pad)*natms;
    type0* s_new;
    int* type_new;
    char* dof_new=NULL;
    CREATE1D(s_new,3*natms_new);
    CREATE1D(type_new,natms_new);
    if(box->dof_xst)
        CREATE1D(dof_new,3*natms_new);
    
    e_p=1.0/(w+2.0*H_p_norm*static_cast<type0>(no_pad));
    
    iiatm=0;
    for(int icell=0;icell<no_pad;icell++)
    {
        for(int iatm=0;iatm<natms;iatm++)
        {
            type_new[iiatm]=type[iatm];
            
            for(int i=0;i<3;i++)
                s_new[3*iiatm+i]=s[3*iatm+i];
            
            s_new[3*iiatm+n]*=r_n;
            s_new[3*iiatm+p]*=r_p;
            s_new[3*iiatm+p]+=e_p*static_cast<type0>(icell)*H_p_norm;
            
            if(box->dof_xst)
                for(int i=0;i<3;i++)
                    dof_new[3*iiatm+i]=dof[3*iatm+i];
            
            
            iiatm++;
        }
    }
    
    delta_p=static_cast<type0>(no_pad)*H_p_norm;
    for(int icell=0;icell<no;icell++)
    {
        
        for(int iatm=0;iatm<natms;iatm++)
        {
            type_new[iiatm]=type[iatm];
            
            t=(s[3*iatm+p]+static_cast<type0>(icell))/static_cast<type0>(no);
            
            delta_n=(s[3*iatm+n]-c_s)*B_n_norm_inv;
            
            mid_map(t,h,l,delta_n,delta_p,traj_p,traj_n);
            
            for(int i=0;i<3;i++)
                s_new[3*iiatm+i]=s[3*iatm+i];
            
            s_new[3*iiatm+n]=0.0;
            s_new[3*iiatm+p]=0.0;
            
            traj_n+=c_s*B_n_norm_inv;
            
            for(int i=0;i<3;i++)
                s_new[3*iiatm+i]+=traj_n*e_n[i];
            
            s_new[3*iiatm+p]+=traj_p*e_p;
            
            if(box->dof_xst)
                for(int i=0;i<3;i++)
                    dof_new[3*iiatm+i]=dof[3*iatm+i];
            
            iiatm++;
        }
        
    }
    
    for(int icell=0;icell<no_pad;icell++)
    {
        
        
        for(int iatm=0;iatm<natms;iatm++)
        {
            type_new[iiatm]=type[iatm];
            
            for(int i=0;i<3;i++)
                s_new[3*iiatm+i]=s[3*iatm+i];
            
            s_new[3*iiatm+n]*=r_n;
            s_new[3*iiatm+p]*=r_p;
            s_new[3*iiatm+p]+=e_p*(static_cast<type0>(icell+no_pad)*H_p_norm+w);
            
            if(box->dof_xst)
                for(int i=0;i<3;i++)
                    dof_new[3*iiatm+i]=dof[3*iatm+i];
            
            iiatm++;
        }
    }
    
    delete [] e_n;
    
    
    for(int i=0;i<3;i++)
    {
        H[n][i]/=r_n;
        H[p][i]/=r_p;
    }
    
    if(natms)
    {
        delete [] s;
        delete [] type;
        if(box->dof_xst)
            delete [] dof;
    }
    
    box->natms=natms_new;
    box->s=s_new;
    box->type=type_new;
    if(box->dof_xst)
        box->dof=dof_new;
    
    for(int i=0;i<no_types;i++)
        box->type_count[i]*=(no+2*no_pad);
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_arc::~Command_arc()
{
}
/*--------------------------------------------
 elliptic
 --------------------------------------------*/
inline void Command_arc::mid_map(type0 t,type0 h,type0 l
,type0 delta_n,type0 delta_p,type0& traj_p,type0& traj_n)
{
    traj_p=r1(t,1.0-w/l)+delta_n*n1(t,1.0-w/l)+delta_p;
    traj_n=r2(t,1.0-w/l)+delta_n*n2(t,1.0-w/l);
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
type0 Command_arc::r1(type0 x,type0 r)
{
    if(r==0.0)
        return x*l;
    else
    {
        if(0.0<x && x<=0.25)
            return l*x*(1.0-4.0*r*x);
        else if(0.25<x && x<=0.5)
            return 0.5*w-r1(0.5-x,r);
        else if(0.5<x && x<=0.75)
            return 0.5*w+r1(x-0.5,r);
        else if(0.75<x && x<=1.0)
            return w-r1(1.0-x,r);
        else if(1.0<x)
            return w+(x-1.0)*l;
        else
            return x*l;
    }

}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
type0 Command_arc::r2(type0 x,type0 r)
{
    
    if(r==0.0)
        return 0.0;
    else
    {
        if(0.0<x && x<=0.25)
            return (4.0*(8.0*x*r-1.0)*sqrt(x*r*(1.0-4.0*r*x))
                    +asin(8.0*r*x-1.0)+0.5*M_PI)*l/(16.0*r);
        else if(0.25<x && x<=0.5)
            return 2.0*r2(0.25,r)-r2(0.5-x,r);
        else if(0.5<x && x<=0.75)
            return 2.0*r2(0.25,r)-r2(x-0.5,r);
        else if(0.75<x && x<=1.0)
            return r2(1.0-x,r);
        else
            return 0.0;
    }
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
type0 Command_arc::n1(type0 x,type0 r)
{
    if(0.0<x && x<=0.25)
        return -4.0*sqrt(r*x*(1.0-4.0*r*x));
    else if(0.25<x && x<=0.5)
        return n1(0.5-x,r);
    else if(0.5<x && x<=0.75)
        return -n1(x-0.5,r);
    else if(0.75<x && x<=1.0)
        return -n1(1.0-x,r);
    else if(1.0<x)
        return 0.0;
    else
        return 0.0;
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
type0 Command_arc::n2(type0 x,type0 r)
{
    
    if(r==0.0)
        return 1.0;
    else
    {
        if(0.0<x && x<=0.25)
            return 1.0-8.0*r*x;
        else if(0.25<x && x<=0.5)
            return n2(0.5-x,r);
        else if(0.5<x && x<=0.75)
            return n2(x-0.5,r);
        else if(0.75<x && x<=1.0)
            return n2(1.0-x,r);
        else
            return 1.0;
    }
    
}



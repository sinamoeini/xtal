#include "command_sine_arc.h"
#include "xmath.h"
#include "region_collection.h"
#include "box_collection.h"
#include "memory.h"
#include "error.h"
#include <cmath>
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_sine_arc::Command_sine_arc(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    
    
    
    if(narg!=9 && narg!=10)
    {
        error->warning("add_region needs 6 arguments\n"
        "SYNTAX: arc box prim_dim normal_dim lo hi delta");
        return;
    }
    
    
    
    int p,n,ibox;
    //width of the arc
    type0 h,w_ov_l,l,w,h_max,eps;
    //number of the unit cells along the arc
    int no,no_pad;
    type0 c_s;

    type0 tmp0;

    
    
    ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[3]);
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
    
    if(narg==10)
        eps=atof(arg[9]);
    else
        eps=0.0;
    
    if(eps<-1.0 || eps>1.0)
    {
        error->warning("epsilon must be between 0.0 & 1.0");
        return;
    }
    
    
    if(w_ov_l/(1.0+eps) <=2.0/M_PI || w_ov_l/(1.0+eps)>=1.0)
    {
        error->warning("w/l must be between 2/pi & 1.0 (%lf)",w_ov_l/(1.0+eps));
        return;
    }
    
    
    
    
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
    h=l*(1.0+eps)*solve(w_ov_l/(1.0+eps));
    
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
            
            mid_map(t,h,l*(1.0+eps),delta_n,delta_p,traj_p,traj_n);
            
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
Command_sine_arc::~Command_sine_arc()
{
}
/*--------------------------------------------
 2*EllipticE(pi^2*x^2)/pi
 30 points for Legendre-Gauss Quadrature
 --------------------------------------------*/
type0 Command_sine_arc::elliptic_e(type0 x)
{
    
    type0 r=x*x;
    type0 ans=0.0;
    ans+=0.102852652894*sqrt(1.0-r*0.255829309967);
    ans+=0.101762389748*sqrt(1.0-r*2.13212136323);
    ans+=0.0995934205868*sqrt(1.0-r*5.07855567722);
    ans+=0.0963687371746*sqrt(1.0-r*7.90283101647);
    ans+=0.0921225222378*sqrt(1.0-r*9.59884371249);
    ans+=0.0868997872011*sqrt(1.0-r*9.73952238574);
    ans+=0.0807558952294*sqrt(1.0-r*8.52093017855);
    ans+=0.0737559747377*sqrt(1.0-r*6.52298510176);
    ans+=0.0659742298822*sqrt(1.0-r*4.38473571984);
    ans+=0.0574931562176*sqrt(1.0-r*2.56925061702);
    ans+=0.0484026728306*sqrt(1.0-r*1.28360607207);
    ans+=0.0387991925696*sqrt(1.0-r*0.521093860614);
    ans+=0.0287847078833*sqrt(1.0-r*0.154867331082);
    ans+=0.0184664683111*sqrt(1.0-r*0.0259591547181);
    ans+=0.00796819249617*sqrt(1.0-r*0.000940010860158);
    
    return ans;
}
/*--------------------------------------------
 EllipticE(2pi*y,pi^2*x^2)/pi
 30 points for Legendre-Gauss Quadrature
 --------------------------------------------*/
type0 Command_sine_arc::elliptic_e(type0 x,type0 y)
{
    
    type0 tmp;
    type0 r=x*x*9.86960440109;
    type0 ans=0.0;
    tmp=sin(y*2.97988909115);
    ans+=0.102852652894*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*3.30329621603);
    ans+=0.102852652894*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*2.65819606339);
    ans+=0.101762389748*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*3.62498924379);
    ans+=0.101762389748*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*2.34162715701);
    ans+=0.0995934205868*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*3.94155815017);
    ans+=0.0995934205868*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*2.03353807898);
    ans+=0.0963687371746*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*4.2496472282);
    ans+=0.0963687371746*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*1.7371946473);
    ans+=0.0921225222378*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*4.54599065988);
    ans+=0.0921225222378*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*1.45573817205);
    ans+=0.0868997872011*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*4.82744713513);
    ans+=0.0868997872011*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*1.19215215575);
    ans+=0.0807558952294*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*5.09103315143);
    ans+=0.0807558952294*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*0.949230665843);
    ans+=0.0737559747377*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*5.33395464134);
    ans+=0.0737559747377*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*0.729548713297);
    ans+=0.0659742298822*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*5.55363659388);
    ans+=0.0659742298822*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*0.535434948818);
    ans+=0.0574931562176*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*5.74775035836);
    ans+=0.0574931562176*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*0.368946957997);
    ans+=0.0484026728306*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*5.91423834918);
    ans+=0.0484026728306*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*0.231849388831);
    ans+=0.0387991925696*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*6.05133591835);
    ans+=0.0387991925696*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*0.12559501532);
    ans+=0.0287847078833*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*6.15759029186);
    ans+=0.0287847078833*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*0.0513081039237);
    ans+=0.0184664683111*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*6.23187720326);
    ans+=0.0184664683111*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*0.00975940760934);
    ans+=0.00796819249617*sqrt(1.0-r*tmp*tmp);
    tmp=sin(y*6.27342589957);
    ans+=0.00796819249617*sqrt(1.0-r*tmp*tmp);
    
    return 0.5*y*ans;
}
/*--------------------------------------------
 elliptic
 --------------------------------------------*/
type0 Command_sine_arc::solve(type0 x)
{
    type0 a=0.0;
    type0 b=1.0/M_PI;
    type0 c=0.5*(a+b);
    type0 fc=elliptic_e(c);
    
    while (abs(fc-x)>1.0e-9)
    {
        if(fc>x)
            a=c;
        else
            b=c;
        c=0.5*(a+b);
        fc=elliptic_e(c);
    }
    
    return c;
}
/*--------------------------------------------
 elliptic
 --------------------------------------------*/
inline void Command_sine_arc::mid_map(type0 t,type0 h,type0 l
,type0 delta_n,type0 delta_p,type0& traj_p,type0& traj_n)
{
    type0 tmp0=h*M_PI*sin(2.0*M_PI*t)/l;
    
    traj_p=l*elliptic_e(h/l,t)-tmp0*delta_n+delta_p;
    
    traj_n=0.5*h*(1.0-cos(2.0*M_PI*t))+delta_n*sqrt(1.0-tmp0*tmp0);
    
}





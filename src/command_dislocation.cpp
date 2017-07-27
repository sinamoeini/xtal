#include "command_dislocation.h"
#include "memory.h"
#include "error.h"
#include "box2cfg.h"
#include "box_collection.h"
#include <cmath>
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_dislocation::Command_dislocation(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{

    
    if(narg!=7)
    {
        error->warning("dislocation command needs 6 arguments\n"
        "SYNTAX: dislocation box periodic_dim dislocation_line_dim (n_edge,n_screw) (nx,ny) poisson");
        return;
    }
    
    int ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
    
    Box* box=box_collection->boxes[ibox];
    type0** H=box->H;
    if(H[1][0]!=0.0 || H[2][0]!=0.0 || H[2][1]!=0.0)
    {
        error->warning("box %s must be orthogonal",arg[1]);
        return;
    }
    int disloc_dim;
    int per_dim;
    int other_dim=-1;
    if(strcmp(arg[2],"x")==0)
    {
        per_dim=0;
        b_edge=H[0][0];
    }
    else if(strcmp(arg[2],"y")==0)
    {
        per_dim=1;
        b_edge=H[1][1];
    }
    else if(strcmp(arg[2],"z")==0)
    {
        per_dim=2;
        b_edge=H[2][2];
    }
    else
    {
        error->warning("invalid screw dislocation direction: %s",arg[2]);
        return;
    }
    
    
    if(strcmp(arg[3],"x")==0)
    {
        disloc_dim=0;
        b_screw=H[0][0];
    }
    else if(strcmp(arg[3],"y")==0)
    {
        disloc_dim=1;
        b_screw=H[1][1];
    }
    else if(strcmp(arg[3],"z")==0)
    {
        disloc_dim=2;
        b_screw=H[2][2];
    }
    else
    {
        error->warning("invalid edge dislocation direction: %s",arg[3]);
        return;
    }
    
    if(disloc_dim==per_dim)
    {
        error->warning("periodic dimension and dislocation line orientation must be prependicular top each other: %s",arg[3]);
        return;
    }
    
    for(int i=0;i<3;i++)
        if(i!=disloc_dim && i!=per_dim)
            other_dim=i;
    
    
    
    if(sscanf(arg[4],"(%d,%d)",&n_edge,&n_screw)==2)
    {
        if(n_edge<0 || n_screw<0)
        {
            error->warning("dislocation unit cell size should be greater than or equal to 0");
            return;
        }
    }
    else
    {
        error->warning("edge and screw dislocation unit cell size "
            "should be of the format: (n_edge,n_screw)");
        return;
    }
    
    int m[2];
    
    if(sscanf(arg[5],"(%d,%d)",&m[0],&m[1])==2)
    {
        if(m[0]<=0 || m[1]<=0 )
        {
            error->warning("unit cell size should be greater than 0");
            return;
        }
        
        if(m[0]<=n_edge || m[1]<=n_screw)
        {
            error->warning("unit cell size should be greater than dislocation");
            return;
        }
    }
    else
    {
        error->warning("unit cell size "
                       "should be of the format: (n_x,n_y)");
        return;
    }
    
    
    
    nu=atof(arg[6]);
    if(nu<=0.0 || nu>0.5)
    {
        error->warning("poisson ratio must be betweeen 0.0 and 0.5");
        return;
    }
    
    
    
    n[per_dim]=m[0];
    n[disloc_dim]=1;
    n[other_dim]=m[1];
    
    box->mul(n);
    
    H=box->H;
    
    x_scale=M_PI;
    y_scale=M_PI*H[other_dim][other_dim]/H[per_dim][per_dim];
    H[per_dim][per_dim]-=0.5*static_cast<type0>(n_edge)*b_edge;
    
    
    ux_scale=static_cast<type0>(n_edge)*b_edge/H[per_dim][per_dim];
    uy_scale=static_cast<type0>(n_edge)*b_edge/H[other_dim][other_dim];
    uz_scale=static_cast<type0>(n_screw)*b_screw/H[disloc_dim][disloc_dim];
    
    type0 s_x_lo=0.5*(1.0-static_cast<type0>(n_edge)/static_cast<type0>(n[0]));
    type0 s_x_hi=0.5*(1.0+static_cast<type0>(n_edge)/static_cast<type0>(n[0]));
    type0 sh_x=static_cast<type0>(n[0])/(static_cast<type0>(n[0])-0.5*static_cast<type0>(n_edge));

    /*
     
     s_x<0.5*(1+n_edge/n[0])
     s_x<=0.5*(1-n_edge/n[0])
     s_y>=0.5;
     
     
     */
    
    type0* s=box->s;
    int natms=box->natms;
    if(ux_scale==0)
    {
        /*
        box->add_vac(0,1,100.0);
        box->add_vac(0,-1,100.0);
        uz_scale=static_cast<type0>(n_screw)*b_screw/H[disloc_dim][disloc_dim];
         */
        type0 dsz;
        for(int i=0;i<natms;i++,s+=3)
        {
            disp(s[per_dim],s[other_dim],dsz);
            s[disloc_dim]+=dsz;
            correct_s(s[disloc_dim]);
            
        }
    }
    else if(uz_scale==0)
    {
        int* del_list;
        CREATE1D(del_list,natms);
        int sz=0;
        type0 dsx,dsy;
        for(int i=0;i<natms;i++,s+=3)
        {

            if(s[per_dim]>=s_x_lo && s[per_dim]<s_x_hi && s[other_dim]>=0.5)
                del_list[sz++]=i;
            else
            {
                disp(s[per_dim],s[other_dim],dsx,dsy);
                s[per_dim]*=sh_x;
                s[per_dim]+=dsx;
                s[other_dim]+=dsy;
                
                correct_s(s[per_dim]);
                correct_s(s[other_dim]);
                
                
            }
            
        }
        
        box->del_atoms(sz,del_list);
        delete [] del_list;
    }
    else
    {
        int* del_list;
        CREATE1D(del_list,natms);
        int sz=0;
        type0 dsx,dsy,dsz;
        for(int i=0;i<natms;i++,s+=3)
        {
            if(s[per_dim]>=s_x_lo && s[per_dim]<s_x_hi && s[other_dim]>=0.5)
                del_list[sz++]=i;
            else
            {
                disp(s[per_dim],s[other_dim],dsx,dsy,dsz);
                s[per_dim]*=sh_x;
                s[per_dim]+=dsx;
                s[other_dim]+=dsy;
                s[disloc_dim]+=dsz;
                
                correct_s(s[disloc_dim]);
                correct_s(s[per_dim]);
                correct_s(s[other_dim]);
                
            }
            
        }
        box->del_atoms(sz,del_list);
        delete [] del_list;
    }
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_dislocation::~Command_dislocation()
{
}
/*--------------------------------------------
 displacement for edge dislocation
 --------------------------------------------*/
void Command_dislocation::correct_s(type0& s)
{
    while(s<0.0)
        s++;
    while(s>=1.0)
        s--;
}
/*--------------------------------------------
 displacement for edge dislocation
 --------------------------------------------*/
void Command_dislocation::disp(type0 x,type0 y,type0& ux,type0& uy)
{
    x-=0.5;
    x*=x_scale;
    y-=0.5;
    y*=y_scale;
    if(x==0.0 && y==0.0)
    {
        ux=-0.25;
        uy=0.0;
        ux*=ux_scale;
        uy*=uy_scale;
        return;
    }
    
    type0 sinx,cosx,sinhy,coshy;
    sinx=sin(x);
    cosx=cos(x);
    sinhy=sinh(y);
    coshy=cosh(y);
    type0 tmp=coshy*coshy-cosx*cosx;
    
    ux=atan(sinx*coshy/(cosx*sinhy))-y*sinx*cosx/(tmp*2.0*(1.0-nu));
    ux/=2.0*M_PI;
    
    uy=(0.5-nu)*log(tmp)-y*sinhy*coshy/tmp;
    uy/=4.0*M_PI*(1.0-nu);
    
    if(y>=0.0)
    {
        if(x>0.0)
            ux-=0.5;
        else if(x<0.0)
            ux+=0.5;
    }
    ux-=0.25;
    ux*=ux_scale;
    uy*=uy_scale;
}

/*--------------------------------------------
 displacement for screw dislocation
 --------------------------------------------*/
void Command_dislocation::disp(type0 x,type0 y,type0& uz)
{
    x-=0.5;
    x*=x_scale;
    y-=0.5;
    y*=y_scale;
    
    if(x==0 && y==0)
    {
        uz=0.0;
        return;
    }
    
    uz=atan(tan(x)/tanh(y))/(2.0*M_PI);
    
    if(y>=0.0)
    {
        if(x>0.0)
            uz-=0.5;
        else
            uz+=0.5;
    }
    
    uz*=uz_scale;
}
/*--------------------------------------------
 displacement for mixed dislocation
 --------------------------------------------*/
void Command_dislocation::disp(type0 x,type0 y,type0& ux,type0& uy,type0& uz)
{
    x-=0.5;
    x*=x_scale;
    y-=0.5;
    y*=y_scale;
    
    if(x==0 && y==0)
    {
        ux=-0.25;
        uy=0.0;
        uz=0.0;
        return;
    }
    
    type0 sinx,cosx,sinhy,coshy;
    sinx=sin(x);
    cosx=cos(x);
    sinhy=sinh(y);
    coshy=cosh(y);
    type0 tmp0=coshy*coshy-cosx*cosx;
    type0 tmp1=atan(sinx*coshy/(cosx*sinhy));
    
    ux=(tmp1-y*sinx*cosx/(tmp0*2.0*(1.0-nu)))/(2.0*M_PI);
    uy=((0.5-nu)*log(tmp0)-y*sinhy*coshy/tmp0)/(4.0*M_PI*(1.0-nu));
    uz=tmp1/(2.0*M_PI);
    
    if(y>=0.0)
    {
        if(x>0.0)
        {
            ux-=0.5;
            uz-=0.5;
        }
        else if(x<0.0)
        {
            ux+=0.5;
            uz+=0.5;
        }
    }
    ux-=0.25;
    ux*=ux_scale;
    uy*=uy_scale;
    uz*=uz_scale;
}






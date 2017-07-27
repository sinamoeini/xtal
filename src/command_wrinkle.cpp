#include "memory.h"
#include "xmath.h"
#include "command_wrinkle.h"
#include <cmath>
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_wrinkle::Command_wrinkle(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    
    N=20;
    l=10.0;
    
    
    int icomp;
    type0 ii;
    type0 max_h,f_norm;
    
    //type0 ii,rsin_theta_2,rcos_theta_2;
    
    no=static_cast<type0>(N);
    
    CREATE1D(x,2*(N+4));
    CREATE1D(x0,2*(N+4));
    CREATE1D(xx0,2*(N+4));
    CREATE1D(f,2*(N+4));
    CREATE1D(f0,2*(N+4));
    CREATE1D(h,2*(N+4));
    
    

    
    
    w=1.0*l;
    
    //m=0.000000285*l*l;
    
    m=1.65e-8*l*l;
    m=0.0*l*l;
    
    max_iter=100000;
    tol=1.0e-14;
    slope=0.4;
    norm=1.0e-1*l/no;
    type0 eform,erelax=0.0;
    
    
    
    for(int j=0;j<17;j++)
    {
        
        icomp=0;
        for(int i=0;i<N+4;i++)
        {
            ii=static_cast<type0>(i);
            x[icomp]=r1((ii-1.5)/no,1.0-w/l);
            x[icomp+1]=r2((ii-1.5)/no,1.0-w/l);
            icomp+=2;
        }

        solve();
        
        
        max_h=0.0;
        for(int i=0;i<N+4;i++)
        {
            max_h=MAX(max_h,x[i*2+1]);
        }
        
        f_norm=0.0;
        for(int i=0;i<2*(N+4);i++)
            f_norm+=f[i]*f[i];
        
        if(j==0)
        {
            eform=0.0;
            erelax=energy();
        }
        else
        {
            eform=(erelax-energy())/(erelax*(no));
        }
        
        
        //printf("%lf %lf %lf %e \n",w/l,max_h/l,energy(),f_norm);
        printf("%lf %lf\n",1-w/l,max_h/w);
        //printf("%lf %12.10lf %e \n",1.0-w/l,eform,f_norm);
        w-=0.005*l;
    }
     


    /*
    for(int i=0;i<(N+4);i++)
    {
        printf("f[%d]: %e %e\n",i,x[2*i],x[2*i+1]);
    }
    */
  
    
    delete [] h;
    delete [] f0;
    delete [] f;
    delete [] xx0;
    delete [] x0;
    delete [] x;
    
    
    

    
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_wrinkle::~Command_wrinkle()
{

}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
type0 Command_wrinkle::force()
{
    type0 en;
    type0 del_inv=no/l;
    type0 rsq,dx,dy;
    type0 r0,r1;
    int icomp;

    
    en=l;
    
    for(int i=1;i<N+3;i++)
    {
        icomp=i*2;
        
        if(i!=1 && i!=N+2)
        {
            
            
            r0=sqrt((x[icomp]-x[icomp-4])*(x[icomp]-x[icomp-4])
                    +(x[icomp+1]-x[icomp-3])*(x[icomp+1]-x[icomp-3]));
            
            r1=sqrt((x[icomp]-x[icomp+4])*(x[icomp]-x[icomp+4])
                    +(x[icomp+1]-x[icomp+5])*(x[icomp+1]-x[icomp+5]));
            
            
            f[icomp]=
            0.5*del_inv*(x[icomp+4]+x[icomp-4]-2.0*x[icomp])
            +2.0*m*del_inv*del_inv*del_inv*
            (4.0*x[icomp+2]+4.0*x[icomp-2]-x[icomp+4]-x[icomp-4]-6.0*x[icomp])
            +(x[icomp]-x[icomp-4])/r0
            +(x[icomp]-x[icomp+4])/r1;
            
            f[icomp+1]=
            0.5*del_inv*(x[icomp+5]+x[icomp-3]-2.0*x[icomp+1])
            +2.0*m*del_inv*del_inv*del_inv*
            (4.0*x[icomp+3]+4.0*x[icomp-1]-x[icomp+5]-x[icomp-3]-6.0*x[icomp+1])
            +(x[icomp+1]-x[icomp-3])/r0
            +(x[icomp+1]-x[icomp+5])/r1;
            
        }
        
        
        rsq=(x[icomp+2]-x[icomp-2])*(x[icomp+2]-x[icomp-2])
        +(x[icomp+3]-x[icomp-1])*(x[icomp+3]-x[icomp-1]);
        dx=x[icomp+2]+x[icomp-2]-2.0*x[icomp];
        dy=x[icomp+3]+x[icomp-1]-2.0*x[icomp+1];
        en+=0.25*del_inv*rsq-sqrt(rsq)
        +m*del_inv*del_inv*del_inv*(dx*dx+dy*dy);
    }
    
    
    f[0]=f[1]=f[2]=f[3]=
    f[2*(N+2)]=f[2*(N+2)+1]=f[2*(N+3)]=f[2*(N+3)+1]=0.0;
    
    return en;
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
type0 Command_wrinkle::energy()
{
    type0 en=l;
    type0 del_inv=no/l;
    type0 rsq,dx,dy;
    int icomp;
    
    for(int i=1;i<N+3;i++)
    {
        icomp=i*2;
        rsq=(x[icomp+2]-x[icomp-2])*(x[icomp+2]-x[icomp-2])
        +(x[icomp+3]-x[icomp-1])*(x[icomp+3]-x[icomp-1]);
        
        dx=x[icomp+2]+x[icomp-2]-2.0*x[icomp];
        dy=x[icomp+3]+x[icomp-1]-2.0*x[icomp+1];
        
        en+=0.25*del_inv*rsq-sqrt(rsq)
        +m*del_inv*del_inv*del_inv*(dx*dx+dy*dy);
    }
    
    return en;
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
void Command_wrinkle::solve()
{
    int iter;
    type0 f0_f0,f_h,f_f0,f_f,r,h_norm;
    type0 en,d_en,curr_en;
    type0 alpha,max_alpha;
    int dof=2*(N+4),chk;
    

    
    en=force();
    f0_f0=0.0;
    for(int i=0;i<dof;i++)
        f0_f0+=f[i]*f[i];
    f_f=f_h=f0_f0;
    
    memcpy(h,f,dof*sizeof(type0));
    
    
    iter=0;
    d_en=tol*3.0;
    chk=0;
    while (iter<max_iter && f_f>tol && chk==0)
    {
        memcpy(x0,x,dof*sizeof(type0));
        memcpy(f0,f,dof*sizeof(type0));
        
        h_norm=0.0;
        for(int i=0;i<dof;i++)
            h_norm+=h[i]*h[i];

        h_norm=sqrt(h_norm);
        
        max_alpha=norm/h_norm;
        
        for(int i=2;i<N+2;i++)
        {
            if(h[2*i+1]<0.0)
            {
                max_alpha=MIN(-x[2*i+1]/h[2*i+1],max_alpha);
            }
        }
        max_alpha*=0.99;
        
        curr_en=en;
        chk=1;
        alpha=max_alpha;
        
        //printf("energy :%lf alpha %lf\n",en,alpha);
        while(alpha>0.0 && chk)
        {
            for(int i=0;i<dof;i++)
                x[i]=x0[i]+alpha*h[i];
            
            en=energy();
            
            //printf("kkk energy :%lf\n",en);
            
            if(en<curr_en-slope*f_h*alpha)
                chk=0;

            alpha*=0.5;
        }
        
        if(chk==0)
        {
            en=force();
            
            f_f0=0.0;
            for(int i=0;i<dof;i++)
                f_f0+=f[i]*f0[i];
            
            f_f=0.0;
            for(int i=0;i<dof;i++)
                f_f+=f[i]*f[i];
            
            r=(f_f-f_f0)/f0_f0;
            
            f0_f0=f_f;

            f_h=0.0;
            
            for(int i=0;i<dof;i++)
            {
                h[i]*=r;
                h[i]+=f[i];
                f_h+=f[i]*h[i];
            }
            
            if(f_h<0.0)
            {
                memcpy(h,f,dof*sizeof(type0));
                f_h=f_f;
            }
            
            d_en=fabs(en-curr_en);
        }
        else
        {
            
            memcpy(x,x0,dof*sizeof(type0));
            /*
            memcpy(h,f,dof*sizeof(type0));
            f_f=0.0;
            for(int i=0;i<dof;i++)
                f_f+=f[i]*f[i];
            f_h=f_f;
            chk=0;
            */
            //norm*=1.2;
            //chk=0;
            
        }
        
        
        iter++;
    }
    

    //cout << iter <<endl;
}


/*--------------------------------------------
 destructor
 --------------------------------------------*/
void Command_wrinkle::md()
{
    int dof,iter,chk;
    type0 f_f,en,d_en,f_norm,max_alpha
    ,curr_en,alpha;
    

    
    
    
    
    dof=2*(N+4);
    en=force();
    f_f=0.0;
    for(int i=0;i<dof;i++)
        f_f+=f[i]*f[i];
    f_norm=sqrt(f_f);

    
    iter=0;
    d_en=tol*3.0;
    chk=0;
    while (iter<max_iter && f_f>tol && chk==0)
    {
        
        memcpy(x0,x,dof*sizeof(type0));
        
        max_alpha=norm/f_norm;
        for(int i=2;i<N+2;i++)
        {
            if(f[2*i+1]<0.0)
            {
                max_alpha=MIN(-x[2*i+1]/f[2*i+1],max_alpha);
            }
        }
        max_alpha*=0.99;
        
        curr_en=en;
        chk=1;
        alpha=max_alpha;
        while(alpha>0.0 && chk)
        {
            for(int i=0;i<dof;i++)
                x[i]=x0[i]+alpha*f[i];
            
            en=energy();
            
            //printf("kkk energy :%lf\n",en);
            
            if(en<curr_en-slope*f_f*alpha)
                chk=0;
            
            alpha*=0.5;
        }
        if(chk==0)
        {
            en=force();

            d_en=fabs(en-curr_en);
        }
        else
        {
            
            memcpy(x,x0,dof*sizeof(type0));
            //norm*=1.2;
            //chk=0;
            
        }
        
        
        
        f_norm=0.0;
        for(int i=0;i<dof;i++)
            f_norm+=f[i]*f[i];
        f_norm=sqrt(f_norm);
        
        iter++;
    }
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
type0 Command_wrinkle::r1(type0 x,type0 r)
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
/*--------------------------------------------
 destructor
 --------------------------------------------*/
type0 Command_wrinkle::r2(type0 x,type0 r)
{
    if(r==0.0)
        return 0.0;
    else
    {
        if(0.0<x && x<=0.25)
            return (4.0*(8.0*x*r-1.0)*sqrt(x*r*(1.0-4.0*r*x))+asin(8.0*r*x-1.0)+0.5*M_PI)*l/(16.0*r);
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
void Command_wrinkle::test()
{
    type0 ii,en,den,alpha,en_0;
    alpha=1.0e-17;
    m=0.0001;
    w=0.80;
    
    int icomp=0;
    for(int i=0;i<N+4;i++)
    {
        ii=static_cast<type0>(i);
        x[icomp]=r1((ii-1.5)/no,1.0-w/l);
        x[icomp+1]=r2((ii-1.5)/no,1.0-w/l);
        icomp+=2;
    }


    
    en=force();
    

    
    
    
    den=0.0;
    for(int j=0;j<2*(N+4);j++)
        den+=f[j]*f[j]*alpha;
    for(int j=0;j<2*(N+4);j++)
        x[j]+=f[j]*alpha;
    
    
    en_0=force();
    
    printf("energy %e %e\n",(en_0-en)/alpha,-den/alpha);
}



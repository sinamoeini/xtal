#include <math.h>
#include "command_rotate.h"
#include "memory.h"
#include "error.h"
#include "box_collection.h"
#include "region_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_rotate::Command_rotate(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    int region_xst,iregion,ibox,iarg,natms,icomp;
    
    type0 n_lngth,theta,cos_theta,sin_theta,tmp0,tmp1,tmp2;
    
    type0* n;
    type0* c;
    type0* c_s;
    type0* s_new;
    type0* s;
    
    type0** Q_0;
    type0** Q;
    type0** H;
    type0** B;
    
    if(narg!=5 && narg!=6)
    {
        error->warning("rotate command needs at least 4 arguments\n"
        "SYNTAX: rotate box (c0,c1,c2) (n0,n1,n2) theta\n"
        "or\n"
        "SYNTAX: rotate box region (c0,c1,c2) (n0,n1,n2) theta\n");
        return;
    }

    ibox=box_collection->find(arg[1]);
    if(ibox<0)
    {
        error->warning("box %s not found",arg[1]);
        return;
    }
    
    if(narg==6)
    {
        region_xst=1;
        iregion=region_collection->find(arg[2]);
        if(iregion<0)
        {
            error->warning("region %s not found",arg[2]);
            return;
        }
        iarg=3;
    }
    else
    {
        iregion=-1;
        region_xst=0;
        iarg=2;
    }
    

    CREATE1D(c,3);
    CREATE1D(n,3);
    
    if(sscanf(arg[iarg],"(%lf,%lf,%lf)",&tmp0,&tmp1,&tmp2)==3)
    {
        c[0]=tmp0;
        c[1]=tmp1;
        c[2]=tmp2;
    }
    else
    {
        error->warning("c should be of the format: (c0,c1,c2)");
        delete [] c;
        delete [] n;
        return;
        
    }
    
    iarg++;
    
    if(sscanf(arg[iarg],"(%lf,%lf,%lf)",&tmp0,&tmp1,&tmp2)==3)
    {
        n[0]=tmp0;
        n[1]=tmp1;
        n[2]=tmp2;
    }
    else
    {
        error->warning("n should be of the format: (n0,n1,n2)");
        delete [] c;
        delete [] n;
        return;
        
    }
    
    iarg++;
    
    theta=atof(arg[iarg]);
    theta*=M_PI/180.0;
    cos_theta=cos(theta);
    sin_theta=sin(theta);
    
    
    
    
    /* normalize n */
    n_lngth=sqrt(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]);
    if(n_lngth==0.0)
    {
        error->warning("n size cannot be 0.0");
        delete [] c;
        delete [] n;
        return;
    }
    n[0]/=n_lngth;
    n[1]/=n_lngth;
    n[2]/=n_lngth;

    
    
    
    
    /* calculate Q_0 */
    
    CREATE2D(Q_0,3,3);
    
    Q_0[0][0]=Q_0[1][1]=Q_0[2][2]=cos_theta;
    
    Q_0[2][1]=n[0]*sin_theta;
    Q_0[1][2]=-n[0]*sin_theta;
    
    Q_0[0][2]=n[1]*sin_theta;
    Q_0[2][0]=-n[1]*sin_theta;

    Q_0[1][0]=n[2]*sin_theta;
    Q_0[0][1]=-n[2]*sin_theta;
    
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            Q_0[i][j]+=n[i]*n[j]*(1.0-cos_theta);
    

    
    /* allocate Q */
    CREATE2D(Q,3,3);
    
    /* calculate Q */
    H=box_collection->boxes[ibox]->H;
    B=box_collection->boxes[ibox]->B;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
        {
            Q[i][j]=0.0;
            for(int k=0;k<3;k++)
                for(int l=0;l<3;l++)
                    Q[i][j]+=B[k][i]*H[j][l]*Q_0[k][l];
        }

    
    
    /* claculate c_s */
    CREATE1D(c_s,3);
    for(int i=0;i<3;i++)
    {
        c_s[i]=0.0;
        for(int j=0;j<3;j++)
            for(int k=0;k<3;k++)
            {
                c_s[i]-=B[j][i]*Q_0[j][k]*c[k];
                if(j==k)
                    c_s[i]+=B[j][i]*c[k];                    
            }
    }
    
    /* remove unneccassry pointers: Q_0, c & n */
    
    for(int i=0;i<3;i++)
        delete [] Q_0[i];
    delete [] Q_0;
    delete [] c;
    delete [] n;

    
    /* allocate s_new */
    CREATE1D(s_new,3);
    
    /* get s & natms*/
    s=box_collection->boxes[ibox]->s;
    natms=box_collection->boxes[ibox]->natms;
    
    /* perform rotation*/
    
    icomp=0;
    if(region_xst)
    {
        Region* region=region_collection->regions[iregion];
        for(int iatm=0;iatm<natms;iatm++)
        {
            if(region->belong(H,&s[icomp]))
            {
                M3mV3(Q,&s[icomp],s_new);
                s[icomp]=s_new[0]+c_s[0];
                s[icomp+1]=s_new[1]+c_s[1];
                s[icomp+2]=s_new[2]+c_s[2];
            }
            icomp+=3;
        }
    }
    else
    {
        for(int iatm=0;iatm<natms;iatm++)
        {
            M3mV3(Q,&s[icomp],s_new);
            s[icomp]=s_new[0]+c_s[0];
            s[icomp+1]=s_new[1]+c_s[1];
            s[icomp+2]=s_new[2]+c_s[2];
            icomp+=3;
        }
    }
    
    
    for(int i=0;i<3*natms;i++)
    {
        while(s[i]<0.0)
            s[i]++;
        while(s[i]>=1.0)
            s[i]--;
    }
    

    for(int i=0;i<3;i++)
        delete [] Q[i];
    delete [] Q;
    delete [] c_s;
    delete [] s_new;
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_rotate::~Command_rotate()
{
}

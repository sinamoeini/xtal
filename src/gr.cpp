#include "gr.h"
#include <math.h>
#include "memory.h"
#include "xmath.h"
#include "region_block.h"

/*--------------------------------------------
 constructor
 --------------------------------------------*/
Gr::Gr(Xtal* xtal,type0***& gr_,type0***& sr_,Box*& box_,bool(&dims_)[3],type0& cutoff_,type0& k_cutoff_,int& no_bins_):InitPtrs(xtal),
box(box_),
cutoff(cutoff_),
k_cutoff(k_cutoff_),
dims(dims_),
no_bins(no_bins_),
gr(gr_),
sr(sr_)
{
    ndims=0;
    for(int i=0;i<3;i++) if(dims[i]) ndims++;
    if(ndims==1)
    {
        gr=NULL;
        allocate_gr();
        for(int i=0;i<3;i++) if(dims[i]) prepare_1d(i);
        return;
    }
    
    dr=cutoff/static_cast<type0>(no_bins);
    dk=k_cutoff/static_cast<type0>(no_bins);
    dr_inv=1.0/dr;
    
    
    
    next_bin=NULL;
    bin_list=NULL;
    get_s_cutoff();
    box->add_ghost(dims,s_cutoff);
    
    setup_bins();
    gr=NULL;
    allocate_gr();
    

    int natms=box->natms;
    int igr;
    type0* s=box->s;
    type0 cut_sq=cutoff*cutoff;
    type0 delta;
    
    for(int i=0;i<natms;i++)
    {
        int ibin=find_bin(s+3*i);
        
        for(int in=0;in<nneigh_bins;in++)
        {
            int j=bin_list[ibin+neigh_bins[in]];
            while(j!=-1)
            {
                
                if(j<=i)
                {
                    j=next_bin[j];
                    continue;
                }
                delta=dist(i,j);
                
                if(delta>=cut_sq)
                {
                    j=next_bin[j];
                    continue;
                }
                
                igr=static_cast<int>(sqrt(delta)*dr_inv);
                gr[box->type[i]][box->type[j]][igr]++;
                if(j<natms)
                    gr[box->type[i]][box->type[j]][igr]++;

                j=next_bin[j];
            }
        }
    }
    finalize_gr();
    deallocate_bins();
    box->del_ghost();
    
    
    
    
}
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Gr::~Gr()
{
    
}
/*--------------------------------------------
 
 --------------------------------------------*/
void Gr::finalize_gr()
{
    type0 vol=box->H[0][0]*box->H[1][1]*box->H[2][2];
    type0 area_frac=1.0;
    type0 hm[3];
    int no_dims=0;
    for(int i=0;i<3;i++)
    {
        if(dims[i])
        {
            no_dims++;
            area_frac*=dr;
            continue;
        }
        type0 tmp=0.0;
        for(int j=i;j<3;j++)
            tmp+=box->B[j][i]*box->B[j][i];

        vol*=sqrt(tmp);
    }
    
    if(no_dims==3)
    {
        area_frac*=4.0*M_PI/3.0;
        hm[0]=area_frac;
        hm[1]=3.0*area_frac;
        hm[2]=3.0*area_frac;
    }
    
    if(no_dims==2)
    {
        area_frac*=M_PI;
        hm[0]=area_frac;
        hm[1]=2.0*area_frac;
        hm[2]=0.0;
    }
    
    
    if(no_dims==1)
    {
        area_frac*=M_PI;
        hm[0]=area_frac;
        hm[1]=0.0;
        hm[2]=0.0;
    }

    int no_types=box->no_types;
    
    type0 density=1.0/vol;
    type0 dfrac;
    type0 ii;
    
    type0 tmp,r,k;

    
    for(int ityp=0;ityp<no_types;ityp++)
        for(int jtyp=0;jtyp<no_types;jtyp++)
        {
            dfrac=static_cast<type0>(box->type_count[ityp])*static_cast<type0>(box->type_count[jtyp])*density;
            //dfrac=static_cast<type0>(box->type_count[ityp]);
            for(int i=0;i<no_bins;i++)
            {
                ii=static_cast<type0>(i);
                gr[ityp][jtyp][i]/=dfrac*(hm[0]+hm[1]*ii+hm[2]*ii*ii);
            }

        }
    
    if(ndims==2)
    {
        for(int ityp=0;ityp<no_types;ityp++)
            for(int jtyp=0;jtyp<no_types;jtyp++)
            {
                r=dr*0.5;
                for(int i=0;i<no_bins;i++,r+=dr)
                {
                    tmp=gr[ityp][jtyp][i]*r*dr;
                    k=0.5*dk;
                    
                    for(int j=0;j<no_bins;j++,k+=dk)
                        sr[ityp][jtyp][j]+=tmp*j0(k*r);
                }
            }
    }
    else
    {
        for(int ityp=0;ityp<no_types;ityp++)
            for(int jtyp=0;jtyp<no_types;jtyp++)
            {
                r=dr*0.5;
                for(int i=0;i<no_bins;i++,r+=dr)
                {
                    tmp=gr[ityp][jtyp][i]*r*dr;
                    k=0.5*dk;
                    
                    for(int j=0;j<no_bins;j++,k+=dk)
                        sr[ityp][jtyp][j]+=tmp*sin(k*r)/k;
                }
            }
    }
}
/*--------------------------------------------
 
 --------------------------------------------*/
inline type0 Gr::dist(int& iatm,int& jatm)
{
    
    type0 tmp0,rsq=0.0;
    for(int i=0;i<3;i++)
    {
        if(!dims[i]) continue;
        for(int j=i;j<3;j++)
        {
            tmp0=(box->s[3*iatm+j]-box->s[3*jatm+j])*box->H[j][i];
            rsq+=tmp0*tmp0;
        }
        
    }
    
    return rsq;
}
/*--------------------------------------------
 
 --------------------------------------------*/
void Gr::find_bin(type0* s_,int (&nn)[3])
{
    for(int i=0;i<3;i++)
        if(dims[i])
            nn[i]=static_cast<int>((s_[i]+s_cutoff[i])/s_cutoff[i]);
    
}
/*--------------------------------------------
 
 --------------------------------------------*/
int Gr::find_bin(type0* s_)
{
    int n=0;
    for(int i=0;i<3;i++)
        if(dims[i])
            n+=bin_vec[i]*static_cast<int>((s_[i]+s_cutoff[i])/s_cutoff[i]);
    
    return n;
}
/*--------------------------------------------
 
 --------------------------------------------*/
void Gr::get_s_cutoff()
{
    type0** B=box->B;
    type0 tmp;
    for(int idim=0;idim<3;idim++)
    {
        tmp=0.0;
        for(int jdim=idim;jdim<3;jdim++)
            tmp+=B[jdim][idim]*B[jdim][idim];
        
        s_cutoff[idim]=sqrt(tmp)*cutoff;
    }
}
/*--------------------------------------------
 
 --------------------------------------------*/
void Gr::setup_bins()
{
    for(int i=0;i<3;i++)
    {
        if(dims[i])
            nbins[i]=static_cast<int>((2.0*s_cutoff[i]+1.0)/s_cutoff[i])+1;
        else
            nbins[i]=1;
    }
    
    bin_vec[0]=1;
    bin_vec[1]=nbins[0];
    bin_vec[2]=nbins[0]*nbins[1];
    
    bin_list=new int[nbins[0]*nbins[1]*nbins[2]];
    

    for(int i=0;i<nbins[0]*nbins[1]*nbins[2];i++)
        bin_list[i]=-1;
    
    

    
    
    nneigh_bins=1;
    for(int i=0;i<3;i++)
        if(dims[i])
            nneigh_bins*=3;
    neigh_bins=new int[nneigh_bins];
    
    int lo_lim[3];
    int hi_lim[3];
    for(int i=0;i<3;i++)
    {
        if(dims[i])
        {
            lo_lim[i]=-1;
            hi_lim[i]=2;
        }
        else
        {
            lo_lim[i]=0;
            hi_lim[i]=1;
        }
        
    }
    
    int ibin=0;
    for(int i0=lo_lim[0];i0<hi_lim[0];i0++)
        for(int i1=lo_lim[1];i1<hi_lim[1];i1++)
            for(int i2=lo_lim[2];i2<hi_lim[2];i2++)
                neigh_bins[ibin++]=i0*bin_vec[0]+i1*bin_vec[1]+i2*bin_vec[2];
    
    
    
    
    //bin the atoms and create the link list
    int t_natms=box->natms+box->nghosts;
    
    next_bin=new int[t_natms];
    type0* s=box->s;
    for(int i=t_natms-1;i>-1;i--)
    {
        ibin=find_bin(s+3*i);
        next_bin[i]=bin_list[ibin];
        bin_list[ibin]=i;
    }
    
}
/*--------------------------------------------
 
 --------------------------------------------*/
void Gr::deallocate_bins()
{
    delete [] bin_list;
    delete [] next_bin;
    delete [] neigh_bins;
}
/*--------------------------------------------
 
 --------------------------------------------*/
void Gr::allocate_gr()
{
    int no_types=box->no_types;
    gr=new type0**[no_types];
    *gr=new type0*[no_types*no_types];
    type0* tmp_gr=new type0[no_types*(no_types+1)*no_bins/2];
    
    for(int i=1;i<no_types;i++)
        gr[i]=gr[i-1]+no_types;
    
    for(int i=0;i<no_types;i++)
        for(int j=0;j<i+1;j++)
        {
            gr[i][j]=tmp_gr;
            gr[j][i]=tmp_gr;
            tmp_gr+=no_bins;
        }
    
    for(int i=0;i<no_types*(no_types+1)*no_bins/2;i++)
        gr[0][0][i]=0.0;
    
    
    sr=new type0**[no_types];
    *sr=new type0*[no_types*no_types];
    type0* tmp_sr=new type0[no_types*(no_types+1)*no_bins/2];
    
    for(int i=1;i<no_types;i++)
        sr[i]=sr[i-1]+no_types;
    
    for(int i=0;i<no_types;i++)
        for(int j=0;j<i+1;j++)
        {
            sr[i][j]=tmp_sr;
            sr[j][i]=tmp_sr;
            tmp_sr+=no_bins;
        }
    
    
    type0 k;
    
    if(ndims==1)
    {
        for(int i=0;i<no_types*(no_types+1)*no_bins/2;i++)
            sr[0][0][i]=0.0;
    }
    else if(ndims==2)
    {
        for(int ityp=0;ityp<no_types;ityp++)
            for(int jtyp=0;jtyp<no_types;jtyp++)
            {
                k=0.5*dk;
                for(int i=0;i<no_bins;i++,k+=dk)
                    sr[ityp][jtyp][i]=-cutoff*j1(cutoff*k)/k;
            }
    }
    else
    {
        for(int ityp=0;ityp<no_types;ityp++)
            for(int jtyp=0;jtyp<no_types;jtyp++)
            {
                k=0.5*dk;
                for(int i=0;i<no_bins;i++,k+=dk)
                    sr[ityp][jtyp][i]=(k*cutoff*cos(k*cutoff)-sin(k*cutoff))/(k*k*k);

            }
    }
    
            
    
    
}
/*--------------------------------------------
 
 --------------------------------------------*/
void Gr::prepare_2d()
{
    int dim0=0,dim1=0;
    int xtra_dim=0;
    if(!dims[0])
    {
        xtra_dim=0;
        dim0=1;
        dim1=2;
    }
    if(!dims[1])
    {
        xtra_dim=1;
        dim0=2;
        dim1=0;
    }
    
    if(!dims[2])
    {
        xtra_dim=2;
        dim0=0;
        dim1=1;
    }
    
    Box* tmp_box=new Box(*box);
    for(int i=0;i<xtra_dim;i++)
        tmp_box->H[xtra_dim][i]=0.0;
    type0 det;
    M3INV(tmp_box->H,tmp_box->B,det);
    
    
    
    
    int natms=tmp_box->natms;
    type0* old_s=box->s;
    type0* s=tmp_box->s;
    type0 x[3];
    type0** old_H=box->H;
    type0** B=tmp_box->B;
    
    x[0]=x[1]=x[2]=0.0;
    for(int i=0;i<natms;i++)
    {
        x[dim0]=x[dim1]=0.0;
        for(int j=dim0;j<3;j++)
            x[dim0]+=old_H[j][dim0]*old_s[3*i+j];
        for(int j=dim0;j<3;j++)
            x[dim0]+=old_H[j][dim0]*old_s[3*i+j];
        for(int j=0;j<3;j++)
            s[3*i+j]=0.0;
        
        for(int j=0;j<3;j++)
            for(int k=j;k<3;k++)
                s[3*i+j]=B[k][j]*x[k];
    }
    
}
/*--------------------------------------------
 
 --------------------------------------------*/
void Gr::prepare_1d(int idim)
{
    
    type0 l,length=box->H[idim][idim];
    cutoff=length;
    
    dr=cutoff/static_cast<type0>(no_bins);
    dk=k_cutoff/static_cast<type0>(no_bins);
    dr_inv=1.0/dr;
    
    type0* s=box->s;
    type0** H=box->H;
    int* type=box->type;
    int natms=box->natms;
    int no_types=box->no_types;

    int ibin;
    
    
    type0** bin_count=new type0*[no_types];
    *bin_count=new type0[no_types*no_bins];
    for(int i=1;i<no_types;i++)
        bin_count[i]=bin_count[i-1]+no_types;
    for(int i=0;i<no_types*no_bins;i++)
        bin_count[0][i]=0.0;
    
    for(int i=0;i<natms;i++)
    {
        l=0.0;
        for(int j=idim;j<3;j++)
            l+=s[3*i+j]*H[j][idim];
        while(l>=length)
            l-=length;
        while(l<0.0)
            l+=length;
        ibin=static_cast<int>(l*dr_inv);
        bin_count[type[i]][ibin]++;
    }
    
    type0 rat;
    for(int it=0;it<no_types;it++)
        for(int jt=it;jt<no_types;jt++)
            for(int i=0;i<no_bins;i++)
            {
                gr[it][jt][0]+=2.0*bin_count[jt][i]*bin_count[it][i];
                for(int j=i+1;j<no_bins;j++)
                {
                    rat=bin_count[jt][j]*bin_count[it][i];
                    gr[it][jt][j-i]+=rat;
                    gr[it][jt][i-j+no_bins]+=rat;
                }
            }
    type0 fac;
    for(int it=0;it<no_types;it++)
        for(int jt=it;jt<no_types;jt++)
        {
            fac=(length/dr)/static_cast<type0>(box->type_count[it]*box->type_count[jt]);
            for(int i=0;i<no_bins;i++)
                gr[it][jt][i]*=fac;
        }
    
    
    type0 k,r,tmp;
    for(int it=0;it<no_types;it++)
        for(int jt=it;jt<no_types;jt++)
        {
            k=0.0;
            for(int i=0;i<no_bins;i++,k+=dk)
            {
                
                r=0.0;
                tmp=0.0;
                for(int j=0;j<no_bins;j++,r+=dr)
                {
                    tmp+=gr[it][jt][j]*cos(r*k);
                }
                
                sr[it][jt][i]=tmp;
            }
        }
    
    
    delete [] *bin_count;
    delete [] bin_count;
}




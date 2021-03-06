#include "box.h"
#include <math.h>
#include "memory.h"
#include "xmath.h"

/*--------------------------------------------
 constructor
 --------------------------------------------*/
Box::Box(Xtal* xtal):InitPtrs(xtal)
{
    natms=nghosts=no_types=dof_xst=0;
    CREATE2D(H,3,3);
    CREATE2D(B,3,3);
}
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Box::Box(Xtal* xtal,char* name):InitPtrs(xtal),
atom_names(NULL),
mass(NULL),
type_count(NULL),
s(NULL),
type(NULL),
dof(NULL)
{
    natms=nghosts=no_types=dof_xst=0;
    CREATE2D(H,3,3);
    CREATE2D(B,3,3);
    int lngth=static_cast<int>(strlen(name))+1;
    CREATE1D(box_name,lngth);
    memcpy(box_name,name,lngth*sizeof(char));
}
/*--------------------------------------------
 int dof_xst;
 char* box_name;
 
 int no_types;
 char** atom_names;
 type0* mass;
 int* type_count;
 
 type0** H;
 type0** B;
 
 int natms;
 type0* s;
 int* type;
 char* dof;
 --------------------------------------------*/
Box::Box(Box& other):InitPtrs(other.xtal)
{
    natms=nghosts=no_types=dof_xst=0;
    CREATE2D(H,3,3);
    CREATE2D(B,3,3);
    
    box_name=NULL;
    
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
        {
            B[i][j]=other.B[i][j];
            H[i][j]=other.H[i][j];
        }
    
    natms=other.natms;
    nghosts=other.nghosts;
    no_types=other.no_types;
    dof_xst=other.dof_xst;
    
    
    
    CREATE1D(mass,no_types);
    CREATE1D(type_count,no_types);
    CREATE1D(atom_names,no_types);
    for(int i=0;i<no_types;i++)
    {
        int len=static_cast<int>(strlen(other.atom_names[i]))+1;
        CREATE1D(atom_names[i],len);
        memcpy(atom_names[i],other.atom_names[i],len*sizeof(char));
        
    }
    
    
    CREATE1D(type,natms);
    CREATE1D(s,3*natms);
    if(dof_xst)
        CREATE1D(dof,natms*3);
    
    memcpy(type,other.type,natms*sizeof(int));
    memcpy(s,other.s,3*natms*sizeof(type0));
    if(dof_xst)
        memcpy(dof,other.dof,3*natms*sizeof(char));
}
/*--------------------------------------------
 change the name of the box
 --------------------------------------------*/
void Box::chng_name(char* name)
{
    delete [] box_name;
    int lngth=static_cast<int>(strlen(name))+1;
    CREATE1D(box_name,lngth);
    memcpy(box_name,name,lngth*sizeof(char));
}
/*--------------------------------------------
 constructor
 --------------------------------------------*/
void Box::join(Box* box0,Box* box1,int dim)
{
    if(no_types)
    {
        for(int i=0;i<no_types;i++)
        {
            delete [] atom_names[i];
        }
        delete [] atom_names;
        delete [] mass;
        delete [] type_count;
    }
    
    if(natms)
    {
        delete [] s;
        delete [] type;
        
        if(dof_xst)
            delete [] dof;
    }
    natms=no_types=dof_xst=0;
    
    //allocate s and type
    natms=box0->natms+box1->natms;
    CREATE1D(s,natms*3);
    CREATE1D(type,natms);
    
    dof_xst=box0->dof_xst;
    if(dof_xst)
        CREATE1D(dof,natms*3);
    
    // add the tyoes
    for(int i=0;i<box0->no_types;i++)
    {
        add_type(box0->mass[i],box0->atom_names[i]);
        type_count[i]=box0->type_count[i];
    }
    
    
    int* type_ref;
    CREATE1D(type_ref,box1->no_types);
    for(int i=0;i<box1->no_types;i++)
    {
        type_ref[i]=add_type(box1->mass[i],box1->atom_names[i]);
        type_count[type_ref[i]]+=box1->type_count[i];
    }
    
    for(int iatom=0;iatom<box0->natms;iatom++)
        type[iatom]=box0->type[iatom];
    
    if(dof_xst)
        for(int iatom=0;iatom<3*box0->natms;iatom++)
            dof[iatom]=box0->dof[iatom];
    
    for(int iatom=0;iatom<box1->natms;iatom++)
        type[box0->natms+iatom]=type_ref[box1->type[iatom]];

    if(dof_xst)
        for(int iatom=0;iatom<3*box1->natms;iatom++)
            dof[3*box0->natms+iatom]=box1->dof[iatom];
    
    
    delete [] type_ref;
    // we are done with type
    
    
    
    
    
    // pre-req to figure out new s
    type0 r0,r1,rtot;
    type0** B0=box0->B;
    r0=1.0/sqrt(B0[0][dim]*B0[0][dim]+B0[1][dim]*B0[1][dim]+B0[2][dim]*B0[2][dim]);
    B0=box1->B;
    r1=1.0/sqrt(B0[0][dim]*B0[0][dim]+B0[1][dim]*B0[1][dim]+B0[2][dim]*B0[2][dim]);
    rtot=r0+r1;
    r0/=rtot;
    r1/=rtot;
    
    type0 smin_0=1.0,smin_1=1.0,smax_0=0.0,smax_1=0.0,del_s;
    
    for(int i=0;i<box0->natms;i++)
    {
        smin_0=MIN(smin_0,box0->s[3*i+dim]);
        smax_0=MAX(smax_0,box0->s[3*i+dim]);
    }
    smin_0*=r0;
    smax_0*=r0;
    for(int i=0;i<box1->natms;i++)
    {
        smin_1=MIN(smin_1,box1->s[3*i+dim]);
        smax_1=MAX(smax_1,box1->s[3*i+dim]);
    }
    
    smin_1*=r1;
    smin_1+=r0;
    smax_1*=r1;
    smax_1+=r0;
    del_s=0.5*((1.0+smin_0-smax_1)-(smin_1-smax_0));
    
    
    
    // taking care of s
    int icomp=0;
    for(int i=0;i<box0->natms;i++)
    {
        for(int j=0;j<3;j++)
            s[icomp+j]=box0->s[3*i+j];
        
        s[icomp+dim]*=r0;
        icomp+=3;
    }
    
    for(int i=0;i<box1->natms;i++)
    {
        for(int j=0;j<3;j++)
            s[icomp+j]=box1->s[3*i+j];
        
        s[icomp+dim]*=r1;
        s[icomp+dim]+=r0+del_s;
        icomp+=3;
    }
    
    
    
    // taking care of H and B
    for(int i=0;i<3;i++)
    {
        if(i!=dim)
        {
            for(int j=0;j<3;j++)
                H[i][j]=0.5*(box0->H[i][j]+box1->H[i][j]);
        }
        else
        {
            for(int j=0;j<3;j++)
                H[i][j]=box0->H[i][j]+box1->H[i][j];
        }
        
    }
    
    M3INV_TRI_LOWER(H,B);
}
/*--------------------------------------------
 constructor
 --------------------------------------------*/
void Box::join(Box* box0,Box* box1)
{
    if(no_types)
    {
        for(int i=0;i<no_types;i++)
        {
            delete [] atom_names[i];
        }
        delete [] atom_names;
        delete [] mass;
        delete [] type_count;
    }
    
    if(natms)
    {
        delete [] s;
        delete [] type;
        
        if(dof_xst)
            delete [] dof;
    }
    natms=no_types=dof_xst=0;
    
    //allocate s and type
    natms=box0->natms+box1->natms;
    CREATE1D(s,natms*3);
    CREATE1D(type,natms);
    
    dof_xst=box0->dof_xst;
    if(dof_xst)
        CREATE1D(dof,natms*3);
    
    // add the types
    for(int i=0;i<box0->no_types;i++)
    {
        add_type(box0->mass[i],box0->atom_names[i]);
        type_count[i]=box0->type_count[i];
    }
    
    
    int* type_ref;
    CREATE1D(type_ref,box1->no_types);
    for(int i=0;i<box1->no_types;i++)
    {
        type_ref[i]=add_type(box1->mass[i],box1->atom_names[i]);
        type_count[type_ref[i]]+=box1->type_count[i];
    }
    
    for(int iatom=0;iatom<box0->natms;iatom++)
        type[iatom]=box0->type[iatom];
    
    if(dof_xst)
        for(int iatom=0;iatom<3*box0->natms;iatom++)
            dof[iatom]=box0->dof[iatom];
    
    for(int iatom=0;iatom<box1->natms;iatom++)
        type[box0->natms+iatom]=type_ref[box1->type[iatom]];
    
    if(dof_xst)
        for(int iatom=0;iatom<3*box1->natms;iatom++)
            dof[3*box0->natms+iatom]=box1->dof[iatom];
    
    
    delete [] type_ref;
    // we are done with type
    
    
    type0 ** A;
    CREATE2D(A,3,3);
    
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
        {
            A[i][j]=0.0;
                for(int k=0;k<3;k++)
                    A[i][j]+=box0->B[k][i]*box1->H[j][k];
            
        }
    

    // taking care of s
    int icomp=0;
    for(int i=0;i<box0->natms;i++)
    {
        for(int j=0;j<3;j++)
            s[icomp+j]=box0->s[3*i+j];
        icomp+=3;
    }
    
    for(int i=0;i<box1->natms;i++)
    {
        for(int j=0;j<3;j++)
        {
            s[icomp+j]=0.0;
            for(int k=0;k<3;k++)
                s[icomp+j]+=A[j][k]*box1->s[3*i+k];
        }
        
        icomp+=3;
    }
    
    for(int i=0;i<3;i++)
        delete [] A[i];
    delete [] A;
    
    // taking care of H and B
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
        {
            H[i][j]=box0->H[i][j];
            B[i][j]=box0->B[i][j];
        }
    
}
/*--------------------------------------------
 desstructor
 --------------------------------------------*/
Box::~Box()
{
    delete [] box_name;
    
    if(no_types)
    {
        for(int i=0;i<no_types;i++)
        {
            delete [] atom_names[i];
        }
        delete [] atom_names;
        delete [] mass;
    }
    
    
    for(int i=0;i<3;i++)
    {
        delete [] H[i];
        delete [] B[i];
    }
    delete [] H;
    delete [] B;
    
    
    if(natms)
    {
        delete [] s;
        delete [] type;
        
        if(dof_xst)
            delete [] dof;
    }
    
}
/*--------------------------------------------
 find a type
 --------------------------------------------*/
int Box::find_type(char* name)
{
    int type=-1;
    for (int i=0;i<no_types;i++)
        if(!strcmp(name,atom_names[i]))
        {
            type=i;
            return type;
        }
    
    return type;
}
/*--------------------------------------------
 add a type
 --------------------------------------------*/
int Box::add_type(type0 m,char* name)
{
    
    int type=-1;
    for (int i=0;i<no_types;i++)
        if(!strcmp(name,atom_names[i]))
            type=i;
    
    if (type!=-1)
        return (type);
    
    
    GROW(type_count,no_types,no_types+1);
    GROW(atom_names,no_types,no_types+1);
    GROW(mass,no_types,no_types+1);
    type_count[no_types]=0;
    int lngth= static_cast<int>(strlen(name))+1;
    CREATE1D(atom_names[no_types],lngth);
    memcpy(atom_names[no_types],name,lngth*sizeof(char));
    mass[no_types]=m;
    
    no_types++;
    return (no_types-1);
}

/*--------------------------------------------
 add an array of atoms
 --------------------------------------------*/
void Box::add_atoms(int no,int* type_buff,type0* s_buff)
{
    
    
    GROW(s,natms*3,(natms+no)*3);
    GROW(type,natms,natms+no);
    
    for(int i=0;i<no*3;i++)
    {
        while(s_buff[i]<0.0)
            s_buff[i]++;
        while(s_buff[i]>=1.0)
            s_buff[i]--;
        s[natms*3+i]=s_buff[i];
    }
    for(int i=0;i<no;i++)
    {
        type[natms+i]=type_buff[i];
        type_count[type_buff[i]]++;
    }
    
    natms+=no;
}
/*--------------------------------------------
 add an array of atoms
 --------------------------------------------*/
void Box::add_atoms(int no,int* type_buff
,type0* s_buff,char* dof_buff)
{
    
    
    GROW(s,natms*3,(natms+no)*3);
    GROW(type,natms,natms+no);
    if(dof_xst)
        GROW(dof,natms*3,(natms+no)*3);
    
    for(int i=0;i<no*3;i++)
    {
        while(s_buff[i]<0.0)
            s_buff[i]++;
        while(s_buff[i]>=1.0)
            s_buff[i]--;
        s[natms*3+i]=s_buff[i];
    }
    for(int i=0;i<no;i++)
    {
        type[natms+i]=type_buff[i];
        type_count[type_buff[i]]++;
    }
    
    if(dof_xst)
        for(int i=0;i<no*3;i++)
            dof[natms*3+i]=dof_buff[i];
    
    natms+=no;
}
/*--------------------------------------------
 del a list of atoms
 --------------------------------------------*/
void Box::del_atoms(int no,int* lst)
{
    int tmp_natms=natms;
    for(int i=no-1;i>-1;i--)
    {
        type_count[type[lst[i]]]--;
        memcpy(&s[3*lst[i]],&s[3*(natms-1)],3*sizeof(type0));
        if(dof_xst)
            memcpy(&dof[3*lst[i]],&dof[3*(natms-1)],3*sizeof(char));
        memcpy(&type[lst[i]],&type[natms-1],sizeof(int));
        
        natms--;
    }
    
    type0* s_tmp;
    int* type_tmp;
    char* dof_tmp=NULL;
    CREATE1D(s_tmp,3*natms);
    CREATE1D(type_tmp,natms);
    
    if(dof_xst)
    {
        CREATE1D(dof_tmp,3*natms);
        memcpy(dof_tmp,dof,3*natms*sizeof(char));
    }
    
    memcpy(s_tmp,s,3*natms*sizeof(type0));
    memcpy(type_tmp,type,natms*sizeof(int));
    
    if(tmp_natms)
    {
        delete [] s;
        delete [] type;
        if(dof_xst)
            delete [] dof;
    }
    
    s=s_tmp;
    type=type_tmp;
    if(dof_xst)
        dof=dof_tmp;
    
    for(int itype=no_types-1;itype>-1;itype--)
        if(type_count[itype]==0)
            del_type(itype);

}
/*--------------------------------------------
 del a all of atoms
 --------------------------------------------*/
void Box::del_atoms()
{
    
    if(no_types)
    {
        for(int i=0;i<no_types;i++)
        {
            delete [] atom_names[i];
        }
        delete [] atom_names;
        delete [] mass;
    }
    
    if(natms)
    {
        delete [] s;
        delete [] type;
        
        if(dof_xst)
            delete [] dof;
    }
    natms=no_types=0;
    
}
/*--------------------------------------------
 change the name of box
 --------------------------------------------*/
void Box::change_name(char* name)
{
    delete [] box_name;
    int lngth=static_cast<int>(strlen(name))+1;
    CREATE1D(box_name,lngth);
    memcpy(box_name,name,lngth*sizeof(char));
}
/*--------------------------------------------
 change the name of box
 --------------------------------------------*/
void Box::add_name(char* name)
{
    int lngth=static_cast<int>(strlen(name))+1;
    CREATE1D(box_name,lngth);
    memcpy(box_name,name,lngth*sizeof(char));
}
/*--------------------------------------------
 multiple box by three numbers
 --------------------------------------------*/
void Box::mul(int* n)
{
    
    type0* nn;
    type0* r;
    int* ii;
    CREATE1D(nn,3);
    CREATE1D(r,3);
    CREATE1D(ii,3);
    for(int i=0;i<3;i++)
        nn[i]=static_cast<type0>(n[i]);

    
    for(int i=0;i<3;i++)
        r[i]=1.0/nn[i];
    
    
    int tmp_natms=n[0]*n[1]*n[2]*natms;
    
    
    
    type0* tmp_s;
    int* tmp_type;
    char* tmp_dof=NULL;
    
    CREATE1D(tmp_s,tmp_natms*3);
    CREATE1D(tmp_type,tmp_natms);
    if(dof_xst)
        CREATE1D(tmp_dof,tmp_natms*3);
    
    int icomp=0;
    int tmp_icomp=0;
    int tmp_iatm=0;
    
    for(ii[0]=0;ii[0]<n[0];ii[0]++)
        for(ii[1]=0;ii[1]<n[1];ii[1]++)
            for(ii[2]=0;ii[2]<n[2];ii[2]++)
            {
                icomp=0;
                for(int iatm=0;iatm<natms;iatm++)
                {
                    for(int i=0;i<3;i++)
                    {
                        tmp_s[tmp_icomp+i]=s[icomp+i]*r[i]+ii[i]*r[i];
                        if(dof_xst)
                            tmp_dof[tmp_icomp+i]=dof[icomp+i];
                    }
                    
                    tmp_type[tmp_iatm]=type[iatm];
                    
                    
                    icomp+=3;
                    tmp_iatm++;
                    tmp_icomp+=3;
                }
            }
    if(natms)
    {
        delete [] type;
        delete [] s;
        if(dof_xst)
            delete [] dof;
    }
    
    s=tmp_s;
    type=tmp_type;
    
    if(dof_xst)
        dof=tmp_dof;
    
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            H[i][j]*=nn[i];
    
    M3INV_TRI_LOWER(H,B);
    
    delete [] ii;
    delete [] r;
    delete [] nn;
    
    natms=tmp_natms;
    for(int i=0;i<no_types;i++)
        type_count[i]*=n[0]*n[1]*n[2];
}
/*--------------------------------------------
 add vaccuum at the top of the box
 --------------------------------------------*/
void Box::add_vac(int dim,int t_b,type0 thickness)
{
    
    type0 old_thickness=
    1.0/sqrt(B[0][dim]*B[0][dim]+B[1][dim]*B[1][dim]+B[2][dim]*B[2][dim]);
    type0 ratio=old_thickness/(old_thickness+thickness);
    int icomp=0;
    
    if(t_b==1)
    {
        for(int iatm=0;iatm<natms;iatm++)
        {
            s[icomp+dim]*=ratio;
            icomp+=3;
        }
    }
    else
    {
        for(int iatm=0;iatm<natms;iatm++)
        {
            s[icomp+dim]*=ratio;
            s[icomp+dim]+=1.0-ratio;
            icomp+=3;
        }
    }

    
    ratio=(old_thickness+thickness)/old_thickness;
    for(int i=0;i<3;i++)
        H[dim][i]*=ratio;
    
    M3INV_TRI_LOWER(H,B);
}
/*--------------------------------------------
 add vaccuum at the top of the box
 --------------------------------------------*/
void Box::ucell_chang(type0** u)
{
    type0 det_u,tmp0;
    int chk,icomp,natms_new;
    
    int* lo_bound;
    int* hi_bound;
    type0* s_tmp;
    type0* b;
    type0* sq;
    //type0* d;
    
    
    type0** n;
    type0** H_new;
    type0** H_x;
    
    
    type0* s_new=NULL;
    int* type_new=NULL;
    char* dof_new=NULL;
    

    CREATE1D(lo_bound,3);
    CREATE1D(hi_bound,3);
    CREATE1D(s_tmp,3);
    CREATE1D(b,3);
    CREATE1D(sq,3);
    
    CREATE2D(H_x,3,3);
    CREATE2D(H_new,3,3);
    CREATE2D(n,3,3);
    
    
    
    M3NORMAL(u,n);
    det_u=M3DET(u);
    // find the lo_bound and hi_bound
    for(int i=0;i<3;i++)
    {
        lo_bound[i]=hi_bound[i]=0.0;
        for(int j=0;j<3;j++)
        {
            lo_bound[i]=MIN(lo_bound[i],static_cast<int>(u[j][i]));
            hi_bound[i]=MAX(hi_bound[i],static_cast<int>(u[j][i]));
        }
        tmp0=0.0;
        for(int j=0;j<3;j++)
            tmp0+=u[j][i];
        
        lo_bound[i]=MIN(lo_bound[i],static_cast<int>(tmp0));
        hi_bound[i]=MAX(hi_bound[i],static_cast<int>(tmp0));
        
        for(int j=0;j<3;j++)
        {
            lo_bound[i]=MIN(lo_bound[i],static_cast<int>(tmp0-u[j][i]));
            hi_bound[i]=MAX(hi_bound[i],static_cast<int>(tmp0-u[j][i]));
        }
    }

    
    
    icomp=0;
    natms_new=0;
    for(int iatm=0;iatm<natms;iatm++)
    {
        
        for(int i0=lo_bound[0];i0<hi_bound[0];i0++)
        {
            for(int i1=lo_bound[1];i1<hi_bound[1];i1++)
            {
                for(int i2=lo_bound[2];i2<hi_bound[2];i2++)
                {
                    s_tmp[0]=s[icomp]+static_cast<type0>(i0);
                    s_tmp[1]=s[icomp+1]+static_cast<type0>(i1);
                    s_tmp[2]=s[icomp+2]+static_cast<type0>(i2);
                    M3mV3(n,s_tmp,b);
                    for(int i=0;i<3;i++)
                        b[i]/=det_u;
                    
                    chk=1;
                    for(int i=0;i<3;i++)
                        if(b[i]<0.0 || b[i]>=1.0)
                            chk=0;
                    
                    if(chk)
                    {
                        GROW(type_new,natms_new,natms_new+1);
                        GROW(s_new,3*natms_new,3*natms_new+3);
                        
                        type_new[natms_new]=type[iatm];
                        s_new[natms_new*3]=b[0];
                        s_new[natms_new*3+1]=b[1];
                        s_new[natms_new*3+2]=b[2];
                        
                        if(dof_xst)
                        {
                            GROW(dof_new,3*natms_new,3*natms_new+3);
                            dof_new[natms_new*3]=dof[icomp];
                            dof_new[natms_new*3+1]=dof[icomp+1];
                            dof_new[natms_new*3+2]=dof[icomp+2];
                        }
                        
                        natms_new++;
                        
                    }
                    
                    
                }
            }
        }
        
        icomp+=3;
    }
    
    
    for(int i=0;i<3;i++)
    {
        for(int j=0;j<3;j++)
        {
            H_x[i][j]=0.0;
            for(int k=0;k<3;k++)
                H_x[i][j]+=u[i][k]*H[k][j];
        }
    }
    
    XMath* xmath=new XMath(xtal);
    xmath->square2lo_tri(H_x,H_new);
    delete xmath;
    
    for(int i=0;i<3;i++)
    {
        delete [] H[i];
        delete [] H_x[i];
        delete [] n[i];
    }
    
    delete [] H;
    delete [] H_x;
    delete [] n;
    
    delete [] b;
    delete [] s_tmp;
    delete [] sq;
    delete [] lo_bound;
    delete [] hi_bound;
    
    if(natms)
    {
        delete [] type;
        delete [] s;
        if(dof_xst)
            delete [] dof;
    }
    

    
    H=H_new;
    s=s_new;
    type=type_new;
    natms=natms_new;
    if(dof_xst)
        dof_new=dof;

    
    M3INV_TRI_LOWER(H,B);
    
    for(int i=0;i<no_types;i++)
        type_count[i]=0;
    for(int i=0;i<natms;i++)
        type_count[type[i]]++;

}
/*--------------------------------------------
 add vaccuum at the top of the box
 --------------------------------------------*/
void Box::id_correction()
{
    // first test to see if there is a mess
    // if there is then do the sort
    int chk=1;
    int iatm=0;
    int lo_cnt=0;
    int hi_cnt;
    int itype=0;
    while (chk && itype<no_types)
    {
        hi_cnt=lo_cnt+type_count[itype];
        while(iatm<hi_cnt && chk)
        {
            if(type[iatm]!=itype)
                chk=0;
            iatm++;
        }
        lo_cnt+=type_count[itype];
        itype++;
    }
    if(chk)
        return;
    

    int* new_type;
    type0* new_s;
    char* new_dof=NULL;
    CREATE1D(new_type,natms);
    CREATE1D(new_s,natms*3);
    if(dof_xst)
        CREATE1D(new_dof,natms*3);
    
    int icurs=0;
    for(int itype=0;itype<no_types;itype++)
    {
        for(int iatm=0;iatm<natms;iatm++)
        {
            if(type[iatm]==itype)
            {
                new_type[icurs]=iatm;
                memcpy(&new_s[icurs*3],&s[iatm*3],3*sizeof(type0));
                if(dof_xst)
                    memcpy(&new_dof[icurs*3],&dof[iatm*3],3*sizeof(char));
                
                new_type[icurs]=itype;
                icurs++;
            }
        }
    }
    
    if(natms)
    {
        delete [] type;
        delete [] s;
        if(dof_xst)
            delete [] dof;
    }
    
    type=new_type;
    s=new_s;
    if(dof_xst)
        dof=new_dof;
}
/*--------------------------------------------
 add vaccuum at the top of the box
 --------------------------------------------*/
void Box::del_type(int itype)
{
    char** new_atom_names;
    type0* new_mass;
    int* new_type_count;
    
    CREATE1D(new_atom_names,no_types-1);
    CREATE1D(new_mass,no_types-1);
    CREATE1D(new_type_count,no_types-1);
    for(int i=0;i<itype;i++)
    {
        new_atom_names[i]=atom_names[i];
        new_mass[i]=mass[i];
        new_type_count[i]=type_count[i];
        
    }
    delete [] atom_names[itype];
    
    for(int i=itype+1;i<no_types;i++)
    {
        new_atom_names[i-1]=atom_names[i];
        new_mass[i-1]=mass[i];
        new_type_count[i-1]=type_count[i];
    }
    if(no_types)
    {
        delete [] atom_names;
        delete [] type_count;
        delete [] mass;
    }
    atom_names=new_atom_names;
    mass=new_mass;
    type_count=new_type_count;
    
    
    for(int i=0;i<natms;i++)
        if(type[i]>itype)
            type[i]--;
    no_types--;
}

/*--------------------------------------------
 add vaccuum at the top of the box
 --------------------------------------------*/
void Box::rm_frac(int dim,type0 s_lo,type0 s_hi)
{
    type0 ds=s_hi-s_lo;
    
    int list_sz=0;
    int* list=NULL;
    
    for(int i=dim;i<natms*3;i+=3)
    {
        if(s[i]<s_lo)
        {
            s[i]/=1.0-ds;
        }
        else if(s[i]>=s_hi)
        {
            s[i]-=ds;
            s[i]/=1.0-ds;
        }
        else
        {
            GROW(list,list_sz,list_sz+1);
            list[list_sz++]=i/3;
        }
    }
    
    
    
    if(list_sz)
    {
        del_atoms(list_sz,list);
        delete [] list;
        list_sz=0;
    }
    
    for(int i=0;i<3;i++)
        H[dim][i]*=1.0-ds;
    
    M3INV_TRI_LOWER(H,B);
}


/*--------------------------------------------
 add image at a specific direction
 --------------------------------------------*/
void Box::add_image(int idir,type0 s_cut)
{
    int s_size=(nghosts+natms)*3;
    int type_size=nghosts+natms;
    
    Arr<type0> _s_(s,s_size,3*type_size/10);
    Arr<int> _type_(type,type_size,type_size/10);
    
    type0 s_tmp[3];
    int type_tmp;
    for(int i=0;i<natms;i++)
    {
        type_tmp=type[i];
        
        s_tmp[0]=s[3*i];
        s_tmp[1]=s[3*i+1];
        s_tmp[2]=s[3*i+2];
        
        s_tmp[idir]++;
        while(s_tmp[idir]<s_cut+1.0)
        {
            _s_(s_tmp);
            _type_(type_tmp);
            s_tmp[idir]++;
        }
        
        
        s_tmp[0]=s[3*i];
        s_tmp[1]=s[3*i+1];
        s_tmp[2]=s[3*i+2];
        
        s_tmp[idir]--;
        while(s_tmp[idir]>=-s_cut)
        {
            _s_(s_tmp);
            _type_(type_tmp);
            s_tmp[idir]--;
        }
        
    }
    
    nghosts=type_size-natms;
}
/*--------------------------------------------
 add image at a specific direction
 --------------------------------------------*/
void Box::add_ghost(bool (&dirs)[3],type0 (&s_cut)[3] )
{
    for(int idim=0;idim<3;idim++)
    {
        if(!dirs[idim]) continue;
        
        add_image(idim,s_cut[idim]);
    }
}
/*--------------------------------------------
 add image at a specific direction
 --------------------------------------------*/
void Box::del_ghost()
{
    if(nghosts==0)
        return;
    
    type0* s_=new type0[3*natms];
    int* type_=new int[natms];
    memcpy(s_,s,3*natms*sizeof(type0));
    memcpy(type_,type,natms*sizeof(int));
    nghosts=0;
}
























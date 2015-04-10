#include <math.h>
#include "cfg2box.h"
#include "memory.h"
#include "error.h"
#include "box_collection.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
CFG2Box::CFG2Box(Xtal* xtal,char* box,char* name):InitPtrs(xtal)
{
    ibox=box_collection->find(box);
    
    basic_length=1.0;
    xtal->error_flag=0;
    tmp_natms=0;
    file_name=name;
    cfgfile=fopen(file_name,"r");
    if(cfgfile==NULL)
    {
        error->warning("%s file not found",file_name);
        xtal->error_flag=-1;
        return;
    }
    
    alloc();
    
    CREATE1D(line,MAX_CHAR);
    
    M3ZERO(H0);
    M3ZERO(eta);
    M3ZERO(trns);
    trns[0][0]=trns[1][1]=trns[2][2]=1.0;
    H0[0][0]=H0[1][1]=H0[2][2]=1.0;
    
    
    fpos_t pos;
    header_cmplt=0;
    while (!header_cmplt)
    {
        
        fgetpos(cfgfile,&pos);
        fgets(line,MAX_CHAR,cfgfile);
        read_header();
        if(xtal->error_flag==-1)
        {
            dealloc();
            delete [] line;
            fclose(cfgfile);
            return;
        }
        
    }
    fsetpos(cfgfile,&pos);
    set_box();
    
    dealloc();
    if(xtal->error_flag==-1)
    {
        delete [] line;
        fclose(cfgfile);
        return;
    }
    
    
    
    CREATE1D(s_buff,3*tmp_natms);
    CREATE1D(type_buff,tmp_natms);
    atom_cmplt=0;
    curr_id=0;
    while (!atom_cmplt)
    {
        read_atom();
        if(xtal->error_flag==-1)
        {
            delete [] s_buff;
            delete [] type_buff;
            delete [] line;
            fclose(cfgfile);
            return;
        }
        if(curr_id==tmp_natms)
            atom_cmplt=1;
    }
    
    if(curr_id<tmp_natms)
    {
        xtal->error_flag=-1;
        delete [] s_buff;
        delete [] type_buff;
        delete [] line;
        fclose(cfgfile);
        return;
    }
    
    
    
    

    box_collection->boxes[ibox]->add_atoms(tmp_natms,type_buff,s_buff);
    
    delete [] s_buff;
    delete [] type_buff;
    delete [] line;
    fclose(cfgfile);
    
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
CFG2Box::~CFG2Box()
{
}
/*--------------------------------------------
 allocate the matrices and vectors
 --------------------------------------------*/
void CFG2Box::alloc()
{
    CREATE2D(H_x,3,3);
    CREATE2D(Ht,3,3);
    CREATE1D(sq,3);
    CREATE1D(b,3);
    CREATE2D(H0,3,3);
    CREATE2D(eta,3,3);
    CREATE2D(eta_sq,3,3);
    CREATE2D(trns,3,3);
    
}
/*--------------------------------------------
 deallocate the matrices and vectors
 --------------------------------------------*/
void CFG2Box::dealloc()
{
    for(int i=0;i<3;i++)
    {
        delete [] Ht[i];
        delete [] H_x[i];
        delete [] eta_sq[i];
        delete [] eta[i];
        delete [] H0[i];
        delete [] trns[i];
    }
    delete [] H_x;
    delete [] Ht;
    delete [] eta_sq;
    delete [] eta;
    delete [] H0;
    delete [] trns;
    delete [] b;
    delete [] sq;
}
/*--------------------------------------------
 read the header of the file
 --------------------------------------------*/
void CFG2Box::read_header()
{
    char* command;
    int narg = xtal->hash_remover(line,command);
    if(narg==0)
        return;
    type0 tmp;
    int icmp,jcmp,tmpno;
    char* strtmp1;
    char* strtmp2;
    
    
    CREATE1D(strtmp1,MAX_CHAR);
    CREATE1D(strtmp2,MAX_CHAR);
    
        
    if(strcmp(command,".NO_VELOCITY.")==0)
    {
        vel_chk=0;
        ext_cfg=1;
        entry_count-=3;
    }
    else if(sscanf(command,"Transform(%d,%d) = %lf",&icmp,&jcmp,&tmp)==3)
    {
        icmp--;
        jcmp--;
        if (icmp>2 || icmp<0)
        {
            error->warning("wrong component in %s file for Transform(%d,%d)",file_name,icmp+1,jcmp+1);
            if(narg)
                delete [] command;
            delete [] strtmp2;
            delete [] strtmp1;
            xtal->error_flag=-1;
            return;
        }
        if(jcmp>2 || jcmp<0)
        {
            error->warning("wrong component in %s file for Transform(%d,%d)",file_name,icmp+1,jcmp+1);
            if(narg)
                delete [] command;
            delete [] strtmp2;
            delete [] strtmp1;
            xtal->error_flag=-1;
            return;
        }
        trns[icmp][jcmp]=tmp;
    }
    else if(sscanf(command,"eta(%d,%d) = %lf",&icmp,&jcmp,&tmp)==3)
    {
        icmp--;
        jcmp--;
        if (icmp>2 || icmp<0)
        {
            error->warning("wrong component in %s file for eta(%d,%d)",file_name,icmp+1,jcmp+1);
            if(narg)
                delete [] command;
            delete [] strtmp2;
            delete [] strtmp1;
            xtal->error_flag=-1;
            return;
        }
        if(jcmp>2 || jcmp<0)
        {
            error->warning("wrong component in %s file for eta(%d,%d)",file_name,icmp+1,jcmp+1);
            if(narg)
                delete [] command;
            delete [] strtmp2;
            delete [] strtmp1;
            xtal->error_flag=-1;
            return;
        }
        eta[icmp][jcmp]=tmp;
    }
    else if(sscanf(command,"entry_count = %d",&tmpno)==1)
    {
        entry_count=tmpno;
        ext_cfg=1;
        int mincomp=3+(3*vel_chk);
        if (entry_count < mincomp)
        {
            error->warning("entry_count in %s should at least be equal to %d",file_name,mincomp);
            if(narg)
                delete [] command;
            delete [] strtmp2;
            delete [] strtmp1;
            xtal->error_flag=-1;
            return;
        }
    }
    else if(sscanf(command,"H0(%d,%d) = %lf A",&icmp,&jcmp,&tmp)==3)
    {
        icmp--;
        jcmp--;
        if (icmp>2 || icmp<0)
        {
            error->warning("wrong component in %s file for H(%d,%d)",file_name,icmp+1,jcmp+1);
            if(narg)
                delete [] command;
            delete [] strtmp2;
            delete [] strtmp1;
            xtal->error_flag=-1;
            return;
        }
        if(jcmp>2 || jcmp<0)
        {
            error->warning("wrong component in %s file for H(%d,%d)",file_name,icmp+1,jcmp+1);
            if(narg)
                delete [] command;
            delete [] strtmp2;
            delete [] strtmp1;
            xtal->error_flag=-1;
            return;
        }
        H0[icmp][jcmp]=tmp;
    }
    else if(sscanf(command,"R = %lf %s",&tmp,strtmp1)==2)
    {
    }
    else if(sscanf(command,"Number of particles = %d",&tmpno)==1)
    {
        tmp_natms=tmpno;
        if(tmp_natms<=0)
        {
            error->warning("Number of particles in %s file should be greater than 0",file_name);
            if(narg)
                delete [] command;
            delete [] strtmp2;
            delete [] strtmp1;
            xtal->error_flag=-1;
            return;
        }
    }
    else if(sscanf(command,"auxiliary[%d] = %s %s",&icmp,strtmp1,strtmp2)==3)
    {
        int mincomp=3+(3*vel_chk);
        if(icmp+mincomp+1>entry_count)
        {
            error->warning("wrong component in %s file for auxiliary[%d], %d+%d+1 > entry_count",file_name,icmp,mincomp,icmp);
            if(narg)
                delete [] command;
            delete [] strtmp2;
            delete [] strtmp1;
            xtal->error_flag=-1;
            return;
        }
    }
    else if(sscanf(command,"A = %lf Angstrom (basic length-scale)",&tmp)==1)
    {
        if(tmp<=0.0)
        {
            error->warning("A in %s file should be greater than 0.0",file_name);
            if(narg)
                delete [] command;
            delete [] strtmp2;
            delete [] strtmp1;
            xtal->error_flag=-1;
            return;
        }
        basic_length=tmp;
    }
    else
    {
        if (narg==8&&ext_cfg==0)
            header_cmplt=1;
        else if (narg==1&&ext_cfg)
            header_cmplt=1;
        else
        {
            error->warning("invalid line in %s file: %s",file_name,command);
            if(narg)
                delete [] command;
            delete [] strtmp2;
            delete [] strtmp1;
            xtal->error_flag=-1;
            return;
        }
    }
    
    if(narg)
        delete [] command;
    delete [] strtmp2;
    delete [] strtmp1;
    return;
}
/*--------------------------------------------
 calculates H from H0, Transform, and eta;
 ** make sure H is zeroed before;
 for now we disregard eta;
 remeber to fix it later;
 --------------------------------------------*/
void CFG2Box::set_box()
{
    
    
    type0 babs;
    
    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++)
        {
            H_x[i][j]=0.0;
            for (int k=0;k<3;k++)
                H_x[i][j]+=H0[i][k]*trns[k][j];
        }
    
    int chk=1;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            if(eta[i][j]!=0.0)
                chk=0;
    if(chk==0)
    {
        for(int i=0;i<3;i++)
            for(int j=0;j<3;j++)
                eta[i][j]*=2.0;
        for(int i=0;i<3;i++)
            eta[i][i]++;
        
        M3sqroot(eta,eta_sq);
        
        if(xtal->error_flag==-1)
            return;
        
        M3EQV(H_x,H0);
        M3ZERO(H_x);
        for (int i=0;i<3;i++)
            for (int j=0;j<3;j++)
                for (int k=0;k<3;k++)
                    H_x[i][j]+=H0[i][k]*eta_sq[k][j];
    }

    
    
    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++)
        {
            H_x[i][j]*=basic_length;
            box_collection->boxes[ibox]->H[i][j]=H_x[i][j];
        }
    
    if (M3DET(H_x)==0.0)
    {
        error->warning("determinant of H in %s file is 0.0",file_name);
        xtal->error_flag=-1;
        return;
    }
    type0 det;
    
    for (int i=0;i<3;i++)
    {
        sq[i]=0.0;
        for (int j=0;j<3;j++)
        {
            sq[i]+=H_x[i][j]*H_x[i][j];
            Ht[i][j]=0.0;
        }
    }
    
    Ht[0][0]=sqrt(sq[0]);
    
    for (int i=0;i<3;i++)
        Ht[1][0]+=H_x[0][i]*H_x[1][i];
    Ht[1][0]/=Ht[0][0];
    
    Ht[1][1]=sqrt(sq[1]-Ht[1][0]*Ht[1][0]);
    
    for (int i=0;i<3;i++)
        Ht[2][0]+=H_x[0][i]*H_x[2][i];
    
    Ht[2][0]/=Ht[0][0];
    
    
    b[0]=H_x[0][1]*H_x[1][2]-H_x[0][2]*H_x[1][1];
    b[1]=H_x[0][2]*H_x[1][0]-H_x[0][0]*H_x[1][2];
    b[2]=H_x[0][0]*H_x[1][1]-H_x[0][1]*H_x[1][0];
    babs=sqrt(b[0]*b[0]+b[1]*b[1]+b[2]*b[2]);
    b[0]/=babs;
    b[1]/=babs;
    b[2]/=babs;
    
    for (int i=0;i<3;i++)
        Ht[2][2]+=H_x[2][i]*b[i];
    Ht[2][1]=sqrt(sq[2]-Ht[2][2]*Ht[2][2]-Ht[2][0]*Ht[2][0]);
    
    M3EQV(Ht,box_collection->boxes[ibox]->H);
    M3EQV(Ht,H_x);
    M3INV(box_collection->boxes[ibox]->H,box_collection->boxes[ibox]->B,det);
    
    
}
/*--------------------------------------------
 calculates square root of 3x3 matrix
 ref: L. P. Franca
 An Algorithm to Compute The Square Root of
 a 3x3 Positive Definite Matrix
 Computers Math. Applic. Vol. 18, No. 5,
 pp. 459-466, 1989
 --------------------------------------------*/
void CFG2Box::M3sqroot(type0** A,type0** Asq)
{
    type0 IA=0;
    for(int i=0;i<3;i++)
        IA+=A[i][i];
    type0 IIA=0;
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            IIA-=A[i][j]*A[j][i];
    
    IIA+=IA*IA;
    IIA*=0.5;
    type0 IIIA=M3DET(A);
    type0 k=IA*IA-3*IIA;
    if(k<0.0)
    {
        error->warning("eta in %s should be positive definite",file_name);
        xtal->error_flag=-1;
        return;
    }
    if(k<TOLERANCE)
    {
        if(IA<=0.0)
        {
            error->warning("eta in %s should be positive definite",file_name);
            xtal->error_flag=-1;
            return;
        }
        M3ZERO(Asq);
        for(int i=0;i<3;i++)
            Asq[i][i]=sqrt(IA/3.0);
        return;
    }
    
    type0 l=IA*(IA*IA -4.5*IIA)+13.5*IIIA;
    
    type0 temp=l/(k*sqrt(k));
    if(temp>1.0||temp<-1.0)
    {
        error->warning("eta in %s should be positive definite",file_name);
        xtal->error_flag=-1;
        return;
    }
    
    type0 phi=acos(temp);
    type0 lambda=sqrt((1.0/3.0)*(IA+2*sqrt(k)*cos(phi/3.0)));
    
    type0 IIIAsq=sqrt(IIIA);
    type0 y=-lambda*lambda+IA+2*IIIAsq/lambda;
    if(y<0.0)
    {
        error->warning("eta in %s should be positive definite",file_name);
        xtal->error_flag=-1;
        return;
    }
    type0 IAsq=lambda+sqrt(y);
    type0 IIAsq=0.5*(IAsq*IAsq-IA);
    
    type0 coef0=IAsq*IIAsq-IIIAsq;
    if(coef0==0)
    {
        error->warning("eta in %s should be positive definite",file_name);
        xtal->error_flag=-1;
        return;
    }
    coef0=1.0/coef0;
    
    M3ZERO(Asq);
    
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            for(int k=0;k<3;k++)
                Asq[i][j]-=coef0*A[i][k]*A[k][j];
    
    type0 coef1=coef0*(IAsq*IAsq-IIAsq);
    for(int i=0;i<3;i++)
        for(int j=0;j<3;j++)
            Asq[i][j]+=coef1*A[i][j];
    
    type0 coef2=coef0*IAsq*IIIAsq;
    for(int i=0;i<3;i++)
        Asq[i][i]+=coef2;
    
}

/*--------------------------------------------
 reads the atom section of the cfg file
 --------------------------------------------*/
void CFG2Box::read_atom()
{
    fgets(line,MAX_CHAR,cfgfile);
    char** arg;
    type0 mass;
    int narg=xtal->parse_line(line,arg);
    
    if(feof(cfgfile))
        atom_cmplt=1;
    
    if(atom_cmplt)
    {
        if(narg)
        {
            for(int i=0;i<narg;i++)
                delete [] arg[i];
            delete [] arg;
        }
        return;
    }
    
    if (narg!=8 && ext_cfg==0)
    {
        error->warning("invalid line in %s file: %s",file_name,line);
        if(narg)
        {
            for(int i=0;i<narg;i++)
                delete [] arg[i];
            delete [] arg;
        }
        xtal->error_flag=-1;
        return;
    }
    if (ext_cfg && !(narg==1 || narg==entry_count))
    {
        error->warning("invalid line in %s file: %s",file_name,line);
        if(narg)
        {
            for(int i=0;i<narg;i++)
                delete [] arg[i];
            delete [] arg;
        }
        xtal->error_flag=-1;
        return;
    }
    
    if(ext_cfg)
    {
        if(narg==1)
        {
            mass=static_cast<type0>(atof(arg[0]));
            
            if(narg)
            {
                for(int i=0;i<narg;i++)
                    delete [] arg[i];
                delete [] arg;
            }
            fgets(line,MAX_CHAR,cfgfile);
            narg=xtal->parse_line(line,arg);
            if(narg!=1)
            {
                error->warning("invalid line in %s file: %s",file_name,line);
                if(narg)
                {
                    for(int i=0;i<narg;i++)
                        delete [] arg[i];
                    delete [] arg;
                }
                xtal->error_flag=-1;
                return;
            }
            if(mass<=0.0)
            {
                error->warning("mass of %s %s file (%lf) should be greater than 0.0",arg[0],file_name,line,mass);
                if(narg)
                {
                    for(int i=0;i<narg;i++)
                        delete [] arg[i];
                    delete [] arg;
                }
                xtal->error_flag=-1;
                return;
            }
            
            last_type=box_collection->boxes[ibox]->add_type(mass,arg[0]);
        }
        else if(narg==entry_count)
        {
            
            type_buff[curr_id]=last_type;
            s_buff[3*curr_id]=atof(arg[0]);
            s_buff[3*curr_id+1]=atof(arg[1]);
            s_buff[3*curr_id+2]=atof(arg[2]);
            
            curr_id++;
        }
        else
        {
            error->warning("invalid line in %s file: %s",file_name,line);
            if(narg)
            {
                for(int i=0;i<narg;i++)
                    delete [] arg[i];
                delete [] arg;
            }
            xtal->error_flag=-1;
            return;
        }
    }
    else
    {
        if(narg==8)
        {
            mass=static_cast<type0>(atof(arg[0]));
            last_type=box_collection->boxes[ibox]->add_type(mass,arg[1]);
            if(mass<=0.0)
            {
                error->warning("mass of %s %s file (%lf) should be greater than 0.0",arg[1],file_name,line,mass);
                if(narg)
                {
                    for(int i=0;i<narg;i++)
                        delete [] arg[i];
                    delete [] arg;
                }
                xtal->error_flag=-1;
                return;
            }
            
            type_buff[curr_id]=last_type;
            s_buff[3*curr_id]=atof(arg[2]);
            s_buff[3*curr_id+1]=atof(arg[3]);
            s_buff[3*curr_id+2]=atof(arg[4]);
            
            curr_id++;
        }
        else
        {
            error->warning("invalid line in %s file: %s",file_name,line);
            if(narg)
            {
                for(int i=0;i<narg;i++)
                    delete [] arg[i];
                delete [] arg;
            }
            xtal->error_flag=-1;
            return;
        }
    }
    
    
    for(int i=0;i<narg;i++)
        delete [] arg[i];
    if(narg)
        delete [] arg;
}

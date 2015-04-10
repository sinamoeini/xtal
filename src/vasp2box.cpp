#include "box_collection.h"
#include "error.h"
#include "memory.h"
#include "xmath.h"
#include "vasp2box.h"
#include <cmath>
/*--------------------------------------------
 constructor
 --------------------------------------------*/
VASP2Box::VASP2Box(Xtal* xtal,char* box_name,char* name):InitPtrs(xtal)
{
    FILE* poscar;
    type0 s0,s1,s2;
    char dof0,dof1,dof2;
    char** arg;
    Box* box;
    int narg;
    type0 mass,det;
    int ibox;
    int iatm,s_x=1;
    
    natms_tmp=0;
    
    file_name=name;
    ibox=box_collection->find(box_name);
    
    
    basic_length=1.0;

    poscar=fopen(file_name,"r");
    if(poscar==NULL)
    {
        error->warning("%s file not found",file_name);
        xtal->error_flag=-1;
        return;
    }
    
    alloc();

    
    for(int i=0;i<2;i++)
        if(read_line(poscar))
            return;
    
    if(sscanf(line,"%lf",&basic_length)!=1)
    {
        error->warning("wrong format in file %s: %s",file_name,line);
        dealloc();
        fclose(poscar);
        xtal->error_flag=-1;
        return;
    }
    
    for (int i=0;i<3;i++)
    {
        if(read_line(poscar))
            return;
        
        if(sscanf(line,"%lf %lf %lf",&H0[i][0],&H0[i][1],&H0[i][2])!=3)
        {
            error->warning("wrong format in file %s: %s",file_name,line);
            dealloc();
            fclose(poscar);
            xtal->error_flag=-1;
            return;
        }
    }
    
    det=M3DET(H0);
    if(det<=0.0)
    {
        error->warning("determinant of H is should be greater than 0.0");
        dealloc();
        fclose(poscar);
        xtal->error_flag=-1;
        return;
    }
    
    if(basic_length<0.0)
        basic_length=pow(-basic_length/det,1.0/3.0);
    
    for (int i=0;i<3;i++)
        for (int j=0;j<3;j++)
            H0[i][j]*=basic_length;

    M3INV(H0,B0,det);

    box=box_collection->boxes[ibox];

    
    if(read_line(poscar))
        return;
    narg=xtal->parse_line(line,arg);
    
    for(int iarg=0;iarg<narg;iarg++)
    {
        mass=find_mass(arg[iarg]);
        
        if(mass==-1.0)
        {
            error->warning("element %s in file %s was not found in the periodic table",arg[iarg],file_name);
            
            for(int i=0;i<narg;i++)
                delete [] arg[i];
            delete [] arg;
            dealloc();
            fclose(poscar);
            xtal->error_flag=-1;
            return;
        }
        
        box->add_type(mass,arg[iarg]);
    }
    
    for(int i=0;i<narg;i++)
        delete [] arg[i];
    if(narg)
        delete [] arg;
    
    
    if(read_line(poscar))
        return;
    narg=xtal->parse_line(line,arg);
    
    if(box->no_types!=narg)
    {
        error->warning("the count of all the types are not provided in file %s: %s",file_name,line);
        
        for(int i=0;i<narg;i++)
            delete [] arg[i];
        if(narg)
            delete [] arg;
        
        dealloc();
        fclose(poscar);
        xtal->error_flag=-1;
        return;
    }
    
    for(int i=0;i<narg;i++)
        box->type_count[i]=atoi(arg[i]);
    
    for(int i=0;i<narg;i++)
        delete [] arg[i];
    if(narg)
        delete [] arg;
    
    
  
    dof_exist=0;
    if(read_line(poscar))
        return;
    
    if(strcmp(line,"Selective dynamics\n")==0)
    {
        dof_exist=1;
        if(read_line(poscar))
            return;
    }

    if(strcmp(line,"Direct\n")==0)
        s_x=1;
    else if(strcmp(line,"Cartesian\n")==0)
        s_x=0;
    else
    {
        error->warning("wrong format in file %s: %s",file_name,line);
        dealloc();
        fclose(poscar);
        xtal->error_flag=-1;
        return;
    }
    
    for(int i=0;i<box->no_types;i++)
        natms_tmp+=box->type_count[i];
    
    
    CREATE1D(s_tmp,3*natms_tmp);
    CREATE1D(type_tmp,natms_tmp);
    if(dof_exist)
        CREATE1D(dof_tmp,3*natms_tmp);

    box->dof_xst=dof_exist;


    
    iatm=0;
    for(int itype=0;itype<box->no_types;itype++)
        for(int iiatm=0;iiatm<box->type_count[itype];iiatm++)
        {
            
            if(read_line(poscar))
                return;
            
            if(dof_exist)
            {
                if(sscanf(line,"%lf %lf %lf %c %c %c",&s0,&s1,&s2,&dof0,&dof1,&dof2)!=6)
                {
                    error->warning("wrong format in file %s: %s",file_name,line);
                    dealloc();
                    fclose(poscar);
                    xtal->error_flag=-1;
                    return;
                }

                dof_tmp[iatm*3]=dof0;
                dof_tmp[iatm*3+1]=dof1;
                dof_tmp[iatm*3+2]=dof2;
            }
            else
            {
                if(sscanf(line,"%lf %lf %lf",&s0,&s1,&s2)!=3)
                {
                    error->warning("wrong format in file %s: %s",file_name,line);
                    dealloc();
                    fclose(poscar);
                    xtal->error_flag=-1;
                    return;
                }
            }

            type_tmp[iatm]=itype;
            
            if(s_x)
            {
                s_tmp[iatm*3]=s0;
                s_tmp[iatm*3+1]=s1;
                s_tmp[iatm*3+2]=s2;
            }
            else
            {
                s_tmp[iatm*3]=s0*B0[0][0]+s1*B0[1][0]+s2*B0[2][0];
                s_tmp[iatm*3+1]=s0*B0[0][1]+s1*B0[1][1]+s2*B0[2][1];
                s_tmp[iatm*3+2]=s0*B0[0][2]+s1*B0[1][2]+s2*B0[2][2];
            }
            
            iatm++;
        }
    
    if(dof_exist)
    {
        for(int iiatm=0;iiatm<3*natms_tmp;iiatm++)
        {
            if(dof_tmp[iiatm]=='T')
                dof_tmp[iiatm]='0';
            else if(dof_tmp[iiatm]=='F')
                dof_tmp[iiatm]='1';
            else
            {
                error->warning("wrong format in file %s: %s",file_name,line);
                dealloc();
                fclose(poscar);
                xtal->error_flag=-1;
                return;
            }
        }
    }
    

    
    
    for(int itype=0;itype<box->no_types;itype++)
        box->type_count[itype]=0;
    
    if(dof_exist)
        box_collection->boxes[ibox]->add_atoms(natms_tmp,type_tmp,s_tmp,dof_tmp);
    else
        box_collection->boxes[ibox]->add_atoms(natms_tmp,type_tmp,s_tmp);
    
    
    XMath* xmath=new XMath(xtal);
    xmath->square2lo_tri(H0,box->H);
    M3INV_TRI_LOWER(box->H,box->B);
    delete xmath;
    
    
    dealloc();
    fclose(poscar);
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
VASP2Box::~VASP2Box()
{
}
/*--------------------------------------------
 read line
 --------------------------------------------*/
int VASP2Box::read_line(FILE* fp)
{
    if(feof(fp))
    {
        error->warning("%s file ended immaturely",file_name);
        dealloc();
        fclose(fp);
        xtal->error_flag=-1;
        return 1;
    }
    
    fgets(line,MAX_CHAR,fp);
    return 0;
}
/*--------------------------------------------
 alloc
 --------------------------------------------*/
void VASP2Box::alloc()
{
    CREATE1D(line,MAX_CHAR);
    CREATE2D(H0,3,3);
    CREATE2D(B0,3,3);
}
/*--------------------------------------------
 dealloc
 --------------------------------------------*/
void VASP2Box::dealloc()
{
    delete [] line;
    for(int i=0;i<3;i++)
    {
        delete [] H0[i];
        delete [] B0[i];
    }
    delete [] H0;
    delete [] B0;
    
    if (natms_tmp)
    {
        delete [] s_tmp;
        delete [] type_tmp;
        
        if(dof_exist)
            delete [] dof_tmp;
    }
    
}
/*--------------------------------------------
 find mass
 --------------------------------------------*/
type0 VASP2Box::find_mass(char* atm_name)
{
    if(strcmp(atm_name,"H")==0)
        return 1.007900;
    else if(strcmp(atm_name,"He")==0)
        return 4.002600;
    else if(strcmp(atm_name,"Li")==0)
        return 6.941000;
    else if(strcmp(atm_name,"Be")==0)
        return 9.012200;
    else if(strcmp(atm_name,"B")==0)
        return 10.811000;
    else if(strcmp(atm_name,"C")==0)
        return 12.010700;
    else if(strcmp(atm_name,"N")==0)
        return 14.006700;
    else if(strcmp(atm_name,"O")==0)
        return 15.999400;
    else if(strcmp(atm_name,"F")==0)
        return 18.998400;
    else if(strcmp(atm_name,"Ne")==0)
        return 20.179700;
    else if(strcmp(atm_name,"Na")==0)
        return 22.989700;
    else if(strcmp(atm_name,"Mg")==0)
        return 24.305000;
    else if(strcmp(atm_name,"Al")==0)
        return 26.981500;
    else if(strcmp(atm_name,"Si")==0)
        return 28.085500;
    else if(strcmp(atm_name,"P")==0)
        return 30.973800;
    else if(strcmp(atm_name,"S")==0)
        return 32.065000;
    else if(strcmp(atm_name,"Cl")==0)
        return 35.453000;
    else if(strcmp(atm_name,"Ar")==0)
        return 39.948000;
    else if(strcmp(atm_name,"K")==0)
        return 39.098300;
    else if(strcmp(atm_name,"Ca")==0)
        return 40.078000;
    else if(strcmp(atm_name,"Sc")==0)
        return 44.955900;
    else if(strcmp(atm_name,"Ti")==0)
        return 47.867000;
    else if(strcmp(atm_name,"V")==0)
        return 50.941500;
    else if(strcmp(atm_name,"Cr")==0)
        return 51.996100;
    else if(strcmp(atm_name,"Mn")==0)
        return 54.938000;
    else if(strcmp(atm_name,"Fe")==0)
        return 55.845000;
    else if(strcmp(atm_name,"Co")==0)
        return 58.933200;
    else if(strcmp(atm_name,"Ni")==0)
        return 58.693400;
    else if(strcmp(atm_name,"Cu")==0)
        return 63.546000;
    else if(strcmp(atm_name,"Zn")==0)
        return 65.390000;
    else if(strcmp(atm_name,"Ga")==0)
        return 69.723000;
    else if(strcmp(atm_name,"Ge")==0)
        return 72.640000;
    else if(strcmp(atm_name,"As")==0)
        return 74.921600;
    else if(strcmp(atm_name,"Se")==0)
        return 78.960000;
    else if(strcmp(atm_name,"Br")==0)
        return 79.904000;
    else if(strcmp(atm_name,"Kr")==0)
        return 83.800000;
    else if(strcmp(atm_name,"Rb")==0)
        return 85.467800;
    else if(strcmp(atm_name,"Sr")==0)
        return 87.620000;
    else if(strcmp(atm_name,"Y")==0)
        return 88.905900;
    else if(strcmp(atm_name,"Zr")==0)
        return 91.224000;
    else if(strcmp(atm_name,"Nb")==0)
        return 92.906400;
    else if(strcmp(atm_name,"Mo")==0)
        return 95.940000;
    else if(strcmp(atm_name,"Tc")==0)
        return 98.000000;
    else if(strcmp(atm_name,"Ru")==0)
        return 101.070000;
    else if(strcmp(atm_name,"Rh")==0)
        return 102.905500;
    else if(strcmp(atm_name,"Pd")==0)
        return 106.420000;
    else if(strcmp(atm_name,"Ag")==0)
        return 107.868200;
    else if(strcmp(atm_name,"Cd")==0)
        return 112.411000;
    else if(strcmp(atm_name,"In")==0)
        return 114.818000;
    else if(strcmp(atm_name,"Sn")==0)
        return 118.710000;
    else if(strcmp(atm_name,"Sb")==0)
        return 121.760000;
    else if(strcmp(atm_name,"Te")==0)
        return 127.600000;
    else if(strcmp(atm_name,"I")==0)
        return 126.904500;
    else if(strcmp(atm_name,"Xe")==0)
        return 131.293000;
    else if(strcmp(atm_name,"Cs")==0)
        return 132.905500;
    else if(strcmp(atm_name,"Ba")==0)
        return 137.327000;
    else if(strcmp(atm_name,"La")==0)
        return 138.905500;
    else if(strcmp(atm_name,"Ce")==0)
        return 140.116000;
    else if(strcmp(atm_name,"Pr")==0)
        return 140.907700;
    else if(strcmp(atm_name,"Nd")==0)
        return 144.240000;
    else if(strcmp(atm_name,"Pm")==0)
        return 145.000000;
    else if(strcmp(atm_name,"Sm")==0)
        return 150.360000;
    else if(strcmp(atm_name,"Eu")==0)
        return 151.964000;
    else if(strcmp(atm_name,"Gd")==0)
        return 157.250000;
    else if(strcmp(atm_name,"Tb")==0)
        return 158.925300;
    else if(strcmp(atm_name,"Dy")==0)
        return 162.500000;
    else if(strcmp(atm_name,"Ho")==0)
        return 164.930300;
    else if(strcmp(atm_name,"Er")==0)
        return 167.259000;
    else if(strcmp(atm_name,"Tm")==0)
        return 168.934200;
    else if(strcmp(atm_name,"Yb")==0)
        return 173.040000;
    else if(strcmp(atm_name,"Lu")==0)
        return 174.967000;
    else if(strcmp(atm_name,"Hf")==0)
        return 178.490000;
    else if(strcmp(atm_name,"Ta")==0)
        return 180.947900;
    else if(strcmp(atm_name,"W")==0)
        return 183.840000;
    else if(strcmp(atm_name,"Re")==0)
        return 186.207000;
    else if(strcmp(atm_name,"Os")==0)
        return 190.230000;
    else if(strcmp(atm_name,"Ir")==0)
        return 192.217000;
    else if(strcmp(atm_name,"Pt")==0)
        return 195.078000;
    else if(strcmp(atm_name,"Au")==0)
        return 196.966500;
    else if(strcmp(atm_name,"Hg")==0)
        return 200.590000;
    else if(strcmp(atm_name,"Tl")==0)
        return 204.383300;
    else if(strcmp(atm_name,"Pb")==0)
        return 207.200000;
    else if(strcmp(atm_name,"Bi")==0)
        return 208.980400;
    else if(strcmp(atm_name,"Po")==0)
        return 209.000000;
    else if(strcmp(atm_name,"At")==0)
        return 210.000000;
    else if(strcmp(atm_name,"Rn")==0)
        return 222.000000;
    else if(strcmp(atm_name,"Fr")==0)
        return 223.000000;
    else if(strcmp(atm_name,"Ra")==0)
        return 226.000000;
    else if(strcmp(atm_name,"Ac")==0)
        return 227.000000;
    else if(strcmp(atm_name,"Th")==0)
        return 232.038100;
    else if(strcmp(atm_name,"Pa")==0)
        return 231.035900;
    else if(strcmp(atm_name,"U")==0)
        return 238.028900;
    else if(strcmp(atm_name,"Np")==0)
        return 237.000000;
    else if(strcmp(atm_name,"Pu")==0)
        return 244.000000;
    else if(strcmp(atm_name,"Am")==0)
        return 243.000000;
    else if(strcmp(atm_name,"Cm")==0)
        return 247.000000;
    else if(strcmp(atm_name,"Bk")==0)
        return 247.000000;
    else if(strcmp(atm_name,"Cf")==0)
        return 251.000000;
    else if(strcmp(atm_name,"Es")==0)
        return 252.000000;
    else if(strcmp(atm_name,"Fm")==0)
        return 257.000000;
    else if(strcmp(atm_name,"Md")==0)
        return 258.000000;
    else if(strcmp(atm_name,"No")==0)
        return 259.000000;
    else if(strcmp(atm_name,"Lr")==0)
        return 262.000000;
    else if(strcmp(atm_name,"Rf")==0)
        return 261.000000;
    else if(strcmp(atm_name,"Db")==0)
        return 262.000000;
    else if(strcmp(atm_name,"Sg")==0)
        return 266.000000;
    else if(strcmp(atm_name,"Bh")==0)
        return 264.000000;
    else if(strcmp(atm_name,"Hs")==0)
        return 277.000000;
    else if(strcmp(atm_name,"Mt")==0)
        return 268.000000;
    else
        return -1.0;
}



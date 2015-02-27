#include "command_del_atoms.h"
#include "box_collection.h"
#include "region_collection.h"
#include "error.h"
#include "memory.h"
#define RNG_M 2147483647
#define RNG_MI 1.0/2147483647
#define RNG_A 16807
#define RNG_Q 127773
#define RNG_R 2836
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Command_del_atoms::Command_del_atoms(Xtal* xtal,
int narg,char** arg):InitPtrs(xtal)
{
    
    if(narg<4)
    {
        error->warning("del_atoms command needs at least 4 arguments\n"
        "SYNTAX: del_atoms region include/exclude box region0 region1 ...\n"
        "or\n"
        "SYNTAX: del_atoms random box type fraction seed\n"
        "or\n"
        "SYNTAX: del_atoms id box file0 file1 ...\n");
        return;
    }
    
    if(strcmp(arg[1],"region")==0)
    {
        if(narg<5)
        {
            error->warning("del_atoms command needs at least 4 arguments\n"
            "SYNTAX: del_atoms region include/exclude box region0 region1 ...\n");
            return;
        }
        
        if(strcmp(arg[2],"include")==0)
            include=1;
        else if(strcmp(arg[2],"exclude")==0)
            include=0;
        else
            error->warning("uknown keyword "
            "for del_atoms region: %s",arg[2]);
        
        int ibox=box_collection->find(arg[3]);
        if(ibox<0)
        {
            error->warning("box %s not found",arg[3]);
            return;
        }

        
        
        int no_regions=narg-4;
        int* iregions;
        CREATE1D(iregions,no_regions);
        
        for(int iregion=0;iregion<no_regions;iregion++)
        {
            iregions[iregion]=region_collection->find(arg[4+iregion]);
            if(iregions[iregion]<0)
            {
                error->warning("region %s not found",arg[4+iregion]);
                delete [] iregions;
                return;
            }
            
        }
        
        
        

        
        Box* box=box_collection->boxes[ibox];
        int natms=box->natms;
        type0* s=box->s;
        type0** H=box->H;

        Region* region;
        
        int* id_lst;
        CREATE1D(id_lst,natms);
        
        
        int icomp=0;
        int no=0;
        int chk;
        int iregion;
        for(int i=0;i<natms;i++)
        {
            chk=1;
            iregion=0;
            while(chk==1 && iregion<no_regions)
            {
                region=region_collection->regions[iregions[iregion]];
                if(region->belong(H,&s[icomp])!=include)
                    chk=0;
                iregion++;
            }
            
            if(chk)
                id_lst[no++]=i;

            icomp+=3;
        }
        
        box_collection->boxes[ibox]->del_atoms(no,id_lst);
        
        
        if(natms)
            delete [] id_lst;

        if(no_regions)
            delete [] iregions;
    }
    else if(strcmp(arg[1],"random")==0)
    {
        
        if(narg!=6)
        {
            error->warning("del_atoms command needs at least 4 arguments\n"
            "SYNTAX: del_atoms random box type fraction seed\n");
            return;
        }
        
        int ibox=box_collection->find(arg[2]);
        
        if(ibox<0)
        {
            error->warning("box %s not found",arg[2]);
            return;
        }
        Box* box=box_collection->boxes[ibox];
        int itype=box->find_type(arg[3]);
        if(itype<0)
        {
            error->warning("type %s not found"
            " in box %s",arg[2],arg[3]);
            return;
        }
        type0 frac=atof(arg[4]);
        type0 frac_ach;
        if(frac>=1.0 || frac<=0.0)
        {
            error->warning("fraction should be between 0.0 and 1.0");
            return;
        }
        
        seed=atoi(arg[5]);
        if(seed<0)
        {
            error->warning("seed should be larger than 0");
            return;
        }
        
        //warm up rand_gen
        for(int i=0;i<20;i++)
            rand_gen();
        
        
        
        int natms=box->natms;
        int* type=box->type;
        
        int* id_lst;
        CREATE1D(id_lst,natms);
        
        int no=0;
        for(int i=0;i<natms;i++)
        {
            if(type[i]==itype)
            {
                if(rand_gen()<frac)
                {
                    id_lst[no++]=i;
                }
            }
        }
        frac_ach=static_cast<type0>(no)
        /static_cast<type0>(box->type_count[itype]);
        
        box->del_atoms(no,id_lst);
        if(natms)
            delete [] id_lst;
        printf("\nfraction desired: %lf fraction achieved: %lf\n\n",frac,frac_ach);
    }
    else if(strcmp(arg[1],"id")==0)
    {
        if(narg<4)
        {
            error->warning("del_atoms command needs at least 3 arguments\n"
            "SYNTAX: del_atoms id box file0 file1 ...\n");
            return;
        }
        
        int ibox=box_collection->find(arg[2]);
        if(ibox<0)
        {
            error->warning("box %s not found",arg[2]);
            return;
        }
        Box* box=box_collection->boxes[ibox];

        int natms=box->natms;
        int no_files=narg-3;
        int id_list_size=0;
        int tmp;
        int size;
        int* id_list=NULL;
        FILE* fp;
        char* line;
        CREATE1D(line,MAX_CHAR);
        
        for(int ifile=0;ifile<no_files;ifile++)
        {
            
            fp=fopen(arg[3+ifile],"r");
            if(fp==NULL)
            {
                error->warning("%s file not found",arg[3+ifile]);
                if(id_list_size)
                    delete [] id_list;
                if(MAX_CHAR)
                    delete [] line;
                return;
            }
            
            fgets(line,MAX_CHAR,fp);
            sscanf(line,"%d",&size);
            GROW(id_list,id_list_size,id_list_size+size);
            
            for(int i=0;i<size;i++)
            {
                if(feof(fp))
                {
                    error->warning("%s file ended immaturely",arg[3+ifile]);
                    if(id_list_size)
                        delete [] id_list;
                    if(MAX_CHAR)
                        delete [] line;
                    return;
                }
                
                fgets(line,MAX_CHAR,fp);
                sscanf(line,"%d",&tmp);
                id_list[id_list_size+i]=tmp;
                if(tmp<0 || natms<=tmp)
                {
                    error->warning("atom id should be between %d & %d");
                    if(id_list_size)
                        delete [] id_list;
                    if(MAX_CHAR)
                        delete [] line;
                    return;
                }
            }
            
            fclose(fp);
            
            id_list_size+=size;
        }
        //delete line
        if(MAX_CHAR)
            delete [] line;
        
        
        //sort
        for(int i=0;i<id_list_size;i++)
            for(int j=i+1;j<id_list_size;j++)
            {
                if(id_list[i]>id_list[j])
                {
                    tmp=id_list[i];
                    id_list[i]=id_list[j];
                    id_list[j]=tmp;
                }
            }
        
        //find the same atoms
        int same_cnt=0;
        for(int i=1;i<id_list_size;i++)
            if(id_list[i]==id_list[i-1])
                same_cnt++;

        //remove the same atoms
        int* del_atms;
        int del_atms_no=id_list_size-same_cnt;
        
        if(same_cnt)
        {
            CREATE1D(del_atms,del_atms_no);
            if (del_atms_no)
                del_atms[0]=id_list[0];
            int cur=1;
            for(int i=1;i<id_list_size;i++)
                if(id_list[i]!=id_list[i-1])
                    del_atms[cur++]=id_list[i];
        }
        else
            del_atms=id_list;
        
        
        box->del_atoms(del_atms_no,del_atms);
        
        if(id_list_size)
            delete [] id_list;
        
        
        
        
    }
    else
    {
        error->warning("uknown keyword "
        "for del_atoms: %s",arg[1]);
        return;
    }
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Command_del_atoms::~Command_del_atoms()
{
    
}
/*--------------------------------------------
 uniform number generator between 0.0 and 1.0
 --------------------------------------------*/
type0 Command_del_atoms::rand_gen()
{
    int first,second,rnd;
    
    first = seed/RNG_Q;
    second = seed%RNG_Q;
    rnd = RNG_A*second-RNG_R*first;
    if (rnd > 0)
        seed=rnd;
    else
        seed=rnd+RNG_M;
    return seed*RNG_MI;
}
#include "region_block.h"
#include "memory.h"
#include "error.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
Region_block::Region_block(Xtal* xtal
,int narg,char** arg):Region(xtal)
{
    type0 tmp0,tmp1;
    alloc=0;
    
    if(narg!=5)
    {
        error->warning("region block needs 4 arguments\n"
        "SYNTAX: block name (lo_x,hi_x) (lo_y,hi_y) (lo_z,hi_z)");
        xtal->error_flag=-1;
        return;
        
    }
    
    
    int lngth=static_cast<int>(strlen(arg[1]))+1;
    CREATE1D(region_name,lngth);
    CREATE1D(bond,6);
    CREATE1D(bond_st,6);
    alloc=1;
    
    memcpy(region_name,arg[1],lngth);
    
    for(int i=0;i<6;i++)
        bond_st[i]=1;

    
    for(int i=0;i<3;i++)
    {
        if(strcmp(arg[2+i],"(none,none)")==0)
        {
            bond_st[2*i]=bond_st[2*i+1]=0;
        }
        else
        {

            if(sscanf(arg[2+i],"(%lf,%lf)",&tmp0,&tmp1)==2)
            {
                bond[2*i]=tmp0;
                bond[2*i+1]=tmp1;
            }
            else
            {
                if(sscanf(arg[2+i],"(none,%lf)",&tmp1)==1)
                {
                    bond_st[2*i]=0;
                    bond[2*i+1]=tmp1;
                }
                else if(sscanf(arg[2+i],"(%lf,none)",&tmp0)==1)
                {
                    bond_st[2*i+1]=0;
                    bond[2*i]=tmp0;
                }
                else
                {
                    error->warning("block boundary for dimension %d "
                    "should be of the format: (lo,hi)",i);
                    xtal->error_flag=-1;
                    return;

                }
            }

        }
        

    }
    
    for(int i=0;i<3;i++)
    {
        if(bond_st[2*i] && bond_st[2*i+1])
        {
            if(bond[2*i]>bond[2*i+1])
            {
                error->warning("lower bound of dimension "
                "%d should be less than higher bound\n",i);
                xtal->error_flag=-1;
                return;
            }
        }
    }
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
Region_block::~Region_block()
{
    if(alloc)
    {
        delete [] bond;
        delete [] bond_st;
        delete [] region_name;
    }


}
/*--------------------------------------------
 belong
 --------------------------------------------*/
int Region_block::belong(type0** H,type0* s)
{
    
    x=H[0][0]*s[0]+H[1][0]*s[1]+H[2][0]*s[2];
    y=H[1][1]*s[1]+H[2][1]*s[2];
    z=H[2][2]*s[2];
    
    if(bond_st[0])
        if (x<bond[0])
            return 0;
    
    if(bond_st[1])
        if (bond[1]<=x)
            return 0;

    if(bond_st[2])
        if (y<bond[2])
            return 0;
    
    if(bond_st[3])
        if (bond[3]<=y)
            return 0;
    
    if(bond_st[4])
        if (z<bond[4])
            return 0;
    
    if(bond_st[5])
        if (bond[5]<=z)
            return 0;

    
    return 1;
    
}
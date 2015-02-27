#include "region_collection.h"
#include "region_styles.h"
#include "memory.h"
#include "error.h"
/*--------------------------------------------
 constructor
 --------------------------------------------*/
RegionCollection::RegionCollection(Xtal* xtal):InitPtrs(xtal)
{
    no_regions=0;
    
}
/*--------------------------------------------
 destructor
 --------------------------------------------*/
RegionCollection::~RegionCollection()
{


    for(int i=0;i<no_regions;i++)
        delete regions[i];

    
    
    if(no_regions)
        delete [] regions;
    
    no_regions=0;
}
/*--------------------------------------------
 add a region
 --------------------------------------------*/
int RegionCollection::add_cmd(int narg,char** arg)
{
 
    class Region** new_regions;
    CREATE1D(new_regions,no_regions+1);
    
    for(int i=0;i<no_regions;i++)
        new_regions[i]=regions[i];
  
    CREATE1D(regions,no_regions+1);
    
    #define Region_Style
    #define RegionStyle(class_name,style_name) \
    else if(strcmp(arg[0],#style_name)==0){   \
    new_regions[no_regions]=new class_name(xtal,narg,arg);}
    if(0){}
    #include "region_styles.h"
    else
    {
        error->warning("unknown region: %s",arg[1]);
        return -1;
    }
    #undef Region_Style
    #undef RegionStyle

    
    if(xtal->error_flag==-1)
    {
        xtal->error_flag=0;
        delete new_regions[no_regions];
        delete [] new_regions;
        return -1;
    }
    
    if(no_regions)
        delete [] regions;
    regions=new_regions;

    no_regions++;
    return no_regions-1;
}
/*--------------------------------------------
 find a region by name
 --------------------------------------------*/
int RegionCollection::find(char* name)
{
    for(int i=0;i<no_regions;i++)
    {
        if(strcmp(name,regions[i]->region_name)==0)
            return i;
    }
    return -1;
}
/*--------------------------------------------
 remove a region by number
 --------------------------------------------*/
void RegionCollection::del(int iregion)
{
    delete regions[iregion];
    
    Region** new_regions;
    CREATE1D(new_regions,no_regions-1);
    for(int i=0;i<iregion;i++)
        new_regions[i]=regions[i];
    for(int i=iregion+1;i<no_regions;i++)
        new_regions[i-1]=regions[i];
    
    delete [] regions;
    
    regions=new_regions;
    no_regions--;

}
/*--------------------------------------------
 remove a region by number
 --------------------------------------------*/
void RegionCollection::del(char* name)
{
    del(find(name));
}




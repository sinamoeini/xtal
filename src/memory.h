#ifndef __xtal__memory__
#define __xtal__memory__
#include <iostream>
#include <exception>
#include <stdio.h>
#include "init.h"

#define GROW(A,oldsize,newsize) memory->groww(A,oldsize,newsize,#A,__LINE__,__FILE__,__FUNCTION__)
#define CREATE1D(A,d0) memory->fcreate(A,d0,#A,__LINE__,__FILE__,__FUNCTION__)
#define CREATE2D(A,d0,d1) memory->fcreate(A,d0,d1,#A,__LINE__,__FILE__,__FUNCTION__)
using namespace std;
    class Memory {
    private:
    protected:
    public:
        Memory(){}
        ~Memory(){}
        
        // allocation for 1d arrays

        
        template <typename TYPE>
        TYPE* fcreate(TYPE*& array,int d0,const char* name,int line_no,const char* file,const char* function)
        {
            if(d0==0)
            {
                array=NULL;
                return NULL;
            }
            //array = NULL;
            try
            {
                array = new TYPE [d0];
            }
            catch(bad_alloc&)
            {
                printf("memory allocation "
                             "failure in file  %s, function %s, line: %d for variable: %s",file,function,line_no,name);
                exit(EXIT_FAILURE);
            }
            return array;
        }
        
        template <typename TYPE>
        TYPE** fcreate(TYPE**& array,int d0,int d1,const char* name,int line_no,const char* file,const char* function)
        {
            fcreate(array,d0,name,line_no,file,function);
            for(int i=0;i<d0;i++)
                fcreate(array[i],d1,name,line_no,file,function);
            return array;
        }
        
        template <typename TYPE>
        TYPE* groww(TYPE*& array,int oldsize,int newsize,const char* name,int line_no,const char* file,const char* function)
        {
            if (oldsize==0)
            {
                return fcreate(array,newsize,name,line_no,file,function);
            }
            else if (oldsize==newsize)
            {
                return array;
            }
            else
            {
                
                TYPE* newarray=array;
                try
                {
                    int size1=newsize;
                    int size2=oldsize;
                    int size=MIN(size1,size2);
                    newarray = new TYPE[newsize];
                    memcpy(newarray,array,size*sizeof(TYPE));
                    delete [] array;
                    array=newarray;
                }
                catch (bad_alloc&)
                {
                    printf("reallocation "
                    "failure in file  %s, function %s, line: %d for variable: %s",file,function,line_no,name);
                    exit(EXIT_FAILURE);
                }
                return array;
            }
        }
        template <typename TYPE>
        TYPE* resizee(TYPE*& array,int oldsize,int newsize,const char* name,int line_no,const char* file,const char* function)
        {
            if (oldsize==0)
            {
                return fcreate(array,newsize,name,line_no,file,function);
            }
            else if (oldsize==newsize)
            {
                return array;
            }
            else
            {
                
                TYPE* newarray=array;
                try
                {
                    newarray = new TYPE[newsize];
                    memcpy(newarray,array,newsize*sizeof(TYPE));
                    delete [] array;
                    array=newarray;
                }
                catch (bad_alloc&)
                {
                    printf("reallocation "
                    "failure in file  %s, function %s, line: %d for variable: %s",file,function,line_no,name);
                    exit(EXIT_FAILURE);                    
                }
                return array;
            }
        }
        
    };



#endif /* defined(__xtal__memory__) */

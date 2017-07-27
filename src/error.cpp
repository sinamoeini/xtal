#include <stdlib.h>
#include <stdio.h>
#include <cstdarg>
#include <cstdlib>
#include <string.h>
#include "error.h"
/*--------------------------------------------
 constructor of the error handler:
 --------------------------------------------*/
Error::Error(Xtal* xtal) : InitPtrs(xtal)
{

}
/*--------------------------------------------
 destructor of the error handler:
 --------------------------------------------*/
Error::~Error()
{
}
/*--------------------------------------------
 output the error line and abort the code
 --------------------------------------------*/
void Error::abort(const char* msg,...)
{
    char err_msg[MAX_CHAR];
    
    va_list args;
    va_start (args, msg);
    vsprintf (err_msg,msg, args);
    
    printf("ABORTED! %s \n",err_msg);
    va_end (args);
    exit(EXIT_FAILURE);
}
/*--------------------------------------------
 output the error line and abort the code
 --------------------------------------------*/
void Error::abort(int line,char* file
,const char* msg,...)
{
    char err_msg[MAX_CHAR];
    
    va_list args;
    va_start (args, msg);
    vsprintf (err_msg,msg, args);
    
    printf("ABORTED! %s \n"
           ,err_msg);
    
    
    va_end (args);
    
    char* pch=NULL;
    pch=strchr(file,'/');
    int start=0;
    while (pch!=NULL)
    {
        start=static_cast<int>(pch-file);
        pch=strchr(pch+1,'/');
    }
    start++;
    int fin=static_cast<int>(strlen(file));
    
    printf("For more details see ");
    for(int i=start;i<fin;i++)
        printf("%c",file[i]);
    printf(":%d\n",line);
    
    exit(EXIT_FAILURE);
}
/*--------------------------------------------
 output the warning msg
 --------------------------------------------*/
void Error::warning(const char *msg,...)
{
    char war_msg[MAX_CHAR];
    
    va_list args;
    va_start (args, msg);
    vsprintf (war_msg,msg, args);
    
    printf("%s \n"
           ,war_msg);
        
    va_end (args);
}



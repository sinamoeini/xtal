#include "xtal.h"
#include "memory.h"
#include "error.h"
#include "box_collection.h"
#include "region_collection.h"
#include "command_styles.h"
char** cmd;
/*--------------------------------------------
 constructor of the main executer
 --------------------------------------------*/
Xtal::Xtal(int narg,char** args)
{
    no_cmds=no_vals=0;
    memory=new Memory;
    error=new Error(this);
    box_collection=new BoxCollection(this);
    region_collection=new RegionCollection(this);
    
    input_file=NULL;
    input_file=stdin;
    
    
    int iarg=1;
    while(iarg<narg)
    {
        if(strcmp(args[iarg],"-i")==0)
        {
            iarg++;
            if(iarg==narg)
                error->abort("no input file");
            input_file=fopen(args[iarg],"r");
            iarg++;
        }
        else if(strcmp(args[iarg],"-o")==0)
        {
            iarg++;
            if(iarg==narg)
                error->abort("no output file");
            output_file=fopen(args[iarg],"w");
            iarg++;
        }
        else
            error->abort("unknown postfix: %s",args[iarg]);
    }
    if(narg!=1)
    {
        
        if(input_file==NULL)
            error->abort("input file not found");
        
        read_file();
        if(input_file!=stdin)
            fclose(input_file);
    }
    else
    {
        read_history(".xtal_history");
        gen_cmd_ls();
        read_line();
        for(int i=0;i<no_cmds+no_vals+1;i++)
            delete [] cmd[i];
        delete [] cmd;
        write_history(".xtal_history");
    }
}
/*--------------------------------------------
 destructor of the main executer
 --------------------------------------------*/
Xtal::~Xtal()
{
    delete region_collection;
    delete box_collection;
    delete error;
    delete memory;
}
/*--------------------------------------------
 parse a command line:
 chops up a 1d array line into 2d array of
 char, also returns the number of arguments
 if the line starts with # it returns 0
 for example:
 line=
 "  this  is a    test, #here is comment";
 will be converted to:
 arg[0]="this";
 arg[1]="is";
 arg[2]="a";
 arg[3]="test,";
 narg=4;
 --------------------------------------------*/
int Xtal::parse_line(char* line,char**& arg)
{
    arg = NULL;
    int narg = 0;
    int cursor = 0;
    int length= static_cast<int>(strlen(line));
    int ll[length];
    
    while (cursor < length
           && line[cursor]!='#'){
        if (isspace(line[cursor]))
            cursor++;
        else
        {
            if (narg==0 && line[cursor]=='#')
                return 0;
            int i = 0;
            while(!isspace(line[cursor])
                  && cursor < length)
            {
                cursor++;
                i++;
            }
            ll[narg]=i+1;
            narg++;
        }
    }
    
    if (narg==0) return 0;
    
    CREATE1D(arg,narg);
    
    for(int i=0;i<narg;i++)
        CREATE1D(arg[i],ll[i]);
    
    narg = 0;
    cursor = 0;
    
    while (cursor<length&&line[cursor]!='#')
    {
        if ( isspace(line[cursor]))
            cursor++;
        else
        {
            int i=0;
            while(!isspace(line[cursor])
                  && cursor < strlen(line))
                arg[narg][i++]=line[cursor++];
            arg[narg][i] = '\0';
            narg++;
        }
    }
    return narg;
}
/*--------------------------------------------
 hash sign remover:
 it removes the statement after hash sign,
 replaces the withe spaces with space;
 for example:
 line=
 "  this  is a    test, #here is comment";
 will be converted to:
 newline=
 "this is a test,";
 narg=4;
 --------------------------------------------*/
int Xtal::hash_remover(char* line,char*& newline)
{
    newline = NULL;
    CREATE1D(newline,MAX_CHAR);
    
    int narg = 0;
    int cursor = 0;
    int icursor = 0;
    while (cursor < strlen(line)&& line[cursor]!='#')
    {
        if (isspace(line[cursor]))
            cursor++;
        
        else
        {   if (narg!=0)
            newline[icursor++]=' ';
            while(!isspace(line[cursor])
                  && cursor < strlen(line))
                newline[icursor++]=line[cursor++];
            narg++;
        }
    }
    newline[icursor]='\0';
    return narg;
}
/*--------------------------------------------
 hash sign remover:
 it removes the statement after hash sign,
 replaces the withe spaces with space;
 for example:
 line=
 "  this  is a    test, #here is comment";
 will be converted to:
 newline=
 "this is a test,";
 narg=4;
 --------------------------------------------*/
void Xtal::concatenate(int narg,char** args
,char*& line)
{
    int lngth=0;
    int pos=0;
    for(int i=0;i<narg;i++)
        lngth+=static_cast<int>(strlen(args[i]));
    
    lngth+=narg;
    CREATE1D(line,lngth);
    for(int i=0;i<narg;i++)
    {
        lngth=static_cast<int>(strlen(args[i]));
        for(int j=0;j<lngth;j++)
        {
            line[pos]=args[i][j];
            pos++;
        }
        
        if(i!=narg-1)
        {
            line[pos]=' ';
            pos++;
        }
        else
        {
            line[pos]='\0';
            pos++;
        }
    }
}
/*--------------------------------------------
 test field
 --------------------------------------------*/
void Xtal::read_line()
{
    /*
    rl_readline_name=(char*)"xtal-0.0$ ";
    rl_attempted_completion_function=completion;
    */
    rl_completion_entry_function=(Function*)command_generator;
    FILE* tmp_input;
    tmp_input=fopen(".input","w");
    char* line,shell_prompt[100];
    snprintf(shell_prompt,sizeof(shell_prompt),"xtal-0.0$ ");
    
    while(1)
    {
        line=readline(shell_prompt);
        rl_bind_key('\t',rl_complete);
        
        if(strcmp(line,"exit")==0)
        {
            fclose(input_file);
            return;
        }
        
        command(line);
        fprintf(tmp_input,"%s\n",line);
        add_vals();
        add_history(line);
        free(line);
    }
    
    
    /*
    FILE* tmp_input;
    tmp_input=fopen(".intput","w");
    char* line, shell_prompt[100];
    snprintf(shell_prompt, sizeof(shell_prompt), "xtal$ ");
    
    rl_completion_entry_function=(Function*)command_generator;
    while(1)
    {
        line=readline(shell_prompt);
        rl_bind_key('\t',rl_complete);
        
        if(strcmp(line,"exit")==0)
        {
            fclose(input_file);
            return;
        }
        
        command(line);
        fprintf(tmp_input,"%s\n",line);
        add_vals();
        add_history(line);
        
        free(line);
    }
     */
}
/*--------------------------------------------
 test field
 --------------------------------------------*/
void Xtal::read_file()
{
    int input_file_chk=1;
    char* line;
    int lngth;
    int size=MAX_CHAR;
    int line_complt,pos;
    int max_line_char;
    char* srch;
    max_line_char=size;
    CREATE1D(line,size);
    
    while (input_file_chk)
    {
        pos=0;
        line_complt=0;
        
        while (line_complt==0)
        {
            fgets(&line[pos],size-pos,input_file);
            lngth=static_cast<int>(strlen(line));
            while(line[lngth-1]!='\n')
            {
                pos=lngth;
                GROW(line,size,pos+max_line_char);
                size=pos+max_line_char;
                fgets(&line[pos],size-pos,input_file);
                lngth=static_cast<int>(strlen(line));
            }
            
            srch=NULL;
            srch=strchr(line,'\\');
            if(srch==NULL)
            {
                line_complt=1;
            }
            else
            {
                pos=static_cast<int>(srch-line);
                GROW(line,size,pos+max_line_char);
                size=pos+max_line_char;
            }
        }
        if(feof(input_file))
        {
            input_file_chk=0;
            continue;
        }
        command(line);
        
    }
    
    delete [] line;
}
/*--------------------------------------------
 analysing the commands
 --------------------------------------------*/
void Xtal::command(char* command)
{
    char** args;
    int narg;
    narg=parse_line(command,args);
    if(narg==0)
        return;

    
    #define Command_Style
    #define CommandStyle(class_name,style_name) \
    else if(strcmp(args[0],#style_name)==0){    \
    class class_name* command =                 \
    new class_name(this,narg,args);             \
    delete command;}
    
    if(0){}
    #include "command_styles.h"
    else
        error->warning("unknown command: %s",args[0]);
    #undef Command_Style
    #undef CommandStyle
}
/*--------------------------------------------
 generate list of commands for auto-complete
 --------------------------------------------*/
void Xtal::gen_cmd_ls()
{
    
    if(0) return;
    no_cmds=0;
    #define Command_Style
    #define CommandStyle(class_name,style_name) no_cmds++;
    #include "command_styles.h"
    #undef CommandStyle
    #undef Command_Style
    
    
    CREATE1D(cmd,no_cmds+1);
    
    
    int i=0;
    int size;
    #define Command_Style
    #define CommandStyle(class_name,style_name)    \
    size=static_cast<int>(strlen(#style_name))+1;  \
    CREATE1D(cmd[i],size);                         \
    memcpy(cmd[i],#style_name,                     \
    size*sizeof(char)); i++;
    #include "command_styles.h"
    #undef Command_Style
    #undef CommandStyle
    
    
    CREATE1D(cmd[no_cmds],2);
    size=static_cast<int>(strlen("\r"))+1;
    memcpy(cmd[no_cmds],"\r",sizeof(char)*size);
    cmds_lst=cmd;
    no_vals=0;
}
/*--------------------------------------------
 generate list of commands for auto-complete
 --------------------------------------------*/
void Xtal::add_vals()
{

    char** cmd_tmp;
    int no_boxes=box_collection->no_boxes;
    int no_regions=region_collection->no_regions;
    int icurs=0;
    int size=no_cmds+no_boxes+no_regions+1;
    CREATE1D(cmd_tmp,size);
    for(int i=0;i<no_cmds;i++)
    {
        size=static_cast<int>(strlen(cmd[i]))+1;
        CREATE1D(cmd_tmp[icurs],size);
        memcpy(cmd_tmp[icurs],cmd[i],size*sizeof(char));
        icurs++;
    }
    for(int i=0;i<no_boxes;i++)
    {
        size=static_cast<int>(strlen(box_collection->boxes[i]->box_name))+1;
        CREATE1D(cmd_tmp[icurs],size);
        memcpy(cmd_tmp[icurs],box_collection->boxes[i]->box_name,size*sizeof(char));
        icurs++;
    }
    for(int i=0;i<no_regions;i++)
    {
        size=static_cast<int>(strlen(region_collection->regions[i]->region_name))+1;
        CREATE1D(cmd_tmp[icurs],size);
        memcpy(cmd_tmp[icurs],region_collection->regions[i]->region_name,size*sizeof(char));
        icurs++;
    }
    
    CREATE1D(cmd_tmp[icurs],2);
    size=static_cast<int>(strlen("\r"))+1;
    memcpy(cmd_tmp[icurs],"\r",sizeof(char)*size);
    
    for(int i=0;i<no_vals+no_cmds+1;i++)
        delete [] cmd[i];
    delete [] cmd;
    
    no_vals=no_boxes+no_regions;
    cmd=cmd_tmp;
    cmds_lst=cmd;

}
/*----------------------------------------------------------------------------------------
 readline crap
 ----------------------------------------------------------------------------------------*/
char** completion(const char* text,int start,int end)
{
    char **matches;
    
    matches=(char** )NULL;
    
    if (start==0)
        matches=rl_completion_matches(text,command_generator);
    return (matches);
}
/*--------------------------------------------

 --------------------------------------------*/
char* command_generator(const char* text,int state)
{
    static int icmd,len;
    char* name;
    if (!state)
    {
        icmd=0;
        len=static_cast<int>(strlen(text));
    }
    while ((name=cmd[icmd]) && strncmp(cmd[icmd],"\r",1)!=0)
    {
        icmd++;
        if (strncmp(name,text,len)==0)
        {
            char* r;
            r=(char* )malloc((static_cast<int>(strlen(name))+1));
            memcpy(r,name,(static_cast<int>(strlen(name))+1)*sizeof(char));
            return r;
        }
    }
    return ((char *)NULL);
}
/*----------------------------------------------------------------------------------------

 ----------------------------------------------------------------------------------------*/



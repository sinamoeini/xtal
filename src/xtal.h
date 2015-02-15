#ifndef __xtal__xtal__
#define __xtal__xtal__
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>

typedef double type0;
char* command_generator(const char*,int);
char** completion(const char*,int,int);

class Xtal
{
private:

    void gen_cmd_ls();
    
protected:
public:
    class Memory* memory;
    class Error* error;
    class BoxCollection* box_collection;
    
    Xtal(int,char**);
    ~Xtal();
    
    FILE* output_file;
    FILE* input_file;
    
    int parse_line(char*,char**&);
    int hash_remover(char*,char*&);
    void concatenate(int,char**,char*&);
    
    void read_file();
    void read_line();
    void command(char*);
    int error_flag;
    void add_vals();
    
    int no_cmds;
    int no_vals;
    char** cmds_lst;
    
};
#endif

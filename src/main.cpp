#include <iostream>
#include "xtal.h"

using namespace std;
int main(int narg, char** arg)
{
    Xtal* xtal=new Xtal(narg,arg);
    delete xtal;    
    return EXIT_SUCCESS;
}


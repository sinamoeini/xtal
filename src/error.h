#ifndef __xtal__error__
#define __xtal__error__
#include <stdio.h>
#include "init.h"
class Error : protected InitPtrs {
private:
protected:
public:
    Error(Xtal *);
    ~Error();
    void abort(const char*,...);
    void abort(int,const char*,const char*,...);
    void warning(const char*,...);
};

#endif

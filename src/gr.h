#ifndef __xtal__gr__
#define __xtal__gr__

#include <stdio.h>
#include "init.h"
#include "box.h"



class Gr:protected InitPtrs
{
private:
    Box*& box;
    type0& cutoff;
    type0& k_cutoff;
    
    type0 s_cutoff[3];
    bool (&dims)[3];
    int nbins[3];
    int bin_vec[3];
    
    int* bin_list;
    int* next_bin;
    
    void get_s_cutoff();
    void setup_bins();
    void deallocate_bins();
    void allocate_gr();
    
    void finalize_gr();
    
    type0 dr,dk;
    type0 dr_inv;
    
    void find_bin(type0*,int(&)[3]);
    int find_bin(type0*);
    int* neigh_bins;
    int nneigh_bins;
    type0 dist(int&,int&);
    
    int& no_bins;
    type0***& gr;
    type0***& sr;
    void prepare_2d();
    void prepare_1d(int);
    int ndims;
public:
    Gr(Xtal*,type0***&,type0***&,Box*&,bool(&)[3],type0&,type0&,int&);
    ~Gr();
};

#endif

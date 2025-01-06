#ifndef RCP_H
#define RCP_H

#include "mmu.h"
#include "rsp.h"

//#include "rdp.h";

typedef struct {
    MMU mmu;
    RSP rsp;
    Parallel_Interface pi;
    Ram_Interface ri;
    Serial_Interface si;
    Video_Interface vi;
    //RDP rdp;
} RCP;

void init_rcp(RCP *rcp, Memory *mem);

#endif
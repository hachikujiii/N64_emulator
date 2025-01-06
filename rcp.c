#include "rcp.h"

void init_rcp(RCP *rcp, Memory *mem) {

    init_si(&rcp->si, mem);
    init_pi(&rcp->pi, mem);
    init_rsp(&rcp->rsp);
    init_mmu(&rcp->mmu, &rcp->rsp);

    rcp->mmu.si = &rcp->si;
    rcp->mmu.pi = &rcp->pi;
    rcp->mmu.ri = &rcp->ri;
    rcp->mmu.vi = &rcp->vi;
    
}
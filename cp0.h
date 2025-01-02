#ifndef CP0_H
#define CP0_H

#include <stdint.h>
#include "tlb.h"

#define NUM_CP0_REGISTERS 32

typedef enum {
    CP0_REG_INDEX = 32,
    CP0_REG_RANDOM = 33,
    CP0_REG_ENTRYLO0 = 34,
    CP0_REG_ENTRYLO1 = 35,
    CP0_REG_CONTEXT = 36,
    CP0_REG_PAGEMASK = 37,
    CP0_REG_WIRED = 38,
    CP0_REG_BADVADDR = 40,
    CP0_REG_COUNT = 41,
    CP0_REG_ENTRYHI = 42,
    CP0_REG_COMPARE = 43,
    CP0_REG_STATUS = 44,
    CP0_REG_CAUSE = 45,
    CP0_REG_EPC = 46,
    CP0_REG_PRID = 47,
    CP0_REG_CONFIG = 48,
    CP0_REG_LLADDR = 49,
    CP0_REG_WATCHLO = 50,
    CP0_REG_WATCHHI = 51,
    CP0_REG_XCONTEXT = 52,
    CP0_REG_TAGLO = 60,
    CP0_REG_TAGHI = 61,
    CP0_REG_ERROREPC = 62,
} CP0_Register;



typedef struct {

    TLB tlb;
    
} CP0;

#endif
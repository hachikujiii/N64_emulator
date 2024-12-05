#ifndef CP0_H
#define CP0_H

#include <stdint.h>
#include "mmu.h"

typedef struct {
    /*32 registers*/
    uint32_t Index;
    uint32_t Random;
    uint32_t EntryLo0;
    uint32_t EntryLo1;
    uint32_t Context;
    uint32_t PageMask;
    uint32_t Wired;
    uint32_t BadVAddr;
    uint32_t Count;
    uint32_t EntryHi;
    uint32_t Compare;
    uint32_t Status;
    uint32_t Cause;
    uint32_t EPC;
    uint32_t PRId;
    uint32_t Config;
    uint32_t LLAddr;
    uint32_t WatchLo;
    uint32_t WatchHi;
    uint32_t XContext;
    uint32_t TagLo;
    uint32_t TagHi;
    uint32_t ErrorEPC;
    /* 7, 21-25, 31 reserved for future use*/
    //TLB tlb;
} CP0;

#endif
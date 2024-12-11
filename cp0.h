#ifndef CP0_H
#define CP0_H

#include <stdint.h>
#include "tlb.h"

typedef struct {
    /*32 registers*/
    uint64_t gpr[32];

    //0 Index;
    //1 Random;
    //2 EntryLo0;
    //3 EntryLo1;
    //4 Context;
    //5 PageMask;
    //6 Wired;
    //7 NOT USED;
    //8 BadVAddr
    //9 Count;
    //10 EntryHi;
    //11 Compare;
    //12 Status;
    //13 Cause;
    //14 EPC;
    //15 PRId;
    //16 Config;
    //17 LLAddr;
    //18 WatchLo;
    //19 WatchHi;
    //20 XContext;
    //28 TagLo;
    //29 TagHi;
    //30 ErrorEPC;
    /* 7, 21-25, 31 reserved for future use*/
    TLB tlb;
} CP0;

#endif
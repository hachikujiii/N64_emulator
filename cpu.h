#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include "mmu.h"
#include "cp0.h"
#include "cp1.h"
#include "pipeline.h"

typedef struct {
    Pipeline pipeline;
    MMU mmu;
    CP0 cp0; /*System Control Processor*/
    CP1 cp1; /*Floating Point Unit*/
    uint64_t gpr[32];
    double fpr[32]; 
    uint64_t PC; /*= init_address?*/
    uint64_t delay_slot;
    uint64_t HI, LO;
    bool LLbit;
    float FCR0;
    float FCR31;

    uint64_t instruction_cache[ICACHE_SIZE / 8];
    uint64_t data_cache[DCACHE_SIZE / 8];

} CPU;

#endif
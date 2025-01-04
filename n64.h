#ifndef N64_H
#define N64_H

#include "mmu.h"
#include "cpu.h"
#include "rcp.h"

typedef struct {
    CPU cpu;
    RCP rcp;
    MMU mmu;
    Memory memory;
} N64;

int cold_reset(N64 *n64);
void init_n64(N64 *n64);
void init_cpu(N64 *n64);
void init_rsp(N64 *n64);

#endif
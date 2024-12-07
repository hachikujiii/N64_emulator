#ifndef N64_H
#define N64_H

#include "memory.h"
#include "cpu.h"
//#include "rcp.h"

typedef struct {
    CPU cpu;         // CPU object
//  RCP rcp;         // RCP object
} N64;

int cold_boot(N64 *n64);

#endif
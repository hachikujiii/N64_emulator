#ifndef N64_H
#define N64_H

#include "cpu.h"
#include "my_structs.h"

typedef struct {
    PIF pif_rom;
    CPU cpu;
    RCP rcp;
} N64;

#endif
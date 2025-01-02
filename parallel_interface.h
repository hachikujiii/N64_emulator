#ifndef PARALLEL_INTERFACE_H
#define PARALLEL_INTERFACE_H

#include <stdint.h>
#include "memory.h"

typedef struct {

    uint8_t *cart_rom;

} Parallel_Interface;

void init_pi(Memory *mem, Parallel_Interface *pi);

#endif
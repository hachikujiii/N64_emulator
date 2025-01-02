#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>

typedef struct {

    uint8_t *rdram;
    uint8_t *cart_rom;
    uint8_t *pif_ram;
    uint8_t *pif_rom;

} Memory;

void init_mem(Memory *mem);

#endif
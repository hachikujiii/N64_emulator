#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdlib.h>
#include "cartridge.h"
#include "pif_rom.h"

#define RDRAM_SIZE         0x800000 //8MB RAM
#define PIF_ROM_SIZE       0x7C0

typedef struct {
    PIF pif_rom;
    Cartridge cartridge;
    uint8_t *cartridge_rom;
} Memory;

int initialize_memory(Memory *memory);
void free_memory(Memory *memory);

#endif
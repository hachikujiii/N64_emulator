#ifndef MMU_H
#define MMU_H

#include "mapping.h"
#include "memory.h"
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    Memory *memory;
} MMU;

uint32_t map_virtual_address(MMU *mmu, uint32_t virtual_address);
bool inRange (uint64_t address, uint64_t start, uint64_t end);
uint32_t direct_map(uint32_t virtual_address, uint32_t segment_start);

#endif
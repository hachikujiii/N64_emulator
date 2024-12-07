#ifndef MMU_H
#define MMU_H

#include "mapping.h"
#include "memory.h"
#include <stdint.h>
#include <stdbool.h>
#include "cp0.h"

typedef struct {
    Memory *memory;
} MMU;

uint32_t map_virtual(MMU *mmu, uint32_t virtual_address);
uint32_t map_physical(MMU *mmu, uint32_t physical_address); 
bool inRange (uint64_t address, uint64_t start, uint64_t end);
uint32_t direct_map(uint32_t virtual_address, uint32_t segment_start);

#endif
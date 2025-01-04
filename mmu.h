#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <stdbool.h>
#include "mapping.h"
#include "memory.h"
#include "parallel_interface.h"
#include "ram_interface.h"
#include "serial_interface.h"
#include "cp0.h"
#include "rsp.h"

#define CART_ROM_SIZE      0x2000000    //32MB ROM
#define RDRAM_SIZE         0x800000     //8MB RAM
#define PIF_ROM_SIZE       0x7C0

typedef struct {
    Parallel_Interface pi;
    //Ram_Interface ri;
    Serial_Interface si;
    RSP *rsp;

} MMU;

uint32_t cpu_read(MMU *mmu, uint32_t address);
uint32_t map_virtual(MMU *mmu, uint32_t virtual_address);
uint32_t map_physical(MMU *mmu, uint32_t physical_address); 
bool inRange (uint64_t address, uint64_t start, uint64_t end);
uint32_t direct_map(uint32_t virtual_address, uint32_t segment_start);
void init_mmu(MMU *mmu, Memory *mem, RSP *rsp);

#endif
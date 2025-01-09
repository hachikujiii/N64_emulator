#ifndef MMU_H
#define MMU_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "mapping.h"
#include "memory.h"
#include "cp0.h"
#include "rsp.h"
#include "parallel_interface.h"
#include "serial_interface.h"
#include "video_interface.h"
#include "ram_interface.h"
#include "audio_interface.h"

#define CART_ROM_SIZE      0x2000000    //32MB ROM
#define RDRAM_SIZE         0x800000     //8MB RAM
#define PIF_ROM_SIZE       0x7C0

typedef enum {
    RDRAM,
    RDRAMX,
    RDRAM_REGS,
    SPDMEM,
    SPIMEM,
    SP_REGS,
    DPCOM,
    DPSPAN,
    MI_REGS,
    VI_REGS,
    AI_REGS,
    PI_REGS,
    RI_REGS,
    SI_REGS,
    CART_D2A1,
    CART_D1A1,
    CART_D2A2,
    CART_D1A2,
    PIF_ROM,
    PIF_RAM,
    CART_D1A3,
    EXSYS_DEV,
    PHYSICAL_RANGE_COUNT

} PhysicalRange;

typedef struct {
    
    RSP *rsp;
    Parallel_Interface *pi;
    Ram_Interface *ri;
    Serial_Interface *si;
    Video_Interface *vi;
    Audio_Interface *ai;

} MMU;

void init_mmu(MMU *mmu, RSP *rsp);
bool inRange (uint32_t address, uint32_t start, uint32_t end);
uint32_t direct_map(uint32_t virtual_address, uint32_t segment_start);
uint32_t cpu_bus_request(MMU *mmu, uint32_t address, uint32_t word, bool access);
uint32_t map_virtual(MMU *mmu, uint32_t virtual_address);
uint8_t map_physical(uint32_t physical_address); 

uint32_t read_pif_wrapper(MMU *mmu, uint32_t physical_address);

uint32_t sp_read_reg_wrapper(MMU *mmu, uint32_t physical_address);
void sp_write_reg_wrapper(MMU *mmu, uint32_t physical_address, uint32_t word);

uint32_t pi_read_reg_wrapper(MMU *mmu, uint32_t physical_address);
void pi_write_reg_wrapper(MMU *mmu, uint32_t physical_address, uint32_t word);

uint32_t vi_read_reg_wrapper(MMU *mmu, uint32_t physical_address);
void vi_write_reg_wrapper(MMU *mmu, uint32_t physical_address, uint32_t word);

uint32_t ai_read_reg_wrapper(MMU *mmu, uint32_t physical_address);
void ai_write_reg_wrapper(MMU *mmu, uint32_t physical_address, uint32_t word);

extern uint32_t (*read_table[PHYSICAL_RANGE_COUNT])(MMU *mmu, uint32_t physical_address);
extern void (*write_table[PHYSICAL_RANGE_COUNT])(MMU *mmu, uint32_t physical_address, uint32_t data);

#endif
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "memory.h"
#include "cp0.h"
#include "rom_loading.h"


#define CART_ROM_SIZE      0x2000000    //32MB ROM
#define RDRAM_SIZE         0x800000     //8MB RAM
#define PIF_ROM_SIZE       0x7C0

void init_mem(Memory *mem) {
    
    memset(mem, 0, sizeof(Memory));

    mem->rdram = malloc(RDRAM_SIZE);
    mem->cart_rom = malloc(CART_ROM_SIZE);         
    mem->pif_ram = malloc(PIF_ROM_SIZE);              
    mem->pif_rom = malloc(0x7C0);              

    if (!mem->rdram || !mem->cart_rom || !mem->pif_ram || !mem->pif_rom) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(EXIT_FAILURE);
    }

    load_roms(mem);
}
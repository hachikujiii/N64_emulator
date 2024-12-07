#include <stdio.h>
#include "memory.h"
#include "rom_loading.h"


int initialize_memory(Memory *memory) {

    memory->cartridge.ROM = (unsigned char *)malloc(ROM_SIZE * sizeof(unsigned char));
    if (!memory->cartridge.ROM) {
        fprintf(stderr, "Memory allocation for ROM failed\n");
        return -1;
    }

    memory->pif_rom.data = (unsigned char *)malloc(PIF_ROM_SIZE * sizeof(unsigned char));
    if (!memory->pif_rom.data) {
        fprintf(stderr, "Memory allocation for PIF ROM failed\n");
        free(memory->cartridge.ROM);
        return -1;
    }

    load_roms(memory);
    return 0;
}

void free_memory(Memory *memory) {
    if (!memory) return;

    if (memory->cartridge.ROM) {
        free(memory->cartridge.ROM);
    }

    if (memory->pif_rom.data) {
        free(memory->pif_rom.data);
    }

    free(memory);
}
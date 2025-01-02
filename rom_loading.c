#include <stdlib.h>
#include <stdio.h>
#include "mmu.h"
#include "rom_loading.h"

int load_roms(Memory *mem) {
    // Load ROMs and PIF ROM into the memory object
    FILE *binfile = fopen("OcarinaOfTime.bin", "rb");
    if (!binfile) {
        fprintf(stderr, "Failed to open .bin file\n");
        return -1;
    }
    size_t bytesRead = fread(mem->cart_rom, sizeof(unsigned char), CART_ROM_SIZE, binfile);
    if (bytesRead != CART_ROM_SIZE) {
        fprintf(stderr, "Failed to read the entire cartridge file\n");
        fclose(binfile);
        return -1;
    }
    fclose(binfile);

    FILE *pif_file = fopen("n64_pif.bin", "rb");
    if (!pif_file) {
        fprintf(stderr, "Failed to open PIF ROM file\n");
        return -1;
    }
    size_t pif_bytes_read = fread(mem->pif_rom, sizeof(unsigned char), PIF_ROM_SIZE, pif_file);
    if (pif_bytes_read != PIF_ROM_SIZE) {
        fprintf(stderr, "Failed to read PIF ROM\n");
        fclose(pif_file);
        return -1;
    }
    fclose(pif_file);

    printf("Loaded ROMs into memory\n");
    return 0; // success
}
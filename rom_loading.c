#include <stdlib.h>
#include <stdio.h>
#include "rom_loading.h"

int load_roms(N64 **n64, Cartridge *cartridge) {

    cartridge->ROM = (unsigned char *)malloc(ROM_SIZE * sizeof(unsigned char));
    if (cartridge->ROM == NULL) {
        fprintf(stderr, "Memory allocation for ROM failed\n");
        return -1;
    }

    //open rom dump file (.bin)
    FILE *binfile = fopen("OcarinaOfTime.bin", "rb");
    if(!binfile) {
        fprintf(stderr, "Failed to open .bin file");
        free(cartridge->ROM);
        return -1;
    }

    size_t bytesRead = fread(cartridge->ROM, sizeof(unsigned char), ROM_SIZE, binfile);
    if (bytesRead != ROM_SIZE) {
        //Handle incomplete read
        fprintf(stderr, "Failed to read the entire cartridge file\n");
        fclose(binfile);
        free(cartridge->ROM);
        return -1;
    }
    fclose(binfile);

    *n64 = (N64*) malloc(sizeof(N64));
    if (*n64 == NULL) {
        fprintf(stderr, "memory allocation for N64 failed\n");
        free(cartridge->ROM);
        return -1;
    }

    (*n64)->pif_rom.data = (unsigned char *)malloc(PIF_ROM_SIZE * sizeof(unsigned char));
    if((*n64)->pif_rom.data == NULL) {
        fprintf(stderr, "Memory allocation for PIF ROM failed\n");
        free(cartridge->ROM);
        free(*n64);
        return -1;
    }

    FILE *pif_file = fopen("n64_pif.bin", "rb");
    if(!pif_file) {
        fprintf(stderr, "Failed to open pif rom file");
        free(cartridge->ROM);
        free(*n64);
        return -1;
    }

    size_t pif_bytes_read = fread((*n64)->pif_rom.data, sizeof(unsigned char), PIF_ROM_SIZE, pif_file);
    if(pif_bytes_read != PIF_ROM_SIZE) {
        fprintf(stderr, "Failed to read PIF ROM\n");
        fclose(pif_file);
        free(cartridge->ROM);
        free(*n64);
        return -1;
    }
    fclose(pif_file);
    
    printf("loaded rom\n");
    return 0; //success
}
#include "serial_interface.h"
#include "mapping.h"

void init_si(Serial_Interface *si, Memory *mem){

    si->pif = mem->pif_rom;
}

uint32_t read_pif(Serial_Interface *si, uint32_t address) {

    uint32_t offset = address - PIF_BOOT_ROM_START;
    return (si->pif[offset] << 24) |
           (si->pif[offset + 1] << 16) |
           (si->pif[offset + 2] << 8) |
            si->pif[offset + 3];
}
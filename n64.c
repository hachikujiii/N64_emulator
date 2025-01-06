#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "n64.h"

void init_n64(N64 *n64) {

    init_rcp(&n64->rcp, &n64->memory);
    init_cpu(n64);
    //n64->rcp.mmu = &n64->mmu;
}

void init_cpu(N64 *n64) {

    n64->cpu.PC = 0xBFC00000;
    init_pipeline(&n64->cpu.pipeline);
    n64->cpu.mmu = &n64->rcp.mmu;
}

void free_mem(Memory *mem) {
    free(mem->rdram);
    free(mem->cart_rom);
    free(mem->pif_ram);
    free(mem->pif_rom);
}

int cold_reset(N64 *n64) {

    memset(n64, 0, sizeof(N64));
    init_mem(&n64->memory);
    init_n64(n64);

    printf("initializing complete!\n");
    return 0;

}
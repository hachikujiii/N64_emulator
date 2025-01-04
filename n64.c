#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "n64.h"

void init_n64(N64 *n64) {
    init_mmu(&n64->mmu, &n64->memory, &n64->rcp.rsp);
    init_cpu(n64);
    init_rsp(n64);
    //n64->rcp.mmu = &n64->mmu;
}

void init_cpu(N64 *n64) {

    n64->cpu.PC = 0xBFC00000;
    init_pipeline(&n64->cpu.pipeline);
    n64->cpu.mmu = &n64->mmu;
}

void init_rsp(N64 *n64) {

    n64->rcp.rsp.sp_status = 1;
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
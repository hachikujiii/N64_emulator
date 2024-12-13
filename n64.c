#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "n64.h"

int cold_boot(N64 *n64) {

    memset(n64, 0, sizeof(N64));
    
    // Allocate and initialize MMU
    n64->cpu.mmu = (MMU *)malloc(sizeof(MMU));
    if (n64->cpu.mmu == NULL) {
        fprintf(stderr, "Failed to allocate memory for MMU.\n");
        return -1;
    }
    memset(n64->cpu.mmu, 0, sizeof(MMU));

    // Allocate and initialize Memory
    n64->cpu.mmu->memory = (Memory *)malloc(sizeof(Memory));
    if (n64->cpu.mmu->memory == NULL) {
        fprintf(stderr, "Failed to allocate memory for Memory.\n");
        free(n64->cpu.mmu);
        return -1;
    }
    memset(n64->cpu.mmu->memory, 0, sizeof(Memory));

    initialize_memory(n64->cpu.mmu->memory);

    printf("initializing complete!\n");
    n64->cpu.PC = 0xBFC00000;
    init_pipeline(&n64->cpu.pipeline);
    void init_function_table(CPU *cpu);
    return 0;

}
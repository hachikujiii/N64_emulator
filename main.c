#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "n64.h"
#include "mystructs.h"
#include "debug_sys.h"
#include "memory.h"
#include "rom_loading.h"

int main(int argc, char *argv[]) {

    N64 *n64 = (N64 *)malloc(sizeof(N64));
    if (n64 == NULL) {
        fprintf(stderr, "Failed to allocate memory for N64.\n");
        return -1;
    }

    cold_boot(n64);
    print_pif_ram(n64->cpu.mmu->memory);

    while(true) {  
        
        IC_stage(&n64->cpu);
        RF_stage(&n64->cpu);
        print_instruction(n64->cpu.pipeline.RFEX_READ.instruction);
        EX_stage(&n64->cpu);
        //DC_stage(n64);
        //WB_stage(n64);
        print_cpu(&n64->cpu);
        ReadToWrite(&n64->cpu);

        //special cases
        // ERET  0100 0010 0000 0000 0000 0000 0001 1000
    }     

}
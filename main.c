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
    printf("INITIAL CPU STATE:\n");
    print_cpu(&n64->cpu);
    printf("\n\n");
    int i = 6;
    while(i > 0) { 
        
        IC_stage(&n64->cpu);
        RF_stage(&n64->cpu);
        EX_stage(&n64->cpu);
        DC_stage(&n64->cpu);
        WB_stage(&n64->cpu);
        print_pipeline(&n64->cpu);
        print_cpu(&n64->cpu);
        ReadToWrite(&n64->cpu);
        i--;
        //special cases
        // ERET  0100 0010 0000 0000 0000 0000 0001 1000
    }     

}
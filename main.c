#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "n64.h"
#include "mystructs.h"
#include "debug_sys.h"
#include "rom_loading.h"
#include <pthread.h>

int main(int argc, char *argv[]) {

    N64 *n64 = (N64 *)malloc(sizeof(N64));
    if (n64 == NULL) {
        fprintf(stderr, "Failed to allocate memory for N64.\n");
        return -1;
    }

    cold_reset(n64);
    print_pif_ram(&n64->memory);
    printf("INITIAL CPU STATE:\n");
    print_pipeline(&n64->cpu);
    print_cpu(&n64->cpu);
    print_rsp(&n64->rcp.rsp);
    printf("\n\n");
    int i = 18;
    while(i > 0) { 
        
        IC_stage(&n64->cpu);
        RF_stage(&n64->cpu);
        EX_stage(&n64->cpu);
        DC_stage(&n64->cpu);
        WB_stage(&n64->cpu);
        print_pipeline(&n64->cpu);
        print_cpu(&n64->cpu);
        ReadToWrite(&n64->cpu);
        //special cases
        // ERET  0100 0010 0000 0000 0000 0000 0001 1000
        i--;
    }     
}
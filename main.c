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
    Memory *memory = (Memory *)malloc(sizeof(Memory));
    //if (argc < 2) {
    //    fprintf(stderr, "Usage: %s <rom_file>\n", argv[0]);
    //    return 1;
    //}

    if (initialize_memory(memory) != 0) {
        free(n64);
        free(memory);
        return -1;
    }

    if (load_roms(memory) != 0) {
        free(n64);
        free(memory);
        return -1;
    }

    memset(n64->cpu.gpr, 0, sizeof(n64->cpu.gpr));
    memset(n64->cpu.fpr, 0, sizeof(n64->cpu.fpr));
    printf("initializing complete!\n");

    //boot_sim(n64);


    n64->cpu.PC = 0xBFC00000;
    print_cpu(&n64->cpu);
    return 1;

    while(true) {  
        
        IC_stage(&n64->cpu, memory);
        RF_stage(&n64->cpu);
        EX_stage(&n64->cpu);
        //DC_stage(n64);
        //WB_stage(n64);
        //print
        ReadToWrite(&n64->cpu);

        //special cases
        // ERET  0100 0010 0000 0000 0000 0000 0001 1000
        print_pipeline(&n64->cpu);

    }     

}
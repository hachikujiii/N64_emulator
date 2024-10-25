#ifndef DEBUG_SYS_H
#define DEBUG_SYS_H
#include "my_structs.h"
#include <stdio.h>

void print_regs(N64 *n64) {
    for(int i = 0; i < 32; i++) {
        
        printf("Register %d = %llX\n", i, n64->cpu.gpr[i]);
    }
    printf("\n");
    printf("PC = %llX\n", n64->cpu.PC);
}

void print_cpu(N64 *n64) {

    printf("general purpose registers:\n");
    for(int i = 0; i < 32; i++) {
        printf("Reg %d = %llX\n", i, n64->cpu.gpr[i]);
    }
    printf("\n");

    printf("floating point registers:\n");
    //to do
    printf("PC = %llX\n", n64->cpu.PC);
    printf("HI = %llX\n", n64->cpu.HI);
    printf("LO = %llX\n", n64->cpu.LO);
}

#endif
#ifndef CPU_H
#define CPU_H

#include "mmu.h"
#include "cp0.h"
#include "pipeline.h"

#define ICACHE_SIZE        0x4000 //16KB Instruction Cache
#define DCACHE_SIZE        0x2000 //8KB Data Cache
#define init_address       0xBFC00000 //starting address on boot process

typedef struct {
    MMU *mmu;
    CP0 cp0; /*System Control Processor*/
    //CP1 cp1; /*Floating Point Unit*/
    uint64_t gpr[32];
    double fpr[32]; 
    uint64_t PC; /*= init_address?*/
    uint64_t delay_slot;
    uint64_t HI, LO;
    bool LLbit;
    float FCR0;
    float FCR31;

    uint64_t instruction_cache[ICACHE_SIZE / 8];
    uint64_t data_cache[DCACHE_SIZE / 8];

    Pipeline pipeline;

    // Opcode function table 
    void (*instruction_table[64])();
} CPU;

// CPU fetch function example
uint32_t fetch_instruction(CPU *cpu);
void execute_instruction(uint32_t instruction);
void IC_stage(CPU *cpu, Memory *memory);
void RF_stage(CPU *cpu);
void EX_stage(CPU *cpu);
void ReadToWrite(CPU *cpu);

#endif
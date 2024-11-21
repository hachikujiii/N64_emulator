#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include "mmu.h"
#include "cp0.h"
#include "cp1.h"
#include "instruction_control.h"
#include "pipeline.h"
#include "my_structs.h"

typedef struct CPU {
    Pipeline pipeline;
    MMU mmu;
    CP0 cp0; /*System Control Processor*/
    CP1 cp1; /*Floating Point Unit*/
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

    // Opcode function table 
    void (*instruction_table[64])(struct CPU *cpu);
} CPU;

void initialize_opcode_table(CPU *cpu);

void execute_r_format(CPU *cpu);
void execute_special_format(CPU *cpu);
void J(CPU *cpu);
void JAL(CPU *cpu);
void BEQ(CPU *cpu);
void BNEQ(CPU *cpu);
void BLEZ(CPU *cpu);
void BGTZ(CPU *cpu);
void ADDI(CPU *cpu);
void ADDIU(CPU *cpu);
void SLTI(CPU *cpu);
void SLTIU(CPU *cpu);
void ANDI(CPU *cpu);
void ORI(CPU *cpu);
void XORI(CPU *cpu);
void LUI(CPU *cpu);
void BEQL(CPU *cpu);
void BNEQL(CPU *cpu);
void BLEZL(CPU *cpu);
void BGTZL(CPU *cpu);
void DADDI(CPU *cpu);
void DADDIU(CPU *cpu);
void LDL(CPU *cpu);
void LDR(CPU *cpu);
void LB(CPU *cpu);
void LH(CPU *cpu);
void LWL(CPU *cpu);
void LW(CPU *cpu);
void LBU(CPU *cpu);
void LHU(CPU *cpu);
void LWR(CPU *cpu);
void LWU(CPU *cpu);
void SB(CPU *cpu);
void SH(CPU *cpu);
void SWL(CPU *cpu);
void SW(CPU *cpu);
void SDL(CPU *cpu);
void SDR(CPU *cpu);
void SWR(CPU *cpu);
void LL(CPU *cpu);
void LLD(CPU *cpu);
void LD(CPU *cpu);
void SC(CPU *cpu);
void SCD(CPU *cpu);
void SD(CPU *cpu);
int instruction_exception(CPU *cpu);

#endif
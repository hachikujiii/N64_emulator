#ifndef CPU_H
#define CPU_H

#include "cp0.h"
#include "mmu.h"
#include "pipeline.h"
#include <stdlib.h>

#define ICACHE_SIZE        0x4000 //16KB Instruction Cache
#define DCACHE_SIZE        0x2000 //8KB Data Cache
#define init_address       0xBFC00000 //starting address on boot process


typedef enum {
    GPR_R0, GPR_AT, GPR_V0,
    GPR_V1, GPR_A0, GPR_A1,
    GPR_A2, GPR_A3, GPR_T0,
    GPR_T1, GPR_T2, GPR_T3,
    GPR_T4, GPR_T5, GPR_T6,
    GPR_T7, GPR_S0, GPR_S1,
    GPR_S2, GPR_S3, GPR_S4,
    GPR_S5, GPR_S6, GPR_S7,
    GPR_T8, GPR_T9, GPR_K0,
    GPR_K1, GPR_GP, GPR_SP,
    GPR_FP, GPR_RA,

    // CP0 registers.
    CP0_REG_0, CP0_REG_1, CP0_REG_2,
    CP0_REG_3, CP0_REG_4, CP0_REG_5,
    CP0_REG_6, CP0_REG_7, CP0_REG_8,
    CP0_REG_9, CP0_REG_10, CP0_REG_11,
    CP0_REG_12, CP0_REG_13, CP0_REG_14,
    CP0_REG_15, CP0_REG_16, CP0_REG_17,
    CP0_REG_18, CP0_REG_19, CP0_REG_20,
    CP0_REG_21, CP0_REG_22, CP0_REG_23,
    CP0_REG_24, CP0_REG_25, CP0_REG_26,
    CP0_REG_27, CP0_REG_28, CP0_REG_29,
    CP0_REG_30, CP0_REG_31,

    // CP1 registers.
    CP1_REG_0, CP1_REG_1, CP1_REG_2,
    CP1_REG_3, CP1_REG_4, CP1_REG_5,
    CP1_REG_6, CP1_REG_7, CP1_REG_8,
    CP1_REG_9, CP1_REG_10, CP1_REG_11,
    CP1_REG_12, CP1_REG_13, CP1_REG_14,
    CP1_REG_15, CP1_REG_16, CP1_REG_17,
    CP1_REG_18, CP1_REG_19, CP1_REG_20,
    CP1_REG_21, CP1_REG_22, CP1_REG_23,
    CP1_REG_24, CP1_REG_25, CP1_REG_26,
    CP1_REG_27, CP1_REG_28, CP1_REG_29,
    CP1_REG_30, CP1_REG_31,

    // Miscellanious registers.
    REG_HI, REG_LO,
    CP1_FCR0, CP1_FCR31,

    NUM_VR4300_REGISTERS
} Registers;

typedef struct {

    MMU *mmu;
    CP0 cp0; /*System Control Processor*/
    //CP1 cp1; /*Floating Point Unit*/
    uint64_t regs[NUM_VR4300_REGISTERS];
    double fpr[32]; 
    uint64_t PC;
    bool LLbit;

    uint64_t instruction_cache[ICACHE_SIZE / 8];
    uint64_t data_cache[DCACHE_SIZE / 8];

    Pipeline pipeline;

    // Opcode function table 
    //TODO:
} CPU;

extern void (*instruction_table[64])(CPU *);
extern void (*function_table[64])(CPU *);

extern Control flags[64];
// CPU fetch function example
uint32_t fetch_instruction(CPU *cpu);
void decode(Pipeline *pipeline);
void execute_instruction(CPU *cpu);
void set_flags(Pipeline *pipeline);
void IC_stage(CPU *cpu);
void RF_stage(CPU *cpu);
void EX_stage(CPU *cpu);
void DC_stage(CPU *cpu);
void WB_stage(CPU *cpu);
void ReadToWrite(CPU *cpu);

//opcode functions
void instruction_exception(CPU *cpu);
void R_FORMAT(CPU *cpu);
void J(CPU *cpu);
void JAL(CPU* cpu);
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
void MTC0(CPU *cpu);
void MTC1(CPU *cpu);
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

//r format function codes
void SLL(CPU *cpu);
void SRL(CPU *cpu);
void SRA(CPU *cpu);
void SLLV(CPU *cpu);
void SRLV(CPU *cpu);
void SRAV(CPU *cpu);
void JR(CPU *cpu);
void JALR(CPU *cpu);
void SYSCALL(CPU *cpu);
void BREAK(CPU *cpu);
void MFHI(CPU *cpu);
void MTHI(CPU *cpu);
void MFLO(CPU *cpu);
void MTLO(CPU *cpu);
void DSLLV(CPU* cpu);
void DSRLV(CPU *cpu);
void DSRAV(CPU *cpu);
void MULT(CPU *cpu);
void MULTU(CPU *cpu);
void DIV(CPU *cpu);
void DIVU(CPU *cpu);
void DMULT(CPU *cpu);
void DMULTU(CPU *cpu);
void DDIV(CPU *cpu);
void DDIVU(CPU *cpu);
void ADD(CPU *cpu);
void ADDU(CPU *cpu);
void SUB(CPU *cpu);
void SUBU(CPU *cpu);
void AND(CPU *cpu);
void OR(CPU *cpu);
void XOR(CPU *cpu);
void NOR(CPU *cpu);
void SLT(CPU *cpu);
void SLTU(CPU *cpu);
void DADD(CPU *cpu);
void DADDU(CPU *cpu);
void DSUB(CPU *cpu);
void DSUBU(CPU *cpu);
void TGE(CPU *cpu);
void TGEU(CPU *cpu);
void TLT(CPU *cpu);
void TLTU(CPU *cpu);
void TE(CPU *cpu);
void TNE(CPU *cpu);
void DSLL(CPU *cpu);
void DSRL(CPU *cpu);
void DSRA(CPU *cpu);
void DSLL32(CPU *cpu);
void DSRL32(CPU *cpu);
void DSRA32(CPU *cpu);

#endif
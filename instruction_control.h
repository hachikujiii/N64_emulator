#ifndef INSTRUCTION_CONTROL_H
#define INSTRUCTION_CONTROL_H

#include <stdio.h>
#include "cpu.h"

typedef enum {
    LOAD, STORE, ALU, BRANCH, JUMP, COPROC, SPECIAL, INVALID
} Instruction_Type;

typedef enum {
    NO_MEM, MEM_READ, MEM_WRITE
} Access_Type;

typedef enum {
    R_TYPE, I_TYPE, J_TYPE, REGIMM, TRAP, ERROR
} Format_Type;

typedef struct {
    Format_Type f_type;
    Instruction_Type type;
    Access_Type memAccess;
    void (*execute)(CPU *cpu);
} Instruction_Control;

typedef struct {
    uint32_t instruction;
    uint8_t opcode;
    uint8_t rs; 
    uint8_t rt;
    uint8_t rd;
    uint8_t shamt;
    uint8_t function;
    int16_t immediate;
    uint64_t rs_val;
    uint64_t rt_val;
    int32_t SEOffset;
    uint32_t branch_addr;
    uint32_t jump_addr;
} Instruction;

//OP CODES
extern Instruction_Control opcode_table[64];
Instruction decode(uint32_t inst);

//opcode_table calls
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
void instruction_exception(CPU *cpu);

#endif



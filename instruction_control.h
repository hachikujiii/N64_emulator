#ifndef INSTRUCTION_CONTROL_H
#define INSTRUCTION_CONTROL_H

#include <stdio.h>
#include <stdint.h>

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
} Access_Control;

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
Instruction decode(uint32_t inst);
extern Access_Control opcode_flags[64];

#endif

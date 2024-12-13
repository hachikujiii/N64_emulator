#ifndef CONTROL_H
#define CONTROL_H

#include <stdio.h>
#include <stdint.h>

typedef enum {
    R_TYPE, I_TYPE, J_TYPE, REGIMM, TRAP, ERROR, NOP
} Format_Type;

typedef enum {
    LOAD, STORE, ALU, BRANCH, JUMP, SPECIAL, INVALID
} Operation_Type;

typedef enum {
    NO_ACCESS, MEM_READ, MEM_WRITE, REG_READ, REG_WRITE, COPROC
} Access_Type;


typedef struct {
    Format_Type f_type;
    Operation_Type type;
    Access_Type memAccess;
} Control;

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

uint32_t byte_swap(uint32_t inst);
Instruction decode(uint32_t inst);
Control set_flags(uint8_t opcode);

#endif

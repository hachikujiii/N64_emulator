#ifndef CONTROL_H
#define CONTROL_H

#include <stdio.h>
#include <stdint.h>

typedef enum {
    R_TYPE, I_TYPE, J_TYPE, TRAP, ERROR, NOP
} Format_Type;

typedef enum {
    LOAD, STORE, ALU, BRANCH, JUMP, SPECIAL, INVALID
} Operation_Type;

typedef enum {
    NO_ACCESS, MEM_READ, MEM_WRITE, REG_READ, REG_WRITE, COPROC
} Access_Type;

typedef enum {
    ZERO, BYTE, H_WORD, UNUSED, WORD, D_WORD = 8
} Num_Bytes;

typedef struct {
    Format_Type format_type;
    Operation_Type op_type;
    Access_Type mem_access;
} Control;

uint32_t byte_swap(uint32_t inst);

#endif

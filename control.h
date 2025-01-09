#ifndef CONTROL_H
#define CONTROL_H

#include <stdio.h>
#include <stdint.h>

typedef enum {
    R_TYPE, I_TYPE, J_TYPE, TRAP, ERROR, NOP //if Itype or coproc ???
} Format_Type;

typedef enum {
    LOAD, STORE, ALU, BRANCH, JUMP, 
    COPROC,
    SPECIAL, 
    INVALID
} Operation_Type;

typedef enum {
    NO_ACCESS, 
    MEM_READ, 
    MEM_WRITE, 
    REG_READ, 
    REG_WRITE
} Access_Type;

typedef enum {
    ZERO = 0, 
    BYTE = 1, 
    H_WORD = 2, 
    WORD = 4,
    D_WORD = 8
} Num_Bytes;

typedef struct {
    Format_Type format_type;
    Operation_Type op_type;
    Access_Type mem_access;
} Control;

uint32_t byte_swap(uint32_t inst);

#endif

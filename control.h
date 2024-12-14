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

uint32_t byte_swap(uint32_t inst);

#endif

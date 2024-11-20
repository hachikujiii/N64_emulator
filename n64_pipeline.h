#ifndef N64_PIPELINE_H
#define N64_PIPELINE_H

#include <stdint.h>
#include <stdbool.h>

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

typedef struct {
    bool PCSrc;
    bool PCWrite;
    bool IDwrite;
    bool flush;
    uint32_t instruction;
} ICRF_Pipeline;

typedef struct {
    bool RegDst;
    bool ALUSrc;

    bool jump;
    bool branch;
    bool zero;
    Instruction_Control control;
    Instruction instruction;
} RFEX_Pipeline;

typedef struct {

    int ALUResult;
    int SWValue;
    int WriteRegNum;
    Instruction_Control control;
    Instruction instruction;
} EXDC_Pipeline;

typedef struct {
    bool MemToReg;
    bool RegWrite;

    int ALUResult;
    int LWDataValue;
    int WriteRegNum;
    Instruction_Control control;
    Instruction instruction;
} DCWB_Pipeline;

typedef struct {
    ICRF_Pipeline ICRF_WRITE;
    ICRF_Pipeline ICRF_READ;

    RFEX_Pipeline RFEX_WRITE;
    RFEX_Pipeline RFEX_READ;

    EXDC_Pipeline EXDC_WRITE;
    EXDC_Pipeline EXDC_READ;

    DCWB_Pipeline DCWB_WRITE;
    DCWB_Pipeline DCWB_READ;
} Pipeline;

#endif
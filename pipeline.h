#ifndef PIPELINE_H
#define PIPELINE_H

#include <stdint.h>
#include <stdbool.h>
#include "cpu.h"
#include "my_structs.h"

void IC_stage(unsigned char* rom, N64 *n64); 
void RF_stage(CPU *cpu); 
void EX_stage(N64 *n64);

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

    Instruction_Control control;
    Instruction instruction;
} RFEX_Pipeline;

typedef struct {
    uint64_t ALU_Result;
    uint64_t SW_Value;
    uint64_t Write_Reg_Num;

    Instruction_Control control;
    Instruction instruction;
} EXDC_Pipeline;

typedef struct {
    uint64_t ALU_Result;
    uint64_t LW_Data_Value;
    uint64_t Write_Reg_Num;

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
#ifndef PIPELINE_H
#define PIPELINE_H

#include "control.h"
#include <stdbool.h>

typedef struct {
    bool PCSrc;
    bool PCWrite;
    bool IDwrite;
    bool flush;
    uint32_t instruction;
} ICRF_Pipeline;

typedef struct {
    Control control;
    Instruction instruction;
    bool RegDst;
    bool ALUSrc;

} RFEX_Pipeline;

typedef struct {
    Control control;
    Instruction instruction;

    uint64_t ALU_Result;
    uint64_t SW_Value;
    uint64_t Write_Reg_Num;

} EXDC_Pipeline;

typedef struct {
    Control control;
    Instruction instruction;

    uint64_t ALU_Result;
    uint64_t LW_Data_Value;
    uint64_t Write_Reg_Num;

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

void initialize_pipeline(Pipeline *pipeline);


#endif
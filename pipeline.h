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
    uint32_t instruction;
    Control control;
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

    bool MemRead;
    bool MemToReg;
    bool MemWrite;
    bool RegWrite;
    bool RegDst;
    bool ALUSrc;

    uint8_t Read_Reg_Num;
    uint8_t Write_Reg_Num;

} RFEX_Pipeline;

typedef struct {
    Control control;
    bool MemRead;
    bool MemToReg;
    bool MemWrite;
    bool RegWrite;

    uint64_t ALU_Result;
    uint64_t SW_Value;

    uint8_t Read_Reg_Num;
    uint8_t Write_Reg_Num;

} EXDC_Pipeline;

typedef struct {
    Control control;
   
    bool MemWrite;
    bool RegWrite;
    uint64_t ALU_Result;
    uint64_t LW_Data_Value;

    uint8_t Write_Reg_Num;

} DCWB_Pipeline;

typedef struct {
    bool stall;               // Flag indicating whether the pipeline should stall
    bool forward_rs;          // Whether to forward the value of rs
    bool forward_rt;          // Whether to forward the value of rt
    bool forward_ALU_result;
    uint64_t forward_rs_val;  // Forwarded value for rs
    uint64_t forward_rt_val;  // Forwarded value for rt
    int forwarding_stage_rs;  // Stage where the forwarded rs value is coming from
    int forwarding_stage_rt;  // Stage where the forwarded rt value is coming from
} HazardControl;

typedef struct {
    ICRF_Pipeline ICRF_WRITE;
    ICRF_Pipeline ICRF_READ;

    RFEX_Pipeline RFEX_WRITE;
    RFEX_Pipeline RFEX_READ;

    EXDC_Pipeline EXDC_WRITE;
    EXDC_Pipeline EXDC_READ;

    DCWB_Pipeline DCWB_WRITE;
    DCWB_Pipeline DCWB_READ;

    HazardControl HAZARD;
} Pipeline;

void init_pipeline(Pipeline *pipeline);

#endif
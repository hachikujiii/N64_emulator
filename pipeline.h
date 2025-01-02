#ifndef PIPELINE_H
#define PIPELINE_H

#include "control.h"
#include <stdbool.h>

typedef enum {
    IC = 0,
    RF,
    EX,
    DC,
    WB
} Stage;

typedef struct {
    bool PCSrc;
    bool PCWrite;
    bool IDwrite;
    bool stall;

    uint32_t instruction;
    uint32_t delay_slot;
} ICRF_Pipeline;

typedef struct {
    uint32_t instruction;
    uint32_t delay_slot;

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

    bool stall;
    bool MemRead;
    bool MemToReg;
    bool MemWrite;
    bool RegWrite;
    bool RegDst;
    bool ALUSrc;

    uint8_t Write_Reg_Num;

} RFEX_Pipeline;

typedef struct {
    Control control;
    bool stall; 
    bool MemRead;
    bool MemToReg;
    bool MemWrite;
    bool RegWrite;
    Num_Bytes num_bytes; 

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
    bool branch;
    bool forward_ALU_result;
    bool forward_load_result;

    Stage stage;
    int stall_count;

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
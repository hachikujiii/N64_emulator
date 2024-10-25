#ifndef N64_PIPELINE_H
#define N64_PIPELINE_H

#include <stdint.h>

typedef struct {
    bool PCSrc;
    bool PCWrite;
    bool IDwrite;
    bool flush;
    uint64_t instruction;

    //need control units for forwarding mem/wb to ex, hazard detection unit, pipeline stall, more?
} ICRF_Pipeline;

typedef struct {
    bool RegDst;
    bool ALUSrc;
    bool MemRead;
    bool MemToReg;
    bool MemWrite;
    bool RegWrite;
    int ALUOp;
    bool jump;
    bool branch;
    bool zero;
    
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
    uint32_t targetaddr;
} RFEX_Pipeline;

typedef struct {
    bool MemRead;
    bool MemWrite;
    bool MemToReg;
    bool RegWrite;

    int ALUResult;
    int SWValue;
    int WriteRegNum;
} EXDC_Pipeline;

typedef struct {
    bool MemToReg;
    bool RegWrite;

    int ALUResult;
    int LWDataValue;
    int WriteRegNum;
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
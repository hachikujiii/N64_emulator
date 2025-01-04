#include "pipeline.h"
#include <string.h>

void init_pipeline(Pipeline *pipeline) {
    
    memset(&pipeline->ICRF_WRITE, 0, sizeof(ICRF_Pipeline));
    memset(&pipeline->ICRF_READ, 0, sizeof(ICRF_Pipeline));

    memset(&pipeline->RFEX_WRITE, 0, sizeof(RFEX_Pipeline));
    memset(&pipeline->RFEX_READ, 0, sizeof(RFEX_Pipeline));

    memset(&pipeline->EXDC_WRITE, 0, sizeof(EXDC_Pipeline));
    memset(&pipeline->EXDC_READ, 0, sizeof(EXDC_Pipeline));

    memset(&pipeline->DCWB_WRITE, 0, sizeof(DCWB_Pipeline));
    memset(&pipeline->DCWB_READ, 0, sizeof(DCWB_Pipeline));

    pipeline->RFEX_WRITE.control.format_type = NOP;
    pipeline->RFEX_WRITE.control.op_type = INVALID;
    pipeline->RFEX_WRITE.control.mem_access = NO_ACCESS;
    pipeline->RFEX_READ.control.format_type = NOP;
    pipeline->RFEX_READ.control.op_type = INVALID;
    pipeline->RFEX_READ.control.mem_access = NO_ACCESS;

    pipeline->EXDC_WRITE.control.format_type = NOP;
    pipeline->EXDC_WRITE.control.op_type = INVALID;
    pipeline->EXDC_WRITE.control.mem_access = NO_ACCESS;
    pipeline->EXDC_READ.control.format_type = NOP;
    pipeline->EXDC_READ.control.op_type = INVALID;
    pipeline->EXDC_READ.control.mem_access = NO_ACCESS;

    pipeline->DCWB_WRITE.control.format_type = NOP;
    pipeline->DCWB_WRITE.control.op_type = INVALID;
    pipeline->DCWB_WRITE.control.mem_access = NO_ACCESS;
    pipeline->DCWB_READ.control.format_type = NOP;
    pipeline->DCWB_READ.control.op_type = INVALID;
    pipeline->DCWB_READ.control.mem_access = NO_ACCESS;
    return;
}  

void insert_nop(Pipeline *pipeline, Stage stage) {

    Control nop_con;
    nop_con.format_type = NOP;
    nop_con.mem_access = NO_ACCESS;
    nop_con.op_type = INVALID;

    switch(stage) {
        case IC:
            pipeline->ICRF_WRITE.instruction = 0;
            break;
        case RF:
            pipeline->RFEX_WRITE.control = nop_con;
            pipeline->RFEX_WRITE.instruction = 0;
            pipeline->RFEX_WRITE.opcode = 0;
            pipeline->RFEX_WRITE.rs = 0; 
            pipeline->RFEX_WRITE.rt = 0;
            pipeline->RFEX_WRITE.rd = 0;
            pipeline->RFEX_WRITE.shamt = 0;
            pipeline->RFEX_WRITE.function = 0;
            pipeline->RFEX_WRITE.immediate = 0;
            pipeline->RFEX_WRITE.rs_val = 0;
            pipeline->RFEX_WRITE.rt_val = 0;
            pipeline->RFEX_WRITE.SEOffset = 0;
            pipeline->RFEX_WRITE.branch_addr = 0;
            pipeline->RFEX_WRITE.jump_addr = 0;
            
            pipeline->RFEX_WRITE.RegDst = 0;
            pipeline->RFEX_WRITE.MemRead = 0;
            pipeline->RFEX_WRITE.MemToReg = 0;
            pipeline->RFEX_WRITE.MemWrite = 0;
            pipeline->RFEX_WRITE.RegWrite = 0;
            pipeline->RFEX_WRITE.ALUSrc = 0;
            pipeline->RFEX_WRITE.Write_Reg_Num = 0;
            break;
        case EX:
            pipeline->EXDC_WRITE.MemRead = 0;
            pipeline->EXDC_WRITE.MemToReg = 0;
            pipeline->EXDC_WRITE.MemWrite = 0;
            pipeline->EXDC_WRITE.RegWrite = 0;
            pipeline->EXDC_WRITE.ALU_Result = 0;
            pipeline->EXDC_WRITE.Write_Reg_Num = 0;
            pipeline->EXDC_WRITE.control = nop_con;
            break;
        case DC:
            break;
        case WB:
            break;
    }
}

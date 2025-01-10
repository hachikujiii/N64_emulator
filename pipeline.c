#include "pipeline.h"
#include <string.h>

void init_pipeline(Pipeline *pipeline) {

    memset(pipeline->HAZARD.reg_status, 0, sizeof(pipeline->HAZARD.reg_status));
    
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

void load_hazard_detection(Pipeline *pipeline) {

    if(pipeline->RFEX_READ.MemToReg) {

        //HANDLES LOAD HAZARDS BY STALLING IC AND RF STAGES FOR ONE CYCLE
        if((pipeline->RFEX_WRITE.rs == pipeline->RFEX_READ.Write_Reg_Num) || 
           (pipeline->RFEX_WRITE.rt == pipeline->RFEX_READ.Write_Reg_Num)) {
            pipeline->HAZARD.stall = 1;
            pipeline->HAZARD.delay_IC_RF_copy = 1;
            pipeline->HAZARD.delay_copy_count = 1;
            pipeline->HAZARD.stall_count = EX;
            return;
        }
    }
}

void check_war_forwarding(Pipeline *pipeline, Format_Type type) {

    switch(type) {

        case I_TYPE:

            if ((pipeline->RFEX_WRITE.rs == pipeline->DCWB_READ.Write_Reg_Num) && (pipeline->DCWB_READ.control.op_type == LOAD)) {
                pipeline->RFEX_WRITE.rs_val = pipeline->DCWB_READ.LW_Data_Value;  // Forward the result directly
            } else if (pipeline->RFEX_WRITE.rs == pipeline->DCWB_READ.Write_Reg_Num) {
                pipeline->RFEX_WRITE.rs_val = pipeline->DCWB_READ.ALU_Result;  // Forward the result directly
            }
            break;

        case R_TYPE:
            if (pipeline->RFEX_WRITE.rs == pipeline->DCWB_READ.Write_Reg_Num) {
                pipeline->RFEX_WRITE.rs_val = pipeline->DCWB_READ.ALU_Result;
            }
            if (pipeline->RFEX_WRITE.rt == pipeline->DCWB_READ.Write_Reg_Num) {
                pipeline->RFEX_WRITE.rt_val = pipeline->DCWB_READ.ALU_Result;
            }
            break;

        default:
            printf("error in war forwarding\n");
            exit(1);
    }
    
}

void check_data_forwarding(Pipeline *pipeline) {

    uint8_t rs = pipeline->RFEX_READ.rs;
    uint8_t rt = pipeline->RFEX_READ.rt;

    //check current instruction for data forwarding
    if(pipeline->RFEX_READ.control.format_type == R_TYPE) {

        if(pipeline->HAZARD.reg_status[rt].is_dirty) {
            pipeline->RFEX_READ.rt_val = forward_data(pipeline, rt);
        }
    }

    if(pipeline->HAZARD.reg_status[rs].is_dirty) {
        pipeline->RFEX_READ.rs_val = forward_data(pipeline, rs);
    }
    
}


uint64_t forward_data(Pipeline *pipeline, uint8_t reg_num) {

    //check if (DC)EX->EX (except if its a load)
    if(reg_num == pipeline->EXDC_READ.Write_Reg_Num) {
        return pipeline->EXDC_READ.ALU_Result;
        
    //check if (WB)DC->EX
    } else if(reg_num == pipeline->DCWB_READ.Write_Reg_Num) {

        if(pipeline->DCWB_READ.control.op_type == LOAD) {
            return pipeline->DCWB_READ.LW_Data_Value;

        } else {
            return pipeline->DCWB_READ.ALU_Result;
        }

    } 

}

void ReadToWrite(Pipeline *pipeline) {

    if (pipeline->HAZARD.delay_IC_RF_copy) {

        pipeline->HAZARD.delay_copy_count--;
        if(pipeline->HAZARD.delay_copy_count == 0) {
            pipeline->HAZARD.delay_IC_RF_copy = 0;
        }

    } else {
        pipeline->ICRF_READ.instruction = pipeline->ICRF_WRITE.instruction;
        pipeline->ICRF_READ.delay_slot = pipeline->ICRF_WRITE.delay_slot;

        pipeline->RFEX_READ.instruction = pipeline->RFEX_WRITE.instruction;
        pipeline->RFEX_READ.delay_slot = pipeline->RFEX_WRITE.delay_slot;
        pipeline->RFEX_READ.control = pipeline->RFEX_WRITE.control;
        pipeline->RFEX_READ.opcode = pipeline->RFEX_WRITE.opcode; 
        pipeline->RFEX_READ.rs = pipeline->RFEX_WRITE.rs;
        pipeline->RFEX_READ.rt = pipeline->RFEX_WRITE.rt;
        pipeline->RFEX_READ.rd = pipeline->RFEX_WRITE.rd;
        pipeline->RFEX_READ.shamt = pipeline->RFEX_WRITE.shamt;
        pipeline->RFEX_READ.function = pipeline->RFEX_WRITE.function;
        pipeline->RFEX_READ.immediate = pipeline->RFEX_WRITE.immediate;
        pipeline->RFEX_READ.SEOffset = pipeline->RFEX_WRITE.SEOffset;
        pipeline->RFEX_READ.rs_val = pipeline->RFEX_WRITE.rs_val;
        pipeline->RFEX_READ.rt_val = pipeline->RFEX_WRITE.rt_val;
        pipeline->RFEX_READ.branch_addr = pipeline->RFEX_WRITE.branch_addr;
        pipeline->RFEX_READ.jump_addr = pipeline->RFEX_WRITE.jump_addr;
        pipeline->RFEX_READ.MemRead = pipeline->RFEX_WRITE.MemRead;
        pipeline->RFEX_READ.MemToReg = pipeline->RFEX_WRITE.MemToReg;
        pipeline->RFEX_READ.MemWrite = pipeline->RFEX_WRITE.MemWrite;
        pipeline->RFEX_READ.RegWrite = pipeline->RFEX_WRITE.RegWrite;
        pipeline->RFEX_READ.RegDst = pipeline->RFEX_WRITE.RegDst;
        pipeline->RFEX_READ.Write_Reg_Num = pipeline->RFEX_WRITE.Write_Reg_Num;
    }

    pipeline->EXDC_READ.control = pipeline->EXDC_WRITE.control;
    pipeline->EXDC_READ.ALU_Result = pipeline->EXDC_WRITE.ALU_Result;
    pipeline->EXDC_READ.SW_Value = pipeline->EXDC_WRITE.SW_Value;
    pipeline->EXDC_READ.MemRead = pipeline->EXDC_WRITE.MemRead;
    pipeline->EXDC_READ.MemToReg = pipeline->EXDC_WRITE.MemToReg;
    pipeline->EXDC_READ.MemWrite = pipeline->EXDC_WRITE.MemWrite;
    pipeline->EXDC_READ.RegWrite = pipeline->EXDC_WRITE.RegWrite;
    pipeline->EXDC_READ.Read_Reg_Num = pipeline->EXDC_WRITE.Read_Reg_Num;
    pipeline->EXDC_READ.Write_Reg_Num = pipeline->EXDC_WRITE.Write_Reg_Num;
    pipeline->EXDC_READ.num_bytes = pipeline->EXDC_WRITE.num_bytes;


    pipeline->DCWB_READ.control = pipeline->DCWB_WRITE.control;
    pipeline->DCWB_READ.ALU_Result = pipeline->DCWB_WRITE.ALU_Result;
    pipeline->DCWB_READ.LW_Data_Value = pipeline->DCWB_WRITE.LW_Data_Value;
    pipeline->DCWB_READ.Write_Reg_Num = pipeline->DCWB_WRITE.Write_Reg_Num;
    pipeline->DCWB_READ.RegWrite = pipeline->DCWB_WRITE.RegWrite;
    pipeline->DCWB_READ.MemWrite = pipeline->DCWB_WRITE.MemWrite;

}
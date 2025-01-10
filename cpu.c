#include "cpu.h"
#include "control.h"

uint32_t fetch_instruction(CPU *cpu) {

    return cpu_bus_request(cpu->mmu, cpu->PC, 0, false);
}

void decode(Pipeline *pipeline) {

    uint32_t inst = pipeline->ICRF_READ.instruction; 
    pipeline->RFEX_WRITE.opcode = (inst >> 26) & 0x3F;
    pipeline->RFEX_WRITE.rs = (inst >> 21) & 0x1F;
    pipeline->RFEX_WRITE.rt = (inst >> 16) & 0x1F;
    pipeline->RFEX_WRITE.rd = (inst >> 11) & 0x1F;
    pipeline->RFEX_WRITE.shamt = (inst >> 6) & 0x1F;
    pipeline->RFEX_WRITE.function = inst & 0x3F;
    pipeline->RFEX_WRITE.immediate = inst & 0xFFFF;
    pipeline->RFEX_WRITE.SEOffset = (int32_t)pipeline->RFEX_WRITE.immediate;
    pipeline->RFEX_WRITE.delay_slot = pipeline->ICRF_READ.delay_slot;
    pipeline->RFEX_WRITE.instruction = inst;

    set_flags(pipeline);
    
    if(pipeline->RFEX_WRITE.control.format_type == R_TYPE) {
        
        pipeline->RFEX_WRITE.RegDst = 1;
        pipeline->RFEX_WRITE.MemRead = 0;
        pipeline->RFEX_WRITE.MemToReg = 0;
        pipeline->RFEX_WRITE.MemWrite = 0;
        pipeline->RFEX_WRITE.ALUSrc = 0;
        pipeline->RFEX_WRITE.RegWrite = 1;
        pipeline->RFEX_WRITE.Write_Reg_Num = pipeline->RFEX_WRITE.rd;

    } else if(pipeline->RFEX_WRITE.control.format_type == I_TYPE) {

        if(pipeline->RFEX_WRITE.control.op_type == LOAD) {
            pipeline->RFEX_WRITE.RegDst = 0;
            pipeline->RFEX_WRITE.MemRead = 1;
            pipeline->RFEX_WRITE.MemToReg = 1;
            pipeline->RFEX_WRITE.MemWrite = 0;
            pipeline->RFEX_WRITE.ALUSrc = 1;
            pipeline->RFEX_WRITE.RegWrite = 1;
            pipeline->RFEX_WRITE.Write_Reg_Num = pipeline->RFEX_WRITE.rt;

        } else if(pipeline->RFEX_WRITE.control.op_type == STORE) {
            pipeline->RFEX_WRITE.RegDst = 0;
            pipeline->RFEX_WRITE.MemRead = 0;
            pipeline->RFEX_WRITE.MemToReg = 0;
            pipeline->RFEX_WRITE.MemWrite = 1;
            pipeline->RFEX_WRITE.ALUSrc = 1;
            pipeline->RFEX_WRITE.RegWrite = 0;

        } else if(pipeline->RFEX_WRITE.control.op_type == ALU) {
            pipeline->RFEX_WRITE.RegDst = 0;
            pipeline->RFEX_WRITE.MemRead = 0;
            pipeline->RFEX_WRITE.MemToReg = 0;
            pipeline->RFEX_WRITE.MemWrite = 0;
            pipeline->RFEX_WRITE.ALUSrc = 0;
            pipeline->RFEX_WRITE.RegWrite = 1;
            pipeline->RFEX_WRITE.Write_Reg_Num = pipeline->RFEX_WRITE.rt;  

        } else if(pipeline->RFEX_WRITE.control.op_type == COPROC) {

            //MFCO rt = gpr, rd = cp0 NO SOURCE
            //MTC0 rt = gpr, rd = cp0 NO SOURCE
            pipeline->RFEX_WRITE.rd += 32;

            pipeline->RFEX_WRITE.RegDst = 0;
            pipeline->RFEX_WRITE.MemRead = 0;
            pipeline->RFEX_WRITE.MemToReg = 0;
            pipeline->RFEX_WRITE.MemWrite = 0;
            pipeline->RFEX_WRITE.ALUSrc = 0;
            pipeline->RFEX_WRITE.RegWrite = 1;
            pipeline->RFEX_WRITE.Write_Reg_Num = pipeline->RFEX_WRITE.rd;
            return;
        }

    } else if(pipeline->RFEX_WRITE.control.format_type == J_TYPE) {

            if(pipeline->RFEX_WRITE.control.op_type == BRANCH || pipeline->RFEX_WRITE.control.op_type == JUMP) {
                pipeline->RFEX_WRITE.RegDst = 0;
                pipeline->RFEX_WRITE.MemRead = 0;
                pipeline->RFEX_WRITE.MemToReg = 0;
                pipeline->RFEX_WRITE.MemWrite = 0;
                pipeline->RFEX_WRITE.ALUSrc = 0;
                pipeline->RFEX_WRITE.RegWrite = 0;
                pipeline->RFEX_WRITE.Write_Reg_Num = 0;
            }

        pipeline->RFEX_WRITE.branch_addr = pipeline->RFEX_WRITE.delay_slot + (((int64_t) pipeline->RFEX_WRITE.immediate) << 2);
        //cpu->pipeline.RFEX_WRITE.jump_addr = cpu->pipeline.RFEX_READ.instruction.instruction & 0x03FFFFFF; //fix this
    } else {
        printf("NOT CP0 RTYPE, ITYPE OR ALU\n");
        exit(EXIT_FAILURE);
    }

    
}

void set_flags(Pipeline *pipeline) {
    pipeline->RFEX_WRITE.control = flags[pipeline->RFEX_WRITE.opcode];
}

void load_reg_data(CPU *cpu) {

    

    if(cpu->pipeline.RFEX_READ.control.format_type != I_TYPE) {
        uint64_t rt_val = cpu->regs[cpu->pipeline.RFEX_READ.rt];
        cpu->pipeline.RFEX_READ.rt_val = rt_val;
        check_data_forwarding(&cpu->pipeline);
    }

    uint64_t rs_val = cpu->regs[cpu->pipeline.RFEX_READ.rs];
    cpu->pipeline.RFEX_READ.rs_val = rs_val;
    check_data_forwarding(&cpu->pipeline);

    if(cpu->pipeline.RFEX_READ.RegWrite) {
        uint8_t write_reg_num = cpu->pipeline.RFEX_READ.Write_Reg_Num;
        cpu->pipeline.HAZARD.reg_status[write_reg_num].is_dirty = 1;
        cpu->pipeline.HAZARD.reg_status[write_reg_num].count_write++;
    }
}


void IC_stage(CPU *cpu) {

    if(cpu->pipeline.HAZARD.stall) {
        cpu->pipeline.HAZARD.stall_count--;
        return;
    }
    //HAZARD FOR BRANCH CHECK BEFORE ANYTHING

    cpu->pipeline.ICRF_WRITE.instruction = fetch_instruction(cpu);
    cpu->pipeline.ICRF_WRITE.delay_slot = cpu->PC + 4;
    cpu->PC += 4;
    //else
        //fetch)instruction using delay slot and set PC to branch addr
}

void RF_stage(CPU *cpu) {

    if(cpu->pipeline.HAZARD.stall) {
        cpu->pipeline.HAZARD.stall_count--;
        return;
    }

    if(cpu->pipeline.ICRF_READ.instruction == 0) {
        insert_nop(&cpu->pipeline, RF);
        return;
    }
    
    decode(&cpu->pipeline);

    load_hazard_detection(&cpu->pipeline);

}

void EX_stage(CPU *cpu) {
    
    if(cpu->pipeline.RFEX_READ.control.format_type == NOP) {
        insert_nop(&cpu->pipeline, EX);
        return;
    }

    //MAKE PASS SIGNALS FUNCTION
    cpu->pipeline.EXDC_WRITE.control = cpu->pipeline.RFEX_READ.control;
    cpu->pipeline.EXDC_WRITE.MemRead = cpu->pipeline.RFEX_READ.MemRead;
    cpu->pipeline.EXDC_WRITE.MemWrite = cpu->pipeline.RFEX_READ.MemWrite;
    cpu->pipeline.EXDC_WRITE.RegWrite = cpu->pipeline.RFEX_READ.RegWrite;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.Write_Reg_Num;


    if((cpu->pipeline.HAZARD.stall) && (cpu->pipeline.HAZARD.stall_count == 0)) {
        insert_nop(&cpu->pipeline, EX);
        cpu->pipeline.HAZARD.stall = 0;
        cpu->pipeline.HAZARD.stall_count = 0;
        return;
    }

    

    load_reg_data(cpu);

    execute_instruction(cpu);
}

void DC_stage(CPU *cpu) {

    if(cpu->pipeline.EXDC_READ.control.format_type == NOP) {
        cpu->pipeline.DCWB_WRITE.MemWrite = 0;
        cpu->pipeline.DCWB_WRITE.RegWrite = 0;
        cpu->pipeline.DCWB_WRITE.ALU_Result = 0;
        cpu->pipeline.DCWB_WRITE.Write_Reg_Num = 0;
        return;
    }

    cpu->pipeline.DCWB_WRITE.control = cpu->pipeline.EXDC_READ.control;
    cpu->pipeline.DCWB_WRITE.MemWrite = cpu->pipeline.EXDC_READ.MemWrite;
    cpu->pipeline.DCWB_WRITE.RegWrite = cpu->pipeline.EXDC_READ.RegWrite;
    cpu->pipeline.DCWB_WRITE.ALU_Result = cpu->pipeline.EXDC_READ.ALU_Result;


    if(cpu->pipeline.DCWB_WRITE.control.mem_access == NO_ACCESS){

        cpu->pipeline.DCWB_WRITE.Write_Reg_Num = cpu->pipeline.EXDC_READ.Write_Reg_Num;
        cpu->pipeline.DCWB_WRITE.ALU_Result = cpu->pipeline.EXDC_READ.ALU_Result;

        if(cpu->pipeline.DCWB_WRITE.control.op_type == COPROC) {

            if(cpu->pipeline.DCWB_WRITE.RegWrite) {
                cpu->regs[cpu->pipeline.DCWB_WRITE.Write_Reg_Num] = cpu->pipeline.DCWB_WRITE.ALU_Result;
            }
            //TODO: if reg_read ()
        }

        //TODO: if cp1 ....

    } else if(cpu->pipeline.DCWB_WRITE.control.mem_access == MEM_READ) {

        if(cpu->pipeline.DCWB_WRITE.control.op_type == LOAD) {

            uint32_t target_addr = cpu->pipeline.EXDC_READ.ALU_Result;
            cpu->pipeline.DCWB_WRITE.LW_Data_Value = cpu_bus_request(cpu->mmu, target_addr, 0, false);

            cpu->pipeline.DCWB_WRITE.Write_Reg_Num = cpu->pipeline.EXDC_READ.Write_Reg_Num;
            cpu->pipeline.DCWB_WRITE.ALU_Result = cpu->pipeline.EXDC_READ.ALU_Result;
        }
        
    } else if (cpu->pipeline.DCWB_WRITE.control.mem_access == MEM_WRITE) {

        if(cpu->pipeline.DCWB_WRITE.control.op_type == STORE) {
            uint32_t target_addr = cpu->pipeline.EXDC_READ.ALU_Result;
            uint32_t word = cpu->pipeline.EXDC_READ.SW_Value;

            cpu->pipeline.DCWB_WRITE.LW_Data_Value = cpu_bus_request(cpu->mmu, target_addr, word, true);
        }
    }
    
    //TODO: HANDLE MEM AND TLB :(
}

void WB_stage(CPU *cpu) {

    if(cpu->pipeline.DCWB_READ.control.format_type == NOP) {
        return;
    }

    uint64_t result = cpu->pipeline.DCWB_READ.ALU_Result;
    uint64_t lw_value = cpu->pipeline.DCWB_READ.LW_Data_Value;
    int writeReg = cpu->pipeline.DCWB_READ.Write_Reg_Num;
        
    if(cpu->pipeline.DCWB_READ.RegWrite) {

        if(cpu->pipeline.DCWB_READ.control.format_type == R_TYPE) {

            if (writeReg == REG_HI) {
                cpu->regs[REG_HI] = result;  // Write to HI register
            } else if (writeReg == REG_LO) {
                cpu->regs[REG_LO] = result;
            } else {
                cpu->regs[writeReg] = result;
            }

            check_war_forwarding(&cpu->pipeline, R_TYPE);

        } else if(cpu->pipeline.DCWB_READ.control.format_type == I_TYPE) {
   
            if(cpu->pipeline.DCWB_READ.control.op_type == LOAD) {
                cpu->regs[writeReg] = lw_value;
            } else if((cpu->pipeline.DCWB_READ.control.op_type == STORE) ||
                (cpu->pipeline.DCWB_READ.control.op_type == COPROC)){
                    //do nothing
            } else {
                cpu->regs[writeReg] = result;
            }

            check_war_forwarding(&cpu->pipeline, I_TYPE);
        }

        if(cpu->pipeline.HAZARD.reg_status[writeReg].is_dirty && 
           cpu->pipeline.HAZARD.reg_status[writeReg].count_write == 1) {

            cpu->pipeline.HAZARD.reg_status[writeReg].is_dirty = false;
            cpu->pipeline.HAZARD.reg_status[writeReg].count_write = 0;

        } else if (cpu->pipeline.HAZARD.reg_status[writeReg].is_dirty && 
                   cpu->pipeline.HAZARD.reg_status[writeReg].count_write > 1) {

            cpu->pipeline.HAZARD.reg_status[writeReg].count_write--;
        }
    }
}




void instruction_exception(CPU *cpu) {
    printf("instruction error\n");
    exit(EXIT_FAILURE);
}

void execute_instruction(CPU *cpu) {

    opcode_table[cpu->pipeline.RFEX_READ.opcode](cpu);
}

void R_FORMAT(CPU *cpu) { 
    uint8_t function = cpu->pipeline.RFEX_READ.function;

    switch(function) {
        case 0x08:
    }
}
//op code instructions for EX pipeline
void J(CPU *cpu) {

    uint32_t delay_slot = cpu->pipeline.RFEX_READ.delay_slot;

    uint32_t j_target = cpu->pipeline.RFEX_READ.instruction & 0x03FFFFFF; //make this a separate field that gets masked if its a jump instruction
    j_target = j_target << 2;
    cpu->PC = (delay_slot & 0xF0000000) | j_target;
}

void JAL(CPU* cpu) {

    uint32_t delay_slot = cpu->pipeline.RFEX_READ.delay_slot;
    cpu->regs[31] = delay_slot + 4;
    
    uint32_t j_target = cpu->pipeline.RFEX_READ.instruction & 0x03FFFFFF;
    j_target = j_target << 2;
    cpu->PC = (delay_slot & 0xF0000000) | j_target;
}

void BEQ(CPU *cpu) {

    uint32_t delay_slot = cpu->pipeline.RFEX_READ.delay_slot;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.immediate;
    uint32_t b_target = ((extended_offset << 2) + delay_slot);

    if(cpu->regs[cpu->pipeline.RFEX_READ.rs] == cpu->regs[cpu->pipeline.RFEX_READ.rt]) {
        cpu->PC = cpu->PC + b_target;
    }
}

void BNEQ(CPU *cpu) {

    uint32_t delay_slot = cpu->pipeline.RFEX_READ.delay_slot;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.immediate;
    uint32_t b_target = ((extended_offset << 2) + delay_slot);

    if(cpu->regs[cpu->pipeline.RFEX_READ.rs] != cpu->regs[cpu->pipeline.RFEX_READ.rt]) {
        cpu->PC = cpu->PC + b_target;
    }
}

void BLEZ(CPU *cpu) {
    
    uint32_t delay_slot = cpu->pipeline.RFEX_READ.delay_slot;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.immediate;
    uint32_t b_target = ((extended_offset << 2) + delay_slot);

    if(cpu->regs[cpu->pipeline.RFEX_READ.rs] <= 0) {
        cpu->PC = cpu->PC + b_target;
    }
}

void BGTZ(CPU *cpu) {
    
    uint32_t delay_slot = cpu->pipeline.RFEX_READ.delay_slot;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.immediate;
    uint32_t b_target = ((extended_offset << 2) + delay_slot);

    if(cpu->regs[cpu->pipeline.RFEX_READ.rs] > 0) {
        cpu->PC = cpu->PC + b_target;
    }
}

void ADDI(CPU *cpu) {
    
    int64_t SE_Immediate = (int64_t)cpu->pipeline.RFEX_READ.immediate;
    int64_t rs_val = (int64_t)cpu->pipeline.RFEX_READ.rs_val;
    int64_t result = rs_val + SE_Immediate;

    //TODO: handle int overflow...
    if ((rs_val > 0 && SE_Immediate > 0 && result < 0) || 
        (rs_val < 0 && SE_Immediate < 0 && result > 0)) {
        //handle_overflow_exception(cpu);
        printf("addi overflow :(\n");
        exit(EXIT_FAILURE);
    }
    cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;                  
}

void ADDIU(CPU *cpu) {

    int16_t immediate = cpu->pipeline.RFEX_READ.immediate;
    int64_t SE_Immediate = (int64_t)immediate;
    int64_t rs_val = (int64_t)cpu->pipeline.RFEX_READ.rs_val;
    int64_t result = rs_val + SE_Immediate;

    cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result; 
}

void SLTI(CPU *cpu) {

    int64_t SE_Immediate = (int64_t)cpu->pipeline.RFEX_READ.immediate;
    int64_t rs_val = (int64_t)cpu->pipeline.RFEX_READ.rs_val;
    if(rs_val < SE_Immediate) {
        cpu->pipeline.EXDC_WRITE.ALU_Result = 1;
    } else {
        cpu->pipeline.EXDC_WRITE.ALU_Result = 0;
    }
}

void SLTIU(CPU *cpu) {

    int64_t SE_Immediate = (int64_t)cpu->pipeline.RFEX_READ.immediate;
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;

    if(rs_val < (uint64_t)SE_Immediate) {
        cpu->pipeline.EXDC_WRITE.ALU_Result = 1;
    } else {
        cpu->pipeline.EXDC_WRITE.ALU_Result = 0;
    }
}

void ANDI(CPU *cpu) {

    uint64_t ZE_Immediate = (uint64_t)cpu->pipeline.RFEX_READ.immediate;
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t result = rs_val & ZE_Immediate;

    cpu->pipeline.EXDC_WRITE.ALU_Result = result;   
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rt;
    cpu->pipeline.EXDC_WRITE.RegWrite = 1;
}

void ORI(CPU *cpu) {

    uint64_t ZE_Immediate = (uint16_t)cpu->pipeline.RFEX_READ.immediate;
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t result = rs_val | ZE_Immediate;

    cpu->pipeline.EXDC_WRITE.ALU_Result = result; 
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rt;
    cpu->pipeline.EXDC_WRITE.RegWrite = 1;
}

void XORI(CPU *cpu) {

    uint64_t ZE_Immediate = (uint64_t)cpu->pipeline.RFEX_READ.immediate;
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t result = rs_val ^ ZE_Immediate;

    cpu->pipeline.EXDC_WRITE.ALU_Result = result; 
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rt;
    cpu->pipeline.EXDC_WRITE.RegWrite = 1;
}

void LUI(CPU *cpu) {

    int32_t immediate = cpu->pipeline.RFEX_READ.immediate;
    int64_t SE_immediate = (int64_t)immediate << 16;
    cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)SE_immediate;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rt;
    cpu->pipeline.EXDC_WRITE.RegWrite = 1;
}

void MTC0(CPU *cpu) {

    cpu->pipeline.EXDC_WRITE.ALU_Result = cpu->pipeline.RFEX_READ.rt_val;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
    cpu->pipeline.EXDC_WRITE.RegWrite = 1;
}

void MTC1(CPU *cpu) {

}

void BEQL(CPU *cpu) {

    uint32_t delay_slot = cpu->pipeline.RFEX_READ.delay_slot;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.immediate;
    uint32_t b_target = ((extended_offset << 2) + delay_slot);

    if(cpu->regs[cpu->pipeline.RFEX_READ.rs] == cpu->regs[cpu->pipeline.RFEX_READ.rt]) {

        cpu->pipeline.ICRF_WRITE.instruction = 0;
        cpu->PC = cpu->PC + b_target;
    }
}

void BNEL(CPU *cpu) {

    uint32_t delay_slot = cpu->pipeline.RFEX_READ.delay_slot;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.SEOffset << 2;
    uint32_t b_target = extended_offset + delay_slot;

    if(cpu->pipeline.RFEX_READ.rs_val != cpu->pipeline.RFEX_READ.rt_val) {
        cpu->PC = cpu->PC + b_target;
    }
}

void BLEZL(CPU *cpu) {

    uint32_t delay_slot = cpu->pipeline.RFEX_READ.delay_slot;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.SEOffset;
    uint32_t b_target = ((extended_offset << 2) + delay_slot);

    if(cpu->regs[cpu->pipeline.RFEX_READ.rs] <= 0) {
        cpu->PC = cpu->PC + b_target;
    }
}

void BGTZL(CPU *cpu) {

    uint32_t delay_slot = cpu->pipeline.RFEX_READ.delay_slot;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.immediate;
    uint32_t b_target = ((extended_offset << 2) + delay_slot);

    if(cpu->regs[cpu->pipeline.RFEX_READ.rs] > 0) {
        cpu->PC = cpu->PC + b_target;
    }
}

void DADDI(CPU *cpu) {

    int64_t SE_Immediate = (int64_t)cpu->pipeline.RFEX_READ.immediate;
    int64_t result = SE_Immediate + cpu->regs[cpu->pipeline.RFEX_READ.rs];
    if((result > INT64_MAX) || (result < INT64_MIN)) {
    /*handle errors and exceptions*/
    } else {
        cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
    }
}

void DADDIU(CPU *cpu) {

    int64_t SE_Immediate = (int64_t)cpu->pipeline.RFEX_READ.immediate;
    int64_t result = SE_Immediate + cpu->regs[cpu->pipeline.RFEX_READ.rs];
    
    cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
}

void LDL(CPU *cpu) {

    //incomplete until i know i need to finish it
    //int64_t SE_Offset = (int64_t)cpu->pipeline.RFEX_READ.instruction.SEOffset;
    //uint64_t base = cpu->regs[cpu->pipeline.RFEX_READ.instruction.rs]; //should already have this in rfex read
    //int64_t target = base + SE_Offset;

    //int byte_offset = target % 8; 
    //int bytes_to_load = 8 - byte_offset;
    //this is where ALU_Result would get stored!
    //cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
    printf("LDL HAPPENING LOL\n");
}

void LDR(CPU *cpu) {

    //cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
    //incomplete until i know i need to finish it
    printf("LDR HAPPENING LOL\n");
}

void LB(CPU *cpu) {
    int64_t SEOffset = (int64_t)cpu->pipeline.RFEX_READ.immediate;
    uint64_t rs_data = cpu->pipeline.RFEX_READ.rs_val;
    cpu->pipeline.EXDC_WRITE.ALU_Result = rs_data + SEOffset;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rt;
}

void LH(CPU *cpu) {

    int64_t SEOffset = (int64_t)cpu->pipeline.RFEX_READ.immediate;
    uint64_t rs_data = cpu->pipeline.RFEX_READ.rs_val;
    cpu->pipeline.EXDC_WRITE.ALU_Result = rs_data + SEOffset;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rt;

    //if least significant bit of alu_result is not zero, address error exception occurs
}

void LWL(CPU *cpu) {

    //cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
    printf("you havent done this instruction yet!\n");
}

void LW(CPU *cpu) {

    int64_t SEOffset = (int64_t)cpu->pipeline.RFEX_READ.immediate;
    uint64_t rs_data = cpu->pipeline.RFEX_READ.rs_val;
    cpu->pipeline.EXDC_WRITE.ALU_Result = rs_data + SEOffset;
    cpu->pipeline.EXDC_WRITE.num_bytes = WORD;

    if((cpu->pipeline.EXDC_WRITE.ALU_Result & 0x3) != 0) {
        printf("address exception for LW\n");
        exit(EXIT_FAILURE);
    }
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rt;
}
void LBU(CPU *cpu) {

    //cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
    printf("you havent done this instruction yet!\n");
}

void LHU(CPU *cpu) {

    //cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
    printf("you havent done this instruction yet!\n");
}

void LWR(CPU *cpu) {

    //cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
    printf("you havent done this instruction yet!\n");
}

void LWU(CPU *cpu) {

   // cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
    printf("you havent done this instruction yet!\n");
}

void SB(CPU *cpu) {

    cpu->pipeline.EXDC_WRITE.SW_Value = cpu->pipeline.RFEX_READ.rt_val;
    printf("you havent done this instruction yet!\n");
}

void SH(CPU *cpu) {

    cpu->pipeline.EXDC_WRITE.SW_Value = cpu->pipeline.RFEX_READ.rt_val;
    printf("you havent done this instruction yet!\n");
}

void SWL(CPU *cpu) {

    cpu->pipeline.EXDC_WRITE.SW_Value = cpu->pipeline.RFEX_READ.rt_val;
    printf("you havent done this instruction yet!\n");
}

void SW(CPU *cpu) {

    int64_t SEOffset = (int64_t)cpu->pipeline.RFEX_READ.immediate;
    uint64_t rs_data = cpu->pipeline.RFEX_READ.rs_val;
    cpu->pipeline.EXDC_WRITE.ALU_Result = rs_data + SEOffset;
    cpu->pipeline.EXDC_WRITE.num_bytes = WORD;
    cpu->pipeline.EXDC_WRITE.SW_Value = cpu->pipeline.RFEX_READ.rt_val;

    if((cpu->pipeline.EXDC_WRITE.ALU_Result & 0x3) != 0) {
        printf("address exception for SW function\n");
        exit(EXIT_FAILURE);
    }

}

void SDL(CPU *cpu) {

    cpu->pipeline.EXDC_WRITE.SW_Value = cpu->pipeline.RFEX_READ.rt_val;
    printf("you havent done this instruction yet!\n");
}

void SDR(CPU *cpu) {

    cpu->pipeline.EXDC_WRITE.SW_Value = cpu->pipeline.RFEX_READ.rt_val;
    printf("you havent done this instruction yet!\n");
}

void SWR(CPU *cpu) {

    cpu->pipeline.EXDC_WRITE.SW_Value = cpu->pipeline.RFEX_READ.rt_val;
    printf("you havent done this instruction yet!\n");
}

void LL(CPU *cpu) {

    //cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
    printf("you havent done this instruction yet!\n");
}

void LLD(CPU *cpu) {

    //cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
    printf("you havent done this instruction yet!\n");
}

void LD(CPU *cpu) {

   // cpu->pipeline.EXDC_WRITE.ALU_Result = (uint64_t)result;
    printf("you havent done this instruction yet!\n");
}

void SC(CPU *cpu) {

    cpu->pipeline.EXDC_WRITE.SW_Value = cpu->pipeline.RFEX_READ.rt_val;
    printf("you havent done this instruction yet!\n");
}

void SCD(CPU *cpu) {

    cpu->pipeline.EXDC_WRITE.SW_Value = cpu->pipeline.RFEX_READ.rt_val;
    printf("you havent done this instruction yet!\n");
}

void SD(CPU *cpu) {

    cpu->pipeline.EXDC_WRITE.SW_Value = cpu->pipeline.RFEX_READ.rt_val;
    printf("you havent done this instruction yet!\n");
}

//op code instruction table
void (*opcode_table[64])(CPU *) = {

    [0x00] = R_FORMAT,
    [0x01] = instruction_exception, //SPECIAL;
    [0x02] = J,
    [0x03] = JAL,
    [0x04] = BEQ,
    [0x05] = BNEQ,
    [0x06] = BLEZ,
    [0x07] = BGTZ,
    [0x08] = ADDI,
    [0x09] = ADDIU,
    [0x0A] = SLTI,
    [0x0B] = SLTIU,
    [0x0C] = ANDI,
    [0x0D] = ORI,
    [0x0E] = XORI,
    [0x0F] = LUI,
    [0x10] = MTC0,
    [0x11] = MTC1,
    [0x12] = instruction_exception,
    [0x13] = instruction_exception,
    [0x14] = BEQL,
    [0x15] = BNEL,
    [0x16] = BLEZL,
    [0x17] = BGTZL,
    [0x18] = DADDI,
    [0x19] = DADDIU,
    [0x1A] = LDL,
    [0X1B] = LDR,
    [0x1C] = instruction_exception,
    [0x1D] = instruction_exception,
    [0x1E] = instruction_exception,
    [0x1F] = instruction_exception,
    [0x20] = LB,
    [0x21] = LH,
    [0x22] = LWL,
    [0X23] = LW,
    [0x24] = LBU,
    [0x25] = LHU,
    [0x26] = LWR,
    [0x27] = LWU,
    [0X28] = SB,
    [0x29] = SH,
    [0x2A] = SW,
    [0x2B] = SW,
    [0x2C] = SDL,
    [0x2D] = SDR,
    [0x2E] = SWR,
    [0x2F] = instruction_exception,
    [0x30] = LL,
    [0x31] = instruction_exception,
    [0x32] = instruction_exception,
    [0x33] = instruction_exception,
    [0x34] = LLD,
    [0x35] = instruction_exception,
    [0x36] = instruction_exception,
    [0x37] = LD,
    [0x38] = SC,
    [0x39] = instruction_exception,
    [0x3A] = instruction_exception,
    [0x3B] = instruction_exception,
    [0x3C] = SCD,
    [0x3D] = instruction_exception,
    [0x3E] = instruction_exception,
    [0x3F] = SD
};


//R format functions
void SLL(CPU *cpu) {
    uint8_t shamt = cpu->pipeline.RFEX_READ.shamt;
    uint64_t rt_data = cpu->pipeline.RFEX_READ.rt_val;

    if(shamt == 0) {
        uint32_t data = (uint32_t) rt_data;
        cpu->pipeline.EXDC_WRITE.ALU_Result = data;
        cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
    } else {
        cpu->pipeline.EXDC_WRITE.ALU_Result = (rt_data << shamt);
        cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
    }
}

void SRL(CPU *cpu) {
    uint8_t shamt = cpu->pipeline.RFEX_READ.shamt;
    uint64_t rt_data = cpu->pipeline.RFEX_READ.rt_val;

    cpu->pipeline.EXDC_WRITE.ALU_Result = (rt_data >> shamt);
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;

    //if shift amt is zero, it can be seen as a no op so make sure to check this !!
}



void SRA(CPU *cpu) {
    uint8_t shamt = cpu->pipeline.RFEX_READ.shamt;
    int64_t rt_data = (int64_t)cpu->pipeline.RFEX_READ.rt_val; // Cast to signed type

    cpu->pipeline.EXDC_WRITE.ALU_Result = rt_data >> shamt;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void SLLV(CPU *cpu) {
    uint8_t shamt = cpu->pipeline.RFEX_READ.rs_val & 0x1F;
    uint64_t rt_data = cpu->pipeline.RFEX_READ.rt_val;

    if(shamt == 0) {
        uint32_t data = (uint32_t) rt_data;
        cpu->pipeline.EXDC_WRITE.ALU_Result = data;
        cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
    } else {
        cpu->pipeline.EXDC_WRITE.ALU_Result = (rt_data << shamt);
        cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
    }
}

void SRLV(CPU *cpu) {
    uint8_t shamt = cpu->pipeline.RFEX_READ.rs_val & 0x1F;
    uint64_t rt_data = cpu->pipeline.RFEX_READ.rt_val;

    cpu->pipeline.EXDC_WRITE.ALU_Result = (rt_data >> shamt);
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
    
}

void SRAV(CPU *cpu) {
    uint8_t shamt = cpu->pipeline.RFEX_READ.rs_val & 0x1F;
    int64_t rt_data = cpu->pipeline.RFEX_READ.rt_val;

    cpu->pipeline.EXDC_WRITE.ALU_Result = rt_data >> shamt;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void JR(CPU *cpu) {
    uint32_t target = cpu->pipeline.RFEX_READ.rs_val;
    if((target & 0x02) != 0) {
        //address exception;
    }
    //need to handle in EX stage, like managing the PC and stuff
    cpu->PC = target;
    printf("tried to execute JR");
    exit(EXIT_FAILURE);
}

void JALR(CPU *cpu) {
    uint32_t target = cpu->pipeline.RFEX_READ.rs_val;
    if((target & 0x02) != 0) {
        //address exception;
    }
    //need to handle in EX stage, like managing the PC and stuff
    cpu->PC = target;

    uint32_t return_addr = cpu->PC + 8;
    cpu->pipeline.EXDC_WRITE.ALU_Result = return_addr;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
    //need to handle this in WB stage, writing return addr to rd or r31
    printf("tried to execute JR\n");
    exit(EXIT_FAILURE);
}

void SYSCALL(CPU *cpu) {
    printf("syscall was attempted\n");
    exit(EXIT_FAILURE);
}

void BREAK(CPU *cpu) {
    printf("break was attempted\n");
    exit(EXIT_FAILURE);
}

void MFHI(CPU *cpu) {
    //two instructions which follow this cannot be mult, multu, div, divu, mthi, dmult, dmultu, ddiv, ddivu

    uint64_t data = cpu->regs[REG_HI];
    cpu->pipeline.EXDC_WRITE.ALU_Result = data;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void MTHI(CPU *cpu) {
    //maybe issue if following instruction is mflo, mfhi, mtlo, or mthi, register lo is undefined?

    uint64_t data = cpu->pipeline.RFEX_READ.rs_val;
    cpu->pipeline.EXDC_WRITE.ALU_Result = data;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = REG_HI; 
}

void MFLO(CPU *cpu) {
    uint64_t data = cpu->regs[REG_LO];
    cpu->pipeline.EXDC_WRITE.ALU_Result = data;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void MTLO(CPU *cpu) {
    uint64_t data = cpu->pipeline.RFEX_READ.rs_val;
    cpu->pipeline.EXDC_WRITE.ALU_Result = data;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = REG_LO; 
}

void DSLLV(CPU* cpu) {
    uint64_t data = cpu->pipeline.RFEX_READ.rt_val;
    uint8_t shamt = cpu->pipeline.RFEX_READ.rs_val & 0x3F;
    cpu->pipeline.EXDC_WRITE.ALU_Result = data << shamt;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void DSRLV(CPU *cpu) {
    uint64_t data = cpu->pipeline.RFEX_READ.rt_val;
    uint8_t shamt = cpu->pipeline.RFEX_READ.rs_val & 0x3F;
    cpu->pipeline.EXDC_WRITE.ALU_Result = data >> shamt;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void DSRAV(CPU *cpu) {
    int64_t data = cpu->pipeline.RFEX_READ.rt_val;
    uint8_t shamt = cpu->pipeline.RFEX_READ.rs_val & 0x3F;
    cpu->pipeline.EXDC_WRITE.ALU_Result = data >> shamt;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void MULT(CPU *cpu) {
    
}

void MULTU(CPU *cpu) {
    
}

void DIV(CPU *cpu) {
    
}

void DIVU(CPU *cpu) {
    
}

void DMULT(CPU *cpu) {
    
}

void DMULTU(CPU *cpu) {
    
}

void DDIV(CPU *cpu) {
    
}

void DDIVU(CPU *cpu) {
    
}

void ADD(CPU *cpu) {
    int64_t rs_val = (int32_t) cpu->pipeline.RFEX_READ.rs_val;
    int64_t rt_val = (int32_t) cpu->pipeline.RFEX_READ.rt_val;
    int64_t result = rs_val + rt_val;
    if (((rs_val > 0) && (rt_val > 0) && (result < 0)) || 
        ((rs_val < 0) && (rt_val < 0) && (result > 0))) {
        // Handle overflow: Set exception state in CPU (or perform appropriate action)
        printf("integer overflow during add\n");
        exit(EXIT_FAILURE);
        //cpu->cp0.exception_state = OVERFLOW_EXCEPTION; // Example exception state
        //cpu->pipeline.EXDC_WRITE.Write_Reg_Num = REG_NONE; // Prevent writing to rd
    } else {
        // No overflow: Write result to destination register
        cpu->pipeline.EXDC_WRITE.ALU_Result = (uint32_t)result;
        cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
    }

}

void ADDU(CPU *cpu) {
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint64_t result = rs_val + rt_val;

    cpu->pipeline.EXDC_WRITE.ALU_Result = result;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void SUB(CPU *cpu) {
    int64_t rs_val = (int32_t) cpu->pipeline.RFEX_READ.rs_val;
    int64_t rt_val = (int32_t) cpu->pipeline.RFEX_READ.rt_val;
    int64_t result = rs_val - rt_val;
    if (((rs_val > 0) && (rt_val < 0) && (result < 0)) || 
        ((rs_val < 0) && (rt_val > 0) && (result > 0))) {
        // Handle overflow: Set exception state in CPU (or perform appropriate action)
        printf("integer overflow during add\n");
        exit(EXIT_FAILURE);
        //cpu->cp0.exception_state = OVERFLOW_EXCEPTION; // Example exception state
        //cpu->pipeline.EXDC_WRITE.Write_Reg_Num = REG_NONE; // Prevent writing to rd
    } else {
        // No overflow: Write result to destination register
        cpu->pipeline.EXDC_WRITE.ALU_Result = (uint32_t)result;
        cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
    }
}

void SUBU(CPU *cpu) {
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint64_t result = rs_val - rt_val;

    cpu->pipeline.EXDC_WRITE.ALU_Result = result;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void AND(CPU *cpu) {
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint64_t result = rs_val & rt_val;

    cpu->pipeline.EXDC_WRITE.ALU_Result = result;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void OR(CPU *cpu) {
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint64_t result = rs_val | rt_val;

    cpu->pipeline.EXDC_WRITE.ALU_Result = result;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void XOR(CPU *cpu) {
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint64_t result = rs_val ^ rt_val;

    cpu->pipeline.EXDC_WRITE.ALU_Result = result;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void NOR(CPU *cpu) {
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint64_t result = ~(rs_val | rt_val);

    cpu->pipeline.EXDC_WRITE.ALU_Result = result;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void SLT(CPU *cpu) {
    int64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    int64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    
    cpu->pipeline.EXDC_WRITE.ALU_Result = (rs_val < rt_val) ? 1 : 0;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;    
}


void SLTU(CPU *cpu) {
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    
    cpu->pipeline.EXDC_WRITE.ALU_Result = (rs_val < rt_val) ? 1 : 0;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd; 
}

void DADD(CPU *cpu) {
    int64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    int64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    int64_t result = rs_val + rt_val;
    if (((rs_val > 0) && (rt_val > 0) && (result < 0)) || 
        ((rs_val < 0) && (rt_val < 0) && (result > 0))) {
        // Handle overflow: Set exception state in CPU (or perform appropriate action)
        printf("integer overflow during add\n");
        exit(EXIT_FAILURE);
        //cpu->cp0.exception_state = OVERFLOW_EXCEPTION; // Example exception state
        //cpu->pipeline.EXDC_WRITE.Write_Reg_Num = REG_NONE; // Prevent writing to rd
    } else {
        // No overflow: Write result to destination register
        cpu->pipeline.EXDC_WRITE.ALU_Result = result;
        cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
    }
}

void DADDU(CPU *cpu) {
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint64_t result = rs_val + rt_val;
    
    cpu->pipeline.EXDC_WRITE.ALU_Result = result;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;

}

void DSUB(CPU *cpu) {
    int64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    int64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    int64_t result = rs_val - rt_val;
    if (((rs_val > 0) && (rt_val < 0) && (result < 0)) || 
        ((rs_val < 0) && (rt_val > 0) && (result > 0))) {
        // Handle overflow: Set exception state in CPU (or perform appropriate action)
        printf("integer overflow during add\n");
        exit(EXIT_FAILURE);
        //cpu->cp0.exception_state = OVERFLOW_EXCEPTION; // Example exception state
        //cpu->pipeline.EXDC_WRITE.Write_Reg_Num = REG_NONE; // Prevent writing to rd
    } else {
        // No overflow: Write result to destination register
        cpu->pipeline.EXDC_WRITE.ALU_Result = result;
        cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
    }
}

void DSUBU(CPU *cpu) {
    
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint64_t result = rs_val - rt_val;
    
    cpu->pipeline.EXDC_WRITE.ALU_Result = result;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void TGE(CPU *cpu) {
    int64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    int64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    printf("trap ge\n");
    exit(EXIT_FAILURE);
    //TODO: HANDLE TRAP
}

void TGEU(CPU *cpu) {
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    printf("trap geu\n");
    exit(EXIT_FAILURE);
    //TODO: HANDLE TRAP
}

void TLT(CPU *cpu) {
    int64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    int64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    printf("trap tlt\n");
    exit(EXIT_FAILURE);
    //TODO: HANDLE TRAP
}

void TLTU(CPU *cpu) {
    uint64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    printf("trap tltu\n");
    exit(EXIT_FAILURE);
    //TODO: HANDLE TRAP
}

void TE(CPU *cpu) {
    int64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    int64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    printf("trap te\n");
    exit(EXIT_FAILURE);
    //TODO: HANDLE TRAP
}

void TNE(CPU *cpu) {
    int64_t rs_val = cpu->pipeline.RFEX_READ.rs_val;
    int64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    printf("trap tne\n");
    exit(EXIT_FAILURE);
    //TODO: HANDLE TRAP
}

void DSLL(CPU *cpu) {
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint8_t shamt = cpu->pipeline.RFEX_READ.shamt;

    cpu->pipeline.EXDC_WRITE.ALU_Result = rt_val << shamt;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void DSRL(CPU *cpu) {
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint8_t shamt = cpu->pipeline.RFEX_READ.shamt;

    cpu->pipeline.EXDC_WRITE.ALU_Result = rt_val >> shamt;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void DSRA(CPU *cpu) {
    int64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint8_t shamt = cpu->pipeline.RFEX_READ.shamt;

    cpu->pipeline.EXDC_WRITE.ALU_Result = rt_val >> shamt;
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void DSLL32(CPU *cpu) {
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint8_t shamt = cpu->pipeline.RFEX_READ.shamt;

    cpu->pipeline.EXDC_WRITE.ALU_Result = rt_val << (32 + shamt);
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void DSRL32(CPU *cpu) {
    uint64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint8_t shamt = cpu->pipeline.RFEX_READ.shamt;

    cpu->pipeline.EXDC_WRITE.ALU_Result = rt_val >> (32 + shamt);
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;
}

void DSRA32(CPU *cpu) {
    int64_t rt_val = cpu->pipeline.RFEX_READ.rt_val;
    uint8_t shamt = cpu->pipeline.RFEX_READ.shamt;

    cpu->pipeline.EXDC_WRITE.ALU_Result = rt_val >> (32 + shamt);
    cpu->pipeline.EXDC_WRITE.Write_Reg_Num = cpu->pipeline.RFEX_READ.rd;    
}



void (*function_table[64])(CPU *) = {

    [0x00] = SLL,
    [0x01] = instruction_exception,
    [0x02] = SRL,
    [0x03] = SRA,
    [0x04] = SLLV,
    [0x05] = instruction_exception,
    [0x06] = SRLV,
    [0x07] = SRAV,
    [0x08] = JR,
    [0x09] = JALR,
    [0x0A] = instruction_exception,
    [0x0B] = instruction_exception,
    [0x0C] = SYSCALL,
    [0x0D] = BREAK,
    [0x0E] = instruction_exception,
    [0x0F] = instruction_exception, //might be NOP
    [0x10] = MFHI,
    [0x11] = MTHI,
    [0x12] = MFLO,
    [0x13] = MTLO,
    [0x14] = DSLLV,
    [0x15] = instruction_exception,
    [0x16] = DSRLV,
    [0x17] = DSRAV,
    [0x18] = MULT,
    [0x19] = MULTU,
    [0x1A] = DIV,
    [0X1B] = DIVU,
    [0x1C] = DMULT,
    [0x1D] = DMULTU,
    [0x1E] = DDIV,
    [0x1F] = DDIVU,
    [0x20] = ADD,
    [0x21] = ADDU,
    [0x22] = SUB,
    [0X23] = SUBU,
    [0x24] = AND,
    [0x25] = OR,
    [0x26] = XOR,
    [0x27] = NOR,
    [0X28] = instruction_exception,
    [0x29] = instruction_exception,
    [0x2A] = SLT,
    [0x2B] = SLTU,
    [0x2C] = DADD,
    [0x2D] = DADDU,
    [0x2E] = DSUB,
    [0x2F] = DSUBU,
    [0x30] = TGE,
    [0x31] = TGEU,
    [0x32] = TLT,
    [0x33] = TLTU,
    [0x34] = TE,
    [0x35] = instruction_exception,
    [0x36] = TNE,
    [0x37] = instruction_exception,
    [0x38] = DSLL,
    [0x39] = instruction_exception,
    [0x3A] = DSRL,
    [0x3B] = DSRA,
    [0x3C] = DSLL32,
    [0x3D] = instruction_exception,
    [0x3E] = DSRL32,
    [0x3F] = DSRA32
};
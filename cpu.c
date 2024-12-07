#include "cpu.h"

uint32_t fetch_instruction(CPU *cpu) {

    uint32_t physical_address = map_virtual(cpu->mmu, cpu->PC);
    uint32_t instruction = map_physical(cpu->mmu, physical_address);

    uint32_t big_endian_instruction = 
        ((instruction & 0xFF) << 24) |
        ((instruction & 0xFF00) << 8) |
        ((instruction & 0xFF0000) >> 8) |
        ((instruction & 0xFF000000) >> 24);

    printf("Fetched Instruction (PC: 0x%016llX, Physical: 0x%08X): 0x%08X\n",
           cpu->PC, physical_address, big_endian_instruction);

    return instruction;
}

void IC_stage(CPU *cpu) {

    cpu->pipeline.ICRF_WRITE.instruction = fetch_instruction(cpu);

}

void RF_stage(CPU *cpu) {

    Instruction decoded_inst = decode(cpu->pipeline.ICRF_READ.instruction);
    decoded_inst.rs_val = cpu->gpr[decoded_inst.rs];
    decoded_inst.rt_val = cpu->gpr[decoded_inst.rt];

    if(cpu->pipeline.RFEX_WRITE.control.f_type == I_TYPE) {
        decoded_inst.SEOffset = (int32_t)decoded_inst.immediate;
    }
    
    if(cpu->pipeline.RFEX_WRITE.control.f_type == J_TYPE) {
        decoded_inst.branch_addr = cpu->PC + 4 + (decoded_inst.SEOffset << 2);
        decoded_inst.jump_addr = cpu->pipeline.RFEX_READ.instruction.instruction & 0x03FFFFFF; //fix this
    }
    //cpu->pipeline.RFEX_WRITE.control = opcode_table[decoded_inst.opcode];
    cpu->pipeline.RFEX_WRITE.instruction = decoded_inst;
}

void EX_stage(CPU *cpu) {
    //pass control values that are not used

    cpu->pipeline.EXDC_WRITE.SW_Value = cpu->pipeline.RFEX_READ.instruction.rt_val;

    /* cpu->pipeline.EXDC_WRITE = ????ALU OR WHATEVER = RFEX_READ */
}

void ReadToWrite(CPU *cpu) {

    cpu->pipeline.ICRF_READ.instruction = cpu->pipeline.ICRF_WRITE.instruction;

    cpu->pipeline.RFEX_READ.instruction = cpu->pipeline.RFEX_WRITE.instruction;
    cpu->pipeline.RFEX_READ.control = cpu->pipeline.RFEX_WRITE.control;

    cpu->pipeline.RFEX_READ.instruction = cpu->pipeline.RFEX_WRITE.instruction;
    cpu->pipeline.RFEX_READ.control = cpu->pipeline.RFEX_WRITE.control;
    
    cpu->pipeline.EXDC_READ.instruction = cpu->pipeline.EXDC_WRITE.instruction;
    cpu->pipeline.EXDC_READ.control = cpu->pipeline.EXDC_WRITE.control;
    cpu->pipeline.EXDC_READ.ALU_Result = cpu->pipeline.EXDC_WRITE.ALU_Result;
    cpu->pipeline.EXDC_READ.SW_Value = cpu->pipeline.EXDC_WRITE.SW_Value;
    cpu->pipeline.EXDC_READ.Write_Reg_Num = cpu->pipeline.EXDC_WRITE.Write_Reg_Num;

    cpu->pipeline.DCWB_READ.instruction = cpu->pipeline.DCWB_WRITE.instruction;
    cpu->pipeline.DCWB_READ.control = cpu->pipeline.DCWB_WRITE.control;
    cpu->pipeline.DCWB_READ.ALU_Result = cpu->pipeline.DCWB_WRITE.ALU_Result;
    cpu->pipeline.DCWB_READ.LW_Data_Value = cpu->pipeline.DCWB_WRITE.LW_Data_Value;
    cpu->pipeline.DCWB_READ.Write_Reg_Num = cpu->pipeline.DCWB_WRITE.Write_Reg_Num;
}


int instruction_exception() {
    printf("instruction error\n");
    return 1;
}


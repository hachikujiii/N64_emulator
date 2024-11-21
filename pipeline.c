#include "n64.h"
#include "pipeline.h"
#include "instruction_control.h"

void IC_stage(unsigned char *rom, N64 *n64) {

    n64->cpu.pipeline.ICRF_WRITE.instruction = *(uint32_t *)(rom + n64->cpu.PC);
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
    cpu->pipeline.RFEX_WRITE.control = opcode_table[decoded_inst.opcode];
    cpu->pipeline.RFEX_WRITE.instruction = decoded_inst;
}

void EX_stage(N64 *n64) {
    //pass control values that are not used

    n64->cpu.pipeline.EXDC_WRITE.SW_Value = n64->cpu.pipeline.RFEX_READ.instruction.rt_val;

    /* cpu->pipeline.EXDC_WRITE = ????ALU OR WHATEVER = RFEX_READ */
}
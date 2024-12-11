#include "control.h"
#include "debug_sys.h"



const char* register_names[] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};


void print_regs(CPU *cpu) {
    for(int i = 0; i < 32; i++) {
        
        printf("Register %d = %llX\n", i, cpu->gpr[i]);
    }
    printf("\n");
    printf("PC = 0x%llX\n", cpu->PC);
}

void print_cpu(CPU *cpu) {

    if(cpu == NULL) {
        printf("CPU is NULL\n");
        return;
    }
    printf("general purpose registers:\n");
    print_regs(cpu);
    printf("\n");

    printf("floating point registers:\n");
    //to do
    printf("\n");
    printf("HI = 0x%llX\n", cpu->HI);
    printf("LO = 0x%llX\n", cpu->LO);
    printf("\n");
}

void print_instruction(Instruction instruction) {

    uint32_t big_endian_instruction = 
        ((instruction.instruction & 0xFF) << 24) |
        ((instruction.instruction & 0xFF00) << 8) |
        ((instruction.instruction & 0xFF0000) >> 8) |
        ((instruction.instruction & 0xFF000000) >> 24);

    printf("Instruction decoded:\n");
    printf("instruction: 0x%08X\n", big_endian_instruction);
    printf("opcode: 0x%02X\n", instruction.opcode);
    printf("rs (source reg): %d\n", instruction.rs);
    printf("rt (temp reg): %d\n", instruction.rt);
    printf("rd (dest reg): %d\n", instruction.rd);
    printf("shamt: 0x%X\n", instruction.shamt);
    printf("function: 0x%X\n", instruction.function);
    printf("immediate: 0x%X\n", instruction.immediate);
    printf("\n");
}

const char* format_type_to_string(Format_Type f_type) { 
    switch(f_type) { 
        case R_TYPE: return "R_TYPE"; 
        case I_TYPE: return "I_TYPE"; 
        case J_TYPE: return "J_TYPE"; 
        case REGIMM: return "REGIMM"; 
        case TRAP: return "TRAP"; 
        case ERROR: return "ERROR"; 
        default: return "UNKNOWN"; 
    } 
} 

const char* instruction_type_to_string(Instruction_Type type) { 
    switch(type) { 
        case LOAD: return "LOAD"; 
        case STORE: return "STORE"; 
        case ALU: return "ALU"; 
        case BRANCH: return "BRANCH"; 
        case JUMP: return "JUMP"; 
        case COPROC: return "COPROC"; 
        case SPECIAL: return "SPECIAL"; 
        case INVALID: return "INVALID"; 
        default: return "UNKNOWN"; 
    } 
} 

const char* access_type_to_string(Access_Type memAccess) { 
    switch(memAccess) { 
        case NO_MEM: return "NO_MEM"; 
        case MEM_READ: return "MEM_READ"; 
        case MEM_WRITE: return "MEM_WRITE"; 
        default: return "UNKNOWN"; 
    } 
}

void print_pif_ram(Memory *memory) {

    // Print out the PIF RAM in hex format (byte by byte)
    for (size_t i = 0; i < PIF_ROM_SIZE; i++) {
        uint8_t byte = memory->pif_rom.data[i];  // Access PIF RAM byte by byte
        printf("%02X ", byte);
        
        // Print 16 bytes per line for readability
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }

    printf("\n");
}

void print_pipeline(CPU *cpu) {

/* NSTRUCTION CACHE / REGISTER FETCH */
    uint32_t instruction_write = byte_swap(cpu->pipeline.ICRF_WRITE.instruction);
    uint32_t instruction_read = byte_swap(cpu->pipeline.ICRF_READ.instruction);
    printf("IC/RF Write\n");
    printf("-----------\n");
    printf("Instruction = 0x%08X\n\n", instruction_write);

    printf("IC/RF Read\n");
    printf("----------\n");
    printf("Instruction = 0x%08X\n\n", instruction_read);

/* REGISTER FETCH / EXECUTION */

    printf("RF/EX Write\n");
    printf("-----------\n");
    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
                format_type_to_string(cpu->pipeline.RFEX_WRITE.control.f_type),
               instruction_type_to_string(cpu->pipeline.RFEX_WRITE.control.type),
                access_type_to_string(cpu->pipeline.RFEX_WRITE.control.memAccess));
    printf("rs_value = %llX, rt_value = %llX, SEOffset = %X\nrt_reg_number = %d, rd_reg_number = %d, Function = %X\n\n",
                cpu->pipeline.RFEX_WRITE.instruction.rs_val, cpu->pipeline.RFEX_WRITE.instruction.rt_val, cpu->pipeline.RFEX_WRITE.instruction.SEOffset, 
                cpu->pipeline.RFEX_WRITE.instruction.rt, cpu->pipeline.RFEX_WRITE.instruction.rd,
                cpu->pipeline.RFEX_WRITE.instruction.function);

    printf("RF/EX Read\n");
    printf("----------\n");
//    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
//                format_type_to_string(n64->cpu.pipeline.RFEX_READ.control.f_type),
//                instruction_type_to_string(n64->cpu.pipeline.RFEX_READ.control.type),
//                access_type_to_string(n64->cpu.pipeline.RFEX_READ.control.memAccess));
//    printf("rs_value = %X, rt_value = %X, SEOffset = %X\nrt_reg_number = %d, rd_reg_number = %d, Function = %X\n\n",
//                n64->cpu.pipeline.RFEX_READ.instruction.rs_val, n64->cpu.pipeline.RFEX_READ.instruction.rt_val, n64->cpu.pipeline.RFEX_READ.instruction.SEOffset, 
//                n64->cpu.pipeline.RFEX_READ.instruction.rt, n64->cpu.pipeline.RFEX_READ.instruction.rd,
//                n64->cpu.pipeline.RFEX_READ.instruction.function);

/* EXECUTION / DATA CACHE */

    printf("EX/DC Write\n");
    printf("------------\n");
//    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
//                format_type_to_string(n64->cpu.pipeline.EXDC_WRITE.control.f_type),
//                instruction_type_to_string(n64->cpu.pipeline.EXDC_WRITE.control.type),
//                access_type_to_string(n64->cpu.pipeline.EXDC_WRITE.control.memAccess));
    printf("ALUResult = %llX, SWValue = %llX, WriteRegNum = %lld\n\n",
                cpu->pipeline.EXDC_WRITE.ALU_Result, cpu->pipeline.EXDC_WRITE.SW_Value, cpu->pipeline.EXDC_WRITE.Write_Reg_Num);

    printf("EX/DC Read\n");
    printf("-----------\n");
//    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
//                format_type_to_string(n64->cpu.pipeline.EXDC_READ.control.f_type),
//                instruction_type_to_string(n64->cpu.pipeline.EXDC_READ.control.type),
//                access_type_to_string(n64->cpu.pipeline.EXDC_READ.control.memAccess));
    printf("ALUResult = %llX, SWValue = %llX, WriteRegNum = %lld\n\n",
                cpu->pipeline.EXDC_READ.ALU_Result, cpu->pipeline.EXDC_READ.SW_Value, cpu->pipeline.EXDC_READ.Write_Reg_Num);

/* DATA CACHE / WRITE BACK */

    printf("DC/WB Write\n");
    printf("------------\n");
//    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
//                format_type_to_string(n64->cpu.pipeline.DCWB_WRITE.control.f_type),
//                instruction_type_to_string(n64->cpu.pipeline.DCWB_WRITE.control.type),
//                access_type_to_string(n64->cpu.pipeline.DCWB_WRITE.control.memAccess));
    printf("LWDataValue = %llX, ALUResult = %llX, WriteRegNum = %lld\n\n",
                cpu->pipeline.DCWB_WRITE.LW_Data_Value, cpu->pipeline.DCWB_WRITE.ALU_Result, cpu->pipeline.DCWB_WRITE.Write_Reg_Num);

    printf("DC/WB Read\n");
    printf("-----------\n");
//    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
//                format_type_to_string(n64->cpu.pipeline.DCWB_READ.control.f_type),
//                instruction_type_to_string(n64->cpu.pipeline.DCWB_READ.control.type),
//                access_type_to_string(n64->cpu.pipeline.DCWB_READ.control.memAccess));
    printf("LWDataValue = %llX, ALUResult = %llX, WriteRegNum = %lld\n\n\n",
                cpu->pipeline.DCWB_READ.LW_Data_Value, cpu->pipeline.DCWB_READ.ALU_Result, cpu->pipeline.DCWB_READ.Write_Reg_Num);           
}

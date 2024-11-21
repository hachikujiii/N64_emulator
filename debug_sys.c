#include "debug_sys.h"
#include "instruction_control.h"

const char* register_names[] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};

void print_regs(N64 *n64) {
    for(int i = 0; i < 32; i++) {
        
        printf("Register %d = %llX\n", i, n64->cpu.gpr[i]);
    }
    printf("\n");
    printf("PC = %llX\n", n64->cpu.PC);
}

void print_cpu(N64 *n64) {

    printf("general purpose registers:\n");
    print_regs(n64);
    printf("\n");

    printf("floating point registers:\n");
    //to do
    printf("PC = %llX\n", n64->cpu.PC);
    printf("HI = %llX\n", n64->cpu.HI);
    printf("LO = %llX\n", n64->cpu.LO);
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

void print_pipeline(N64 *n64) {

/* NSTRUCTION CACHE / REGISTER FETCH */

    printf("IC/RF Write\n");
    printf("-----------\n");
    printf("Instruction = 0x%08X\n\n", n64->cpu.pipeline.ICRF_WRITE.instruction);

    printf("IC/RF Read\n");
    printf("----------\n");
    printf("Instruction = 0x%08X\n\n", n64->cpu.pipeline.ICRF_READ.instruction);

/* REGISTER FETCH / EXECUTION */

    printf("RF/EX Write\n");
    printf("-----------\n");
    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
                format_type_to_string(n64->cpu.pipeline.RFEX_WRITE.control.f_type),
                instruction_type_to_string(n64->cpu.pipeline.RFEX_WRITE.control.type),
                access_type_to_string(n64->cpu.pipeline.RFEX_WRITE.control.memAccess));
    printf("rs_value = %X, rt_value = %X, SEOffset = %X\nrt_reg_number = %d, rd_reg_number = %d, Function = %X\n\n",
                n64->cpu.pipeline.RFEX_WRITE.instruction.rs_val, n64->cpu.pipeline.RFEX_WRITE.instruction.rt_val, n64->cpu.pipeline.RFEX_WRITE.instruction.SEOffset, 
                n64->cpu.pipeline.RFEX_WRITE.instruction.rt, n64->cpu.pipeline.RFEX_WRITE.instruction.rd,
                n64->cpu.pipeline.RFEX_WRITE.instruction.function);

    printf("RF/EX Read\n");
    printf("----------\n");
    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
                format_type_to_string(n64->cpu.pipeline.RFEX_READ.control.f_type),
                instruction_type_to_string(n64->cpu.pipeline.RFEX_READ.control.type),
                access_type_to_string(n64->cpu.pipeline.RFEX_READ.control.memAccess));
    printf("rs_value = %X, rt_value = %X, SEOffset = %X\nrt_reg_number = %d, rd_reg_number = %d, Function = %X\n\n",
                n64->cpu.pipeline.RFEX_READ.instruction.rs_val, n64->cpu.pipeline.RFEX_READ.instruction.rt_val, n64->cpu.pipeline.RFEX_READ.instruction.SEOffset, 
                n64->cpu.pipeline.RFEX_READ.instruction.rt, n64->cpu.pipeline.RFEX_READ.instruction.rd,
                n64->cpu.pipeline.RFEX_READ.instruction.function);

/* EXECUTION / DATA CACHE */

    printf("EX/DC Write\n");
    printf("------------\n");
    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
                format_type_to_string(n64->cpu.pipeline.EXDC_WRITE.control.f_type),
                instruction_type_to_string(n64->cpu.pipeline.EXDC_WRITE.control.type),
                access_type_to_string(n64->cpu.pipeline.EXDC_WRITE.control.memAccess));
    printf("ALUResult = %X, SWValue = %X, WriteRegNum = %d\n\n",
                n64->cpu.pipeline.EXDC_WRITE.ALU_Result, n64->cpu.pipeline.EXDC_WRITE.SW_Value, n64->cpu.pipeline.EXDC_WRITE.Write_Reg_Num);

    printf("EX/DC Read\n");
    printf("-----------\n");
    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
                format_type_to_string(n64->cpu.pipeline.EXDC_READ.control.f_type),
                instruction_type_to_string(n64->cpu.pipeline.EXDC_READ.control.type),
                access_type_to_string(n64->cpu.pipeline.EXDC_READ.control.memAccess));
    printf("ALUResult = %X, SWValue = %X, WriteRegNum = %d\n\n",
                n64->cpu.pipeline.EXDC_READ.ALU_Result, n64->cpu.pipeline.EXDC_READ.SW_Value, n64->cpu.pipeline.EXDC_READ.Write_Reg_Num);

/* DATA CACHE / WRITE BACK */

    printf("DC/WB Write\n");
    printf("------------\n");
    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
                format_type_to_string(n64->cpu.pipeline.DCWB_WRITE.control.f_type),
                instruction_type_to_string(n64->cpu.pipeline.DCWB_WRITE.control.type),
                access_type_to_string(n64->cpu.pipeline.DCWB_WRITE.control.memAccess));
    printf("LWDataValue = %X, ALUResult = %X, WriteRegNum = %d\n\n",
                n64->cpu.pipeline.DCWB_WRITE.LW_Data_Value, n64->cpu.pipeline.DCWB_WRITE.ALU_Result, n64->cpu.pipeline.DCWB_WRITE.Write_Reg_Num);

    printf("DC/WB Read\n");
    printf("-----------\n");
    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
                format_type_to_string(n64->cpu.pipeline.DCWB_READ.control.f_type),
                instruction_type_to_string(n64->cpu.pipeline.DCWB_READ.control.type),
                access_type_to_string(n64->cpu.pipeline.DCWB_READ.control.memAccess));
    printf("LWDataValue = %X, ALUResult = %X, WriteRegNum = %d\n\n\n",
                n64->cpu.pipeline.DCWB_READ.LW_Data_Value, n64->cpu.pipeline.DCWB_READ.ALU_Result, n64->cpu.pipeline.DCWB_READ.Write_Reg_Num);           
}

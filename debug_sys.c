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
        
        printf("Register %d = %llX\n", i, cpu->regs[i]);
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

    //printf("floating point registers:\n");
    //to do
    printf("\n");
    printf("HI = 0x%llX\n", cpu->regs[REG_HI]);
    printf("LO = 0x%llX\n", cpu->regs[REG_LO]);
    printf("\n\n");
    printf("CP0 registers:\n");
    printf("[0] Index: 0x%llX\n", cpu->regs[32]);
    printf("[1] Random: 0x%llX\n", cpu->regs[33]);
    printf("[2] EntryLo0: 0x%llX\n", cpu->regs[34]);
    printf("[3] EntryLo1: 0x%llX\n", cpu->regs[35]);
    printf("[4] Context: 0x%llX\n", cpu->regs[36]);
    printf("[5] PageMask: 0x%llX\n", cpu->regs[37]);
    printf("[6] Wired: 0x%llX\n", cpu->regs[38]);
    printf("[7] NOT USED: 0x%llX\n", cpu->regs[39]);
    printf("[8] BadVAddr: 0x%llX\n", cpu->regs[40]);
    printf("[9] Count: 0x%llX\n", cpu->regs[41]);
    printf("[10] EntryHi: 0x%llX\n", cpu->regs[42]);
    printf("[11] Compare: 0x%llX\n", cpu->regs[43]);
    printf("[12] Status: 0x%llX\n", cpu->regs[44]);
    printf("[13] Cause: 0x%llX\n", cpu->regs[45]);
    printf("[14] EPC: 0x%llX\n", cpu->regs[46]);
    printf("[15] PRId: 0x%llX\n", cpu->regs[47]);
    printf("[16] Config: 0x%llX\n", cpu->regs[48]);
    printf("[17] LLAddr: 0x%llX\n", cpu->regs[49]);
    printf("[18] WatchLo: 0x%llX\n", cpu->regs[50]);
    printf("[19] WatchHi: 0x%llX\n", cpu->regs[51]);
    printf("[20] XContext: 0x%llX\n", cpu->regs[52]);
    printf("[28] TagLo: 0x%llX\n", cpu->regs[53]);
    printf("[29] TagHi: 0x%llX\n", cpu->regs[54]);
    printf("[30] ErrorEPC: 0x%llX\n", cpu->regs[55]);
}

void print_instruction(Pipeline *pipeline) {


    uint32_t instruction = pipeline->RFEX_WRITE.instruction;

    printf("\nInstruction decoded:\n");
    printf("instruction: 0x%08X\n", instruction);
    printf("opcode: 0x%02X\n", pipeline->RFEX_WRITE.opcode);
    printf("rs (source reg): %d\n", pipeline->RFEX_WRITE.rs);
    printf("rt (temp reg): %d\n", pipeline->RFEX_WRITE.rt);
    printf("rd (dest reg): %d\n", pipeline->RFEX_WRITE.rd);
    printf("shamt: 0x%X\n", pipeline->RFEX_WRITE.shamt);
    printf("function: 0x%X\n", pipeline->RFEX_WRITE.function);
    printf("immediate: 0x%X\n", pipeline->RFEX_WRITE.immediate);
    printf("\n\n");
}

const char* format_type_to_string(Format_Type f_type) { 
    switch(f_type) { 
        case R_TYPE: return "R_TYPE"; 
        case I_TYPE: return "I_TYPE"; 
        case J_TYPE: return "J_TYPE";  
        case TRAP: return "TRAP"; 
        case ERROR: return "ERROR";
        case NOP: return "NOP";
        default: return "UNKNOWN"; 
    } 
} 

const char* instruction_type_to_string(Operation_Type type) { 
    switch(type) { 
        case LOAD: return "LOAD"; 
        case STORE: return "STORE"; 
        case ALU: return "ALU"; 
        case BRANCH: return "BRANCH"; 
        case JUMP: return "JUMP";  
        case SPECIAL: return "SPECIAL"; 
        case INVALID: return "INVALID"; 
        default: return "UNKNOWN"; 
    } 
} 

const char* access_type_to_string(Access_Type memAccess) { 
    switch(memAccess) { 
        case NO_ACCESS: return "NO_ACCESS"; 
        case MEM_READ: return "MEM_READ"; 
        case MEM_WRITE: return "MEM_WRITE"; 
        case REG_READ: return "REG_READ";
        case REG_WRITE: return "REG_WRITE";
        case COPROC: return "COPROC";
        default: return "UNKNOWN"; 
    } 
}

void print_pif_ram(Memory *mem) {

    // Print out the PIF RAM in hex format (byte by byte)
    for (size_t i = 0; i < PIF_ROM_SIZE; i++) {
        uint8_t byte = mem->pif_rom[i];  // Access PIF RAM byte by byte
        printf("%02X ", byte);
        
        // Print 16 bytes per line for readability
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }

    printf("\n");
}

int counter = 0;

void print_pipeline(CPU *cpu) {
printf("\n================================\n");
printf("CYCLE %d\n", counter++);
printf("================================\n\n");
/* NSTRUCTION CACHE / REGISTER FETCH */
    uint32_t instruction_write = cpu->pipeline.ICRF_WRITE.instruction;
    uint32_t instruction_read = cpu->pipeline.ICRF_READ.instruction;
    printf("IC/RF Write\n");
    printf("-----------\n");
    printf("Instruction = 0x%08X\n\n", instruction_write);

    printf("IC/RF Read\n");
    printf("----------\n");
    printf("Instruction = 0x%08X\n\n", instruction_read);

/* REGISTER FETCH / EXECUTION */

    printf("RF/EX Write\n");
    printf("-----------\n");
    printf("Control: Format = %s, Op type = %s, Mem access = %s\n", 
                format_type_to_string(cpu->pipeline.RFEX_WRITE.control.format_type),
                instruction_type_to_string(cpu->pipeline.RFEX_WRITE.control.op_type),
                access_type_to_string(cpu->pipeline.RFEX_WRITE.control.mem_access));
    printf("Flags: MemRead = %d, MemToReg = %d, MemWrite = %d, RegWrite = %d, RegDst = %d\n", 
                cpu->pipeline.RFEX_WRITE.MemRead, cpu->pipeline.RFEX_WRITE.MemToReg, cpu->pipeline.RFEX_WRITE.MemWrite, 
                cpu->pipeline.RFEX_WRITE.RegWrite, cpu->pipeline.RFEX_WRITE.RegDst);
    printf("Decoded Inst: rs_reg_num = %d, rt_reg_number = %d, rd_reg_number = %d\n",
                cpu->pipeline.RFEX_WRITE.rs, cpu->pipeline.RFEX_WRITE.rt, cpu->pipeline.RFEX_WRITE.rd); 
    printf("Data: rs_value = 0x%llX, rt_value = 0x%llX, Immediate = 0x%08X\nFunction = 0x%X\n\n",
                cpu->pipeline.RFEX_WRITE.rs_val, cpu->pipeline.RFEX_WRITE.rt_val, cpu->pipeline.RFEX_WRITE.immediate,
                cpu->pipeline.RFEX_WRITE.function);

    printf("RF/EX Read\n");
    printf("----------\n");
    printf("Control: Format = %s, Op type = %s, Mem access = %s\n", 
                format_type_to_string(cpu->pipeline.RFEX_READ.control.format_type),
                instruction_type_to_string(cpu->pipeline.RFEX_READ.control.op_type),
                access_type_to_string(cpu->pipeline.RFEX_READ.control.mem_access));
    printf("Flags: MemRead = %d, MemToReg = %d, MemWrite = %d, RegWrite = %d, RegDst = %d\n", 
                cpu->pipeline.RFEX_READ.MemRead, cpu->pipeline.RFEX_READ.MemToReg, cpu->pipeline.RFEX_READ.MemWrite, 
                cpu->pipeline.RFEX_READ.RegWrite, cpu->pipeline.RFEX_READ.RegDst);
    printf("Decoded Inst: rs_reg_num = %d, rt_reg_number = %d, rd_reg_number = %d\n",
                cpu->pipeline.RFEX_READ.rs, cpu->pipeline.RFEX_READ.rt, cpu->pipeline.RFEX_READ.rd); 
    printf("Data: rs_value = 0x%llX, rt_value = 0x%llX, Immediate = 0x%08X\nFunction = 0x%X\n\n",
                cpu->pipeline.RFEX_READ.rs_val, cpu->pipeline.RFEX_READ.rt_val, cpu->pipeline.RFEX_READ.immediate,
                cpu->pipeline.RFEX_READ.function);

/* EXECUTION / DATA CACHE */

    printf("EX/DC Write\n");
    printf("------------\n");
    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
                format_type_to_string(cpu->pipeline.EXDC_WRITE.control.format_type),
                instruction_type_to_string(cpu->pipeline.EXDC_WRITE.control.op_type),
                access_type_to_string(cpu->pipeline.EXDC_WRITE.control.mem_access));
    printf("ALUResult = %llX, SWValue = %llX, WriteRegNum = %d, RegWrite = %d\n\n",
                cpu->pipeline.EXDC_WRITE.ALU_Result, cpu->pipeline.EXDC_WRITE.SW_Value, cpu->pipeline.EXDC_WRITE.Write_Reg_Num,
                cpu->pipeline.EXDC_WRITE.RegWrite);

    printf("EX/DC Read\n");
    printf("-----------\n");
    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
                format_type_to_string(cpu->pipeline.EXDC_READ.control.format_type),
                instruction_type_to_string(cpu->pipeline.EXDC_READ.control.op_type),
                access_type_to_string(cpu->pipeline.EXDC_READ.control.mem_access));
    printf("ALUResult = %llX, SWValue = %llX, WriteRegNum = %d, RegWrite = %d\n\n",
                cpu->pipeline.EXDC_READ.ALU_Result, cpu->pipeline.EXDC_READ.SW_Value, cpu->pipeline.EXDC_READ.Write_Reg_Num,
                cpu->pipeline.EXDC_READ.RegWrite);

/* DATA CACHE / WRITE BACK */

    printf("DC/WB Write\n");
    printf("------------\n");
    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
                format_type_to_string(cpu->pipeline.DCWB_WRITE.control.format_type),
                instruction_type_to_string(cpu->pipeline.DCWB_WRITE.control.op_type),
                access_type_to_string(cpu->pipeline.DCWB_WRITE.control.mem_access));
    printf("LWDataValue = %llX, ALUResult = %llX, WriteRegNum = %d, RegWrite = %d\n\n",
                cpu->pipeline.DCWB_WRITE.LW_Data_Value, cpu->pipeline.DCWB_WRITE.ALU_Result, cpu->pipeline.DCWB_WRITE.Write_Reg_Num,
                cpu->pipeline.DCWB_WRITE.RegWrite);

    printf("DC/WB Read\n");
    printf("-----------\n");
    printf("Control: Format = %s, Inst type = %s, Mem access = %s\n", 
                format_type_to_string(cpu->pipeline.DCWB_READ.control.format_type),
                instruction_type_to_string(cpu->pipeline.DCWB_READ.control.op_type),
                access_type_to_string(cpu->pipeline.DCWB_READ.control.mem_access));
    printf("LWDataValue = %llX, ALUResult = %llX, WriteRegNum = %d, RegWrite = %d\n\n\n",
                cpu->pipeline.DCWB_READ.LW_Data_Value, cpu->pipeline.DCWB_READ.ALU_Result, cpu->pipeline.DCWB_READ.Write_Reg_Num,
                cpu->pipeline.DCWB_READ.RegWrite);           
}

void print_rsp(RSP *rsp) {

    printf("----------------------\n");
    printf("REALITY SIGNAL PROCESSOR DUMP:\n\n");
    printf("----------------------\n");
    printf("Registers:\n");
    printf("sp_dma_spaddr: 0x%X\n", rsp->sp_dma_spaddr);
    printf("sp_dma_ramaddr: 0x%X\n", rsp->sp_dma_ramaddr);
    printf("sp_dma_rdlen: 0x%X\n", rsp->sp_dma_rdlen);
    printf("sp_dma_wrlen: 0x%X\n", rsp->sp_dma_wrlen);
    printf("sp_status: 0x%X\n", rsp->sp_status);
    printf("sp_dma_full: 0x%X\n", rsp->sp_dma_full);
    printf("sp_dma_busy: 0x%X\n", rsp->sp_dma_busy);
    printf("sp_semaphore: 0x%X\n\n", rsp->sp_semaphore);

    //print imem
    //print dmem
}
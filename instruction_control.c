#include "instruction_control.h"

//OP CODES
Instruction_Control opcode_table[64] = {
    [0x00] = {R_TYPE,   ALU,      NO_MEM,     execute_r_format},
    [0x01] = {TRAP,     SPECIAL,  NO_MEM,     execute_special_format},
    [0x02] = {J_TYPE,   JUMP,     NO_MEM,     J},
    [0x03] = {J_TYPE,   JUMP,     NO_MEM,     JAL},
    [0x04] = {J_TYPE,   BRANCH,   NO_MEM,     BEQ},
    [0x05] = {J_TYPE,   BRANCH,   NO_MEM,     BNEQ},
    [0x06] = {J_TYPE,   BRANCH,   NO_MEM,     BLEZ},
    [0x07] = {J_TYPE,   BRANCH,   NO_MEM,     BGTZ},
    [0x08] = {I_TYPE,   ALU,      NO_MEM,     ADDI},
    [0x09] = {I_TYPE,   ALU,      NO_MEM,     ADDIU},
    [0x0A] = {I_TYPE,   ALU,      NO_MEM,     SLTI},
    [0x0B] = {I_TYPE,   ALU,      NO_MEM,     SLTIU},
    [0x0C] = {I_TYPE,   ALU,      NO_MEM,     ANDI},
    [0x0D] = {I_TYPE,   ALU,      NO_MEM,     ORI},
    [0x0E] = {I_TYPE,   ALU,      NO_MEM,     XORI},
    [0x0F] = {I_TYPE,   ALU,      NO_MEM,     LUI},
    [0x10] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x11] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x12] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x13] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x14] = {J_TYPE,   BRANCH,   NO_MEM,     BEQL},
    [0x15] = {J_TYPE,   BRANCH,   NO_MEM,     BNEQL},
    [0x16] = {J_TYPE,   BRANCH,   NO_MEM,     BLEZL},
    [0x17] = {J_TYPE,   BRANCH,   NO_MEM,     BGTZL},
    [0x18] = {I_TYPE,   ALU,      NO_MEM,     DADDI},
    [0x19] = {I_TYPE,   ALU,      NO_MEM,     DADDIU},
    [0x1A] = {I_TYPE,   LOAD,     MEM_READ,   LDL},
    [0X1B] = {I_TYPE,   LOAD,     MEM_READ,   LDR},
    [0x1C] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x1D] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x1E] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x1F] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x20] = {I_TYPE,   LOAD,     MEM_READ,   LB},
    [0x21] = {I_TYPE,   LOAD,     MEM_READ,   LH},
    [0x22] = {I_TYPE,   LOAD,     MEM_READ,   LWL},
    [0X23] = {I_TYPE,   LOAD,     MEM_READ,   LW},
    [0x24] = {I_TYPE,   LOAD,     MEM_READ,   LBU},
    [0x25] = {I_TYPE,   LOAD,     MEM_READ,   LHU},
    [0x26] = {I_TYPE,   LOAD,     MEM_READ,   LWR},
    [0x27] = {I_TYPE,   LOAD,     MEM_READ,   LWU},
    [0X28] = {I_TYPE,   STORE,    MEM_WRITE,  SB},
    [0x29] = {I_TYPE,   STORE,    MEM_WRITE,  SH},
    [0x2A] = {I_TYPE,   STORE,    MEM_WRITE,  SWL},
    [0x2B] = {I_TYPE,   STORE,    MEM_WRITE,  SW},
    [0x2C] = {I_TYPE,   STORE,    MEM_WRITE,  SDL},
    [0x2D] = {I_TYPE,   STORE,    MEM_WRITE,  SDR},
    [0x2E] = {I_TYPE,   STORE,    MEM_WRITE,  SWR},
    [0x2F] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x30] = {I_TYPE,   LOAD,     MEM_READ,   LL},
    [0x31] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x32] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x33] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x34] = {I_TYPE,   LOAD,     MEM_READ,   LLD},
    [0x35] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x36] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x37] = {I_TYPE,   LOAD,     MEM_READ,   LD},
    [0x38] = {I_TYPE,   STORE,    MEM_WRITE,  SC},
    [0x39] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x3A] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x3B] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x3C] = {I_TYPE,   STORE,    MEM_WRITE,  SCD},
    [0x3D] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x3E] = {ERROR,    INVALID,  NO_MEM,     instruction_exception},
    [0x3F] = {I_TYPE,   STORE,    MEM_WRITE,  SD}
};

Instruction decode(uint32_t inst) {

    Instruction new_instruction;
    new_instruction.instruction = inst;
    new_instruction.opcode = (inst >> 26) & 0x3F;
    new_instruction.rs = (inst >> 21) & 0x1F;
    new_instruction.rt = (inst >> 16) & 0x1F;
    new_instruction.rd = (inst >> 11) & 0x1F;
    new_instruction.shamt = (inst >> 6) & 0x1F;
    new_instruction.function = inst & 0x3F;
    new_instruction.immediate = inst & 0xFFFF;

    return new_instruction;
}

int instruction_exception(CPU *cpu) {
    printf("instruction error\n");
    return 1;
}

void execute_r_format(CPU *cpu) {

    uint8_t function = cpu->pipeline.RFEX_READ.instruction.function;

    switch (function) {

        case 0x00: /*SLL*/
            cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rd] = 
            (cpu->gpr[cpu->pipeline.RFEX_READ.rt] << cpu->pipeline.RFEX_READ.instruction.shamt);
            break;
        case 0x02: /*SRL*/
            cpu->gpr[cpu->pipeline.RFEX_READ.rd] = 
            (cpu->gpr[cpu->pipeline.RFEX_READ.rt] >> cpu->pipeline.RFEX_READ.shamt); 
            break;

        case 0x03: /*SRA*/
            n64->cpu.gpr[rd] = (int64_t)n64->cpu.gpr[rt] >> shamt; 
            break;

        case 0x04: /*SLLV*/
            uint32_t shift_amt = n64->cpu.gpr[rs] & 0x1F;
            n64->cpu.gpr[rd] = n64->cpu.gpr[rt] << shift_amt;
            break;

        case 0x06: /*SRLV*/
            uint32_t shift_amt = n64->cpu.gpr[rs] & 0x1F;
            n64->cpu.gpr[rd] = n64->cpu.gpr[rt] >> shift_amt;
            break;

        case 0x07: /*SRAV*/
            uint32_t shift_amt = n64->cpu.gpr[rs] & 0x1F;
            n64->cpu.gpr[rd] = (int64_t) ((int32_t)n64->cpu.gpr[rt]) >> shift_amt;
            break;
                    
        case 0x08: //jump to address stored in rs
            char *rs_reg = register_names[rs];
            printf("%X  JR  %s\n", PC, rs_reg);
            PC += 4;
            break;

        case 0x09:

            switch(shamt) {

                case 0x1F: //jump to address stored in rs, store return in r31
                    char *rs_reg = register_names[rs];
                    printf("%X  JALR  %s\n", PC, rs_reg);
                    PC += 4;
                    break;

                default: //JALR when shamt is something else
                    char *rs_reg = register_names[rs];
                    char *rd_reg = register_names[rd];
                    printf("%X  JALR  %s, %s\n", PC, rd_reg, rs_reg);
                    PC += 4;
                    break;

            }

        case 0x0C:
            printf("%X  SYSCALL\n", PC);
            PC += 4;
            break;

        case 0x0D:
            printf("%X  BREAK\n", PC);
            PC += 4;
            break;

        case 0x10: /*MFHI*/
            n64->cpu.gpr[rd] = n64->cpu.HI;  //HAZARD, if any two instructions following mfhi modify hi
            break;
                    
        case 0x11: /*MTHI*/
            n64->cpu.HI = n64->cpu.gpr[rs]; 
            break;

        case 0x12: /*MFLO*/
            n64->cpu.gpr[rd] = n64->cpu.LO;  //HAZARD, if any two instructions following mfhi modify lo
            break;
                    
        case 0x13: /*MTLO*/
            n64->cpu.HL = n64->cpu.gpr[rs];  //HAZARD, if any two instructions following mfhi modify hi
            break;
                        
        case 0x14: /*DSLLV*/
            uint32_t shift_amt = n64->cpu.gpr[rs] & 0x3F;
            if(shift_amt > 63) {
                shift_amt = 63;
            }
            int64_t rt_val = n64->cpu.gpr[rt];
            int64_t result = rt_val << shift_amt;
            n64->cpu.gpr[rd] = result;
            break;

        case 0x16: /*DSRLV*/
                        uint32_t shift_amt = n64->cpu.gpr[rs] & 0x3F;
                        uint64_t rt_val = n64->cpu.gpr[rt];
                        uint64_t result = rt_val >> shift_amt;
                        n64->cpu.gpr[rd] = result;
                        break;

                    case 0x17: /*DSRAV*/
                        uint32_t shift_amt = n64->cpu.gpr[rs] & 0x3F;
                        int64_t rt_val = n64->cpu.gpr[rt];
                        int64_t result = rt_val >> shift_amt;
                        n64->cpu.gpr[rd] = result;
                        break;

                    case 0x18: /*MULT - 5 cycle stall*/
                        int32_t rs_val = n64->cpu.gpr[rs];
                        int32_t rt_val = n64->cpu.gpr[rt];
                        int64_t result = (int64_t)(rs_val * rt_val);
                        n64->cpu.LO = (uint64_t)(result & 0xFFFFFFFF);
                        n64->cpu.HI = (uint64_t)((result >> 32) & 0xFFFFFFFF);
                        break;

                    case 0x19: /*MULTU - 5 cycle stall*/
                        uint32_t rs_val = n64->cpu.gpr[rs];
                        uint32_t rt_val = n64->cpu.gpr[rt];
                        uint64_t result = (uint64_t)(rs_val * rt_val);
                        n64->cpu.LO = (uint64_t)(result & 0xFFFFFFFF);
                        n64->cpu.HI = (uint64_t)((result >> 32) & 0xFFFFFFFF);
                        break;

                    case 0x1A: /*DIV - 37 cycle stall*/
                        int32_t rs_val = n64->cpu.gpr[rs];
                        int32_t rt_val = n64->cpu.gpr[rt];

                        // Check for division by zero
                        if (rt_val == 0) {

                            n64->cpu.LO = 0; 
                            n64->cpu.HI = 0; 
                        } else {
                            int32_t quotient = rs_val / rt_val;
                            int32_t remainder = rs_val % rt_val;

                            n64->cpu.LO = (int64_t)quotient;
                            n64->cpu.HI = (int64_t)remainder;
                        }
                        break;

                    case 0x1B: /*DIVU - 37 cycle stall*/
                        uint32_t rs_val = n64->cpu.gpr[rs];
                        uint32_t rt_val = n64->cpu.gpr[rt];

                        if (rt_val == 0) {
                            n64->cpu.LO = 0; 
                            n64->cpu.HI = 0; 
                        } else {
                            uint32_t quotient = rs_val / rt_val;
                            uint32_t remainder = rs_val % rt_val;

                            n64->cpu.LO = (uint64_t)quotient;
                            n64->cpu.HI = (uint64_t)remainder;
                        }
                        break;

                    case 0x1C: /*DMULT - 8 cycle stall*/ 
                        int64_t rs_val = n64->cpu.gpr[rs];
                        int64_t rt_val = n64->cpu.gpr[rt];
                        __int128_t result = rs_val * rt_val;
                        n64->cpu.LO = (int64_t)result;
                        n64->cpu.HI = (int64_t) (result >> 64);
                        break;

                    case 0x1D: /*DMULTU - 8 cycle stall*/
                        uint64_t rs_val = n64->cpu.gpr[rs];
                        uint64_t rt_val = n64->cpu.gpr[rt];
                        __uint128_t result = rs_val * rt_val;
                        n64->cpu.LO = (uint64_t)result;
                        n64->cpu.HI = (uint64_t) (result >> 64);
                        break;

                    case 0x1E: /*DDIV - 69 cycle stall*/
                        int64_t rs_val = n64->cpu.gpr[rs];
                        int64_t rt_val = n64->cpu.gpr[rt];
                        
                        // Handle division by zero
                        if (rt_val == 0) {
                            n64->cpu.LO = 0;
                            n64->cpu.HI = 0;
                        } else {

                            int64_t quotient = rs_val / rt_val;
                            int64_t remainder = rs_val % rt_val;
                            
                            n64->cpu.LO = quotient;
                            n64->cpu.HI = remainder;
                        }
                        break;
                    
                    case 0x1F: /* DDIVU - 69 cycle stall*/
                        uint64_t rs_val = n64->cpu.gpr[rs];
                        uint64_t rt_val = n64->cpu.gpr[rt];
                        
                        // Handle division by zero
                        if (rt_val == 0) {
                            n64->cpu.LO = 0;
                            n64->cpu.HI = 0;
                        } else {

                            uint64_t quotient = rs_val / rt_val;
                            uint64_t remainder = rs_val % rt_val;
                            
                            n64->cpu.LO = quotient;
                            n64->cpu.HI = remainder;
                        }
                        break;

                    case 0x20: /*ADD*/
                        int32_t rs_val = n64->cpu.gpr[rs];
                        int32_t rt_val = n64->cpu.gpr[rt];
                        int32_t result = rs_val + rt_val;
                        if (((rs_val > 0) && (rt_val < 0) && (result < 0)) ||
                            ((rs_val < 0) && (rt_val > 0) && (result > 0))) {
                            /*handle errors and exceptions*/
                        } else {
                            n64->cpu.gpr[rd] = (int64_t) result;
                        }
                        break;

                    case 0x21: /*ADDU*/
                        uint32_t rs_val = n64->cpu.gpr[rs];
                        uint32_t rt_val = n64->cpu.gpr[rt];
                        uint32_t result = rs_val + rt_val;
                        
                        n64->cpu.gpr[rd] = (uint64_t) result;
                        break;

                    case 0x22: /*SUB*/
                        int32_t rs_val = n64->cpu.gpr[rs];
                        int32_t rt_val = n64->cpu.gpr[rt];
                        int32_t result = rt_val - rs_val;
                        if (((rs_val > 0) && (rt_val < 0) && (result < 0)) ||
                            ((rs_val < 0) && (rt_val > 0) && (result > 0))) {
                            /*handle errors and exceptions*/
                        } else {
                            n64->cpu.gpr[rd] = (int64_t) result;
                        }
                        break;

                    case 0x23: /*SUBU*/
                        uint32_t rs_val = n64->cpu.gpr[rs];
                        uint32_t rt_val = n64->cpu.gpr[rt];
                        uint32_t result = rs_val - rt_val;
                       
                        n64->cpu.gpr[rd] = (uint64_t) result;
                        break;
                    
                    case 0x24: /*AND*/
                        n64->cpu.gpr[rd] = n64->cpu.gpr[rs] & n64->cpu.gpr[rt];
                        break;
                    
                    case 0x25: /*OR*/
                        n64->cpu.gpr[rd] = n64->cpu.gpr[rs] | n64->cpu.gpr[rt];
                        break;
                    
                    case 0x26: /*XOR*/
                        n64->cpu.gpr[rd] = n64->cpu.gpr[rs] ^ n64->cpu.gpr[rt];
                        break;

                    case 0x27: /*NOR*/
                         n64->cpu.gpr[rd] = ~(n64->cpu.gpr[rs] | n64->cpu.gpr[rt]);
                        break;

                    case 0x2A: /*SLT*/
                        int32_t rs_val = n64->cpu.gpr[rs];
                        int32_t rt_val = n64->cpu.gpr[rt];
                        if(rs_val < rt_val) {
                            n64->cpu.gpr[rd] = 1;
                        } else {
                            n64->cpu.gpr[rd] = 0;
                        }
                        break;  
                    
                    case 0x2B: /*SLTU*/
                        uint32_t rs_val = n64->cpu.gpr[rs];
                        uint32_t rt_val = n64->cpu.gpr[rt];
                        if(rs_val < rt_val) {
                            n64->cpu.gpr[rd] = 1;
                        } else {
                            n64->cpu.gpr[rd] = 0;
                        }
                        break;

                    case 0x2C: /*DADD*/
                        int64_t rs_val = n64->cpu.gpr[rs];
                        int64_t rt_val = n64->cpu.gpr[rt];
                        int64_t result = rs_val + rt_val;

                        if (((rs_val > 0) && (rt_val > 0) && (result < 0)) ||
                            ((rs_val < 0) && (rt_val < 0) && (result > 0))) {
                            /*handle errors and exceptions*/
                        } else {
                            n64->cpu.gpr[rd] = result;
                        }
                        break;

                    case 0x2D: /*DADDU*/
                        uint64_t rs_val = n64->cpu.gpr[rs];
                        uint64_t rt_val = n64->cpu.gpr[rt];
                        uint64_t result = rs_val + rt_val;
                        n64->cpu.gpr[rd] = result;
                        break;
                    
                    case 0x2E: /*DSUB*/
                        int64_t rs_val = n64->cpu.gpr[rs];
                        int64_t rt_val = n64->cpu.gpr[rt];
                        int64_t result = rs_val - rt_val;
                        if (((rs_val > 0) && (rt_val < 0) && (result < 0)) ||
                            ((rs_val < 0) && (rt_val > 0) && (result > 0))) {
                            /*handle errors and exceptions*/
                        } else {
                            n64->cpu.gpr[rd] = result;
                        }
                        break;

                    case 0x2F: /*DSUBU*/
                        uint64_t rs_val = n64->cpu.gpr[rs];
                        uint64_t rt_val = n64->cpu.gpr[rt];
                        uint64_t result = rs_val - rt_val;
                        n64->cpu.gpr[rd] = result;
                        break;

                    case 0x30: //if rs >= rt, cause trap exception
                        char *rs_reg = register_names[rs];
                        char *rt_reg = register_names[rt];
                        printf("%X  TGE  %s, %s\n", PC, rs_reg, rt_reg);
                        PC += 4;
                        break;
                    
                    case 0x31: //if unsigned rs >= unsigned rt, cause trap exception
                        char *rs_reg = register_names[rs];
                        char *rt_reg = register_names[rt];
                        printf("%X  TGEU  %s, %s\n", PC, rs_reg, rt_reg);
                        PC += 4;
                        break;

                    case 0x32: //if rs < rt, cause trap exception
                        char *rs_reg = register_names[rs];
                        char *rt_reg = register_names[rt];
                        printf("%X  TLT  %s, %s\n", PC, rs_reg, rt_reg);
                        PC += 4;
                        break;  

                    case 0x33: //if unsigned rs < unsigned rt, cause trap exception
                        char *rs_reg = register_names[rs];
                        char *rt_reg = register_names[rt];
                        printf("%X  TLTU  %s, %s\n", PC, rs_reg, rt_reg);
                        PC += 4;
                        break;
                    
                    case 0X34: //if rs == rt cause trap exception
                        char *rs_reg = register_names[rs];
                        char *rt_reg = register_names[rt];
                        printf("%X  TEQ  %s, %s\n", PC, rs_reg, rt_reg);
                        PC += 4;
                        break;
                    
                    case 0x36: //trap not equal
                        char *rs_reg = register_names[rs];
                        char *rt_reg = register_names[rt];
                        printf("%X  TNE  %s, %s\n", PC, rs_reg, rt_reg);
                        PC += 4;
                        break;
                    
                    case 0x38: /*DSLL*/
                        int64_t rt_val = n64->cpu.gpr[rt];
                        int64_t result = rt_val << shamt;
                        n64->cpu.gpr[rd] = result;
                        break;

                    case 0x3A: /*DSRL*/
                        uint64_t rt_val = n64->cpu.gpr[rt];
                        uint64_t result = rt_val >> shamt;
                        n64->cpu.gpr[rd] = result;
                        break;

                    case 0x3B: /*DSRA*/
                        int64_t rt_val = n64->cpu.gpr[rt];
                        int64_t result = rt_val >> shamt;
                        n64->cpu.gpr[rd] = result;
                        break;
                    
                    case 0x3C: /*DSLL32*/
                        int64_t rt_val = n64->cpu.gpr[rt];
                        int64_t result = rt_val << (32 + shamt);
                        n64->cpu.gpr[rd] = result;
                        break;
                    
                    case 0x3E: /*DSRL32*/
                        uint64_t rt_val = n64->cpu.gpr[rt];
                        uint64_t result = rt_val >> (32 + shamt);
                        n64->cpu.gpr[rd] = result;
                        break;

                    case 0x3F: /*DSRA32*/
                        int64_t rt_val = n64->cpu.gpr[rt];
                        int64_t result = rt_val >> (32 + shamt);
                        n64->cpu.gpr[rd] = result;
                        break;
                default:
                    printf("ERROR: UNKNOWN INSTRUCTION\n");
                    PC += 4;
                    break;
                }
}

void J(CPU *cpu) {

    cpu->delay_slot = cpu->PC + 4;

    uint32_t j_target = cpu->pipeline.RFEX_READ.instruction.instruction & 0x03FFFFFF; //make this a separate field that gets masked if its a jump instruction
    j_target = j_target << 2;
    cpu->PC = (cpu->delay_slot & 0xF0000000) | j_target;
}

void JAL(CPU* cpu) {

    cpu->delay_slot = cpu->PC + 4;
    cpu->gpr[31] = cpu->delay_slot + 4;
    
    uint32_t j_target = cpu->pipeline.RFEX_READ.instruction.instruction & 0x03FFFFFF;
    j_target = j_target << 2;
    cpu->PC = (cpu->delay_slot & 0xF0000000) | j_target;
}

void BEQ(CPU *cpu) {

    cpu->delay_slot = cpu->PC + 4;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.instruction.SEOffset;
    uint32_t b_target = ((extended_offset << 2) + cpu->delay_slot);

    if(cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rs] == cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt]) {
        cpu->PC = cpu->PC + b_target;
    }
}

void BNEQ(CPU *cpu) {

    cpu->delay_slot = cpu->PC + 4;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.instruction.SEOffset;
    uint32_t b_target = ((extended_offset << 2) + cpu->delay_slot);

    if(cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rs] != cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt]) {
        cpu->PC = cpu->PC + b_target;
    }
}

void BLEZ(CPU *cpu) {
    cpu->delay_slot = cpu->PC + 4;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.instruction.SEOffset;
    uint32_t b_target = ((extended_offset << 2) + cpu->delay_slot);

    if(cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rs] <= 0) {
        cpu->PC = cpu->PC + b_target;
    }
}

void BGTZ(CPU *cpu) {
    cpu->delay_slot = cpu->PC + 4;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.instruction.SEOffset;
    uint32_t b_target = ((extended_offset << 2) + cpu->delay_slot);

    if(cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rs] > 0) {
        cpu->PC = cpu->PC + b_target;
    }
}

void ADDI(CPU *cpu) {
    
    int64_t SE_Immediate = (int64_t)cpu->pipeline.RFEX_READ.instruction.immediate;
    int64_t rs_val = (int64_t)cpu.pipeline.RFEX_READ.instruction.rs_val;
    int64_t result = rs_val + SE_Immediate;

    //TODO: handle int overflow...
    if ((rs_val > 0 && SE_Immediate > 0 && result < 0) || 
        (rs_val < 0 && SE_Immediate < 0 && result > 0)) {
        //handle_overflow_exception(cpu);
        printf("addi overflow :(\n");
        return 1;
    }
    cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = (uint64_t)result;                  
}

void ADDIU(CPU *cpu) {

    int16_t immediate = cpu->pipeline.RFEX_READ.instruction.immediate;
    int64_t SE_Immediate = (int64_t)immediate;
    int64_t rs_val = (int64_t)cpu.pipeline.RFEX_READ.instruction.rs_val;
    int64_t result = rs_val + SE_Immediate;

    cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = (uint64_t)result; 
}

void SLTI(CPU *cpu) {

    int64_t SE_Immediate = (int64_t)cpu->pipeline.RFEX_READ.instruction.immediate;
    int64_t rs_val = (int64_t)cpu->pipeline.RFEX_READ.instruction.rs_val;
    if(rs_val < SE_immediate) {
        cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = 1;
    } else {
        cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = 0;
    }
}

void SLTIU(CPU *cpu) {

    int64_t SE_Immediate = (int64_t)cpu->pipeline.RFEX_READ.instruction.immediate;
    uint64_t rs_val = cpu->pipeline.RFEX_READ.instruction.rs_val;

    if(rs_val < (uint64_t)SE_immediate) {
        cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = 1;
    } else {
        cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = 0;
    }
}

void ANDI(CPU *cpu) {

    uint64_t ZE_Immediate = (uint64_t)cpu->pipeline.RFEX_READ.instruction.immediate;
    uint64_t rs_val = cpu->pipeline.RFEX_READ.instruction.rs_val;
    uint64_t result = rs_val & ZE_Immediate;

    cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = result;   
}

void ORI(CPU *cpu) {

    uint64_t ZE_Immediate = (uint64_t)cpu->pipeline.RFEX_READ.instruction.immediate;
    uint64_t rs_val = cpu->pipeline.RFEX_READ.instruction.rs_val;
    uint64_t result = rs_val | ZE_Immediate;

    cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = result; 
}

void XORI(CPU *cpu) {

    uint64_t ZE_Immediate = (uint64_t)cpu->pipeline.RFEX_READ.instruction.immediate;
    uint64_t rs_val = cpu->pipeline.RFEX_READ.instruction.rs_val;
    uint64_t result = rs_val ^ ZE_Immediate;

    cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = result; 
}

void LUI(CPU *cpu) {

    int16_t immediate = cpu->pipeline.RFEX_READ.instruction.immediate;
    int64_t SE_immediate = (int64_t)immediate << 16;
    cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = (uint64_t)SE_immediate;
}

void BEQL(CPU *cpu) {

    cpu->delay_slot = cpu->PC + 4;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.instruction.SEOffset;
    uint32_t b_target = ((extended_offset << 2) + cpu->delay_slot);

    if(cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rs] == cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt]) {
        cpu->PC = cpu->PC + b_target;
    }
}

void BNEQL(CPU *cpu) {

    cpu->delay_slot = cpu->PC + 4;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.instruction.SEOffset;
    uint32_t b_target = ((extended_offset << 2) + cpu->delay_slot);

    if(cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rs] != cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt]) {
        cpu->PC = cpu->PC + b_target;
    }
}

void BLEZL(CPU *cpu) {

    cpu->delay_slot = cpu->PC + 4;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.instruction.SEOffset;
    uint32_t b_target = ((extended_offset << 2) + cpu->delay_slot);

    if(cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rs] <= 0) {
        cpu->PC = cpu->PC + b_target;
    }
}

void BGTZL(CPU *cpu) {

    cpu->delay_slot = cpu->PC + 4;
    int32_t extended_offset = cpu->pipeline.RFEX_READ.instruction.SEOffset;
    uint32_t b_target = ((extended_offset << 2) + cpu->delay_slot);

    if(cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rs] > 0) {
        cpu->PC = cpu->PC + b_target;
    }
}

void DADDI(CPU *cpu) {

    int64_t SE_Immediate = (int64_t)cpu->pipeline.RFEX_READ.instruction.immediate;
    int64_t result = SE_Immediate + cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rs];
    if((result > INT64_MAX) || (result < INT64_MIN)) {
    /*handle errors and exceptions*/
    } else {
        cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = (uint64_t)result;
    }
}

void DADDIU(CPU *cpu) {

    int64_t SE_Immediate = (int64_t)cpu->pipeline.RFEX_READ.instruction.immediate;
    int64_t result = SE_Immediate + cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rs];
    
    cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rt] = (uint64_t)result;
}

void LDL(CPU *cpu) {

    //incomplete until i know i need to finish it
    int64_t SE_Offset = (int64_t)cpu->pipeline.RFEX_READ.instruction.SEOffset;
    uint64_t base = cpu->gpr[cpu->pipeline.RFEX_READ.instruction.rs];
    int64_t target = base + SE_Offset;

    int byte_offset = target % 8; 
    int bytes_to_load = 8 - byte_offset;
    //this is where ALU_Result would get stored!
    printf("LDL HAPPENING LOL\n");
}

void LDR(CPU *cpu) {

    //incomplete until i know i need to finish it
    printf("LDR HAPPENING LOL\n");
}

LB
LH
LWL
LW
LBU
LHU
LWR
LWU
SB
SH
SWL
SW
SDL
SDR
SWR

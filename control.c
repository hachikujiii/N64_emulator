#include "control.h"

//OP CODES
Control flags[64] = {
    [0x00] = {R_TYPE,   ALU,      NO_MEM},
    [0x01] = {TRAP,     SPECIAL,  NO_MEM},
    [0x02] = {J_TYPE,   JUMP,     NO_MEM},
    [0x03] = {J_TYPE,   JUMP,     NO_MEM},
    [0x04] = {J_TYPE,   BRANCH,   NO_MEM},
    [0x05] = {J_TYPE,   BRANCH,   NO_MEM},
    [0x06] = {J_TYPE,   BRANCH,   NO_MEM},
    [0x07] = {J_TYPE,   BRANCH,   NO_MEM},
    [0x08] = {I_TYPE,   ALU,      NO_MEM},
    [0x09] = {I_TYPE,   ALU,      NO_MEM},
    [0x0A] = {I_TYPE,   ALU,      NO_MEM},
    [0x0B] = {I_TYPE,   ALU,      NO_MEM},
    [0x0C] = {I_TYPE,   ALU,      NO_MEM},
    [0x0D] = {I_TYPE,   ALU,      NO_MEM},
    [0x0E] = {I_TYPE,   ALU,      NO_MEM},
    [0x0F] = {I_TYPE,   ALU,      NO_MEM},
    [0x10] = {ERROR,    INVALID,  NO_MEM},
    [0x11] = {ERROR,    INVALID,  NO_MEM},
    [0x12] = {ERROR,    INVALID,  NO_MEM},
    [0x13] = {ERROR,    INVALID,  NO_MEM},
    [0x14] = {J_TYPE,   BRANCH,   NO_MEM},
    [0x15] = {J_TYPE,   BRANCH,   NO_MEM},
    [0x16] = {J_TYPE,   BRANCH,   NO_MEM},
    [0x17] = {J_TYPE,   BRANCH,   NO_MEM},
    [0x18] = {I_TYPE,   ALU,      NO_MEM},
    [0x19] = {I_TYPE,   ALU,      NO_MEM},
    [0x1A] = {I_TYPE,   LOAD,     MEM_READ},
    [0X1B] = {I_TYPE,   LOAD,     MEM_READ},
    [0x1C] = {ERROR,    INVALID,  NO_MEM},
    [0x1D] = {ERROR,    INVALID,  NO_MEM},
    [0x1E] = {ERROR,    INVALID,  NO_MEM},
    [0x1F] = {ERROR,    INVALID,  NO_MEM},
    [0x20] = {I_TYPE,   LOAD,     MEM_READ},
    [0x21] = {I_TYPE,   LOAD,     MEM_READ},
    [0x22] = {I_TYPE,   LOAD,     MEM_READ},
    [0X23] = {I_TYPE,   LOAD,     MEM_READ},
    [0x24] = {I_TYPE,   LOAD,     MEM_READ},
    [0x25] = {I_TYPE,   LOAD,     MEM_READ},
    [0x26] = {I_TYPE,   LOAD,     MEM_READ},
    [0x27] = {I_TYPE,   LOAD,     MEM_READ},
    [0X28] = {I_TYPE,   STORE,    MEM_WRITE},
    [0x29] = {I_TYPE,   STORE,    MEM_WRITE},
    [0x2A] = {I_TYPE,   STORE,    MEM_WRITE},
    [0x2B] = {I_TYPE,   STORE,    MEM_WRITE},
    [0x2C] = {I_TYPE,   STORE,    MEM_WRITE},
    [0x2D] = {I_TYPE,   STORE,    MEM_WRITE},
    [0x2E] = {I_TYPE,   STORE,    MEM_WRITE},
    [0x2F] = {ERROR,    INVALID,  NO_MEM},
    [0x30] = {I_TYPE,   LOAD,     MEM_READ},
    [0x31] = {ERROR,    INVALID,  NO_MEM},
    [0x32] = {ERROR,    INVALID,  NO_MEM},
    [0x33] = {ERROR,    INVALID,  NO_MEM},
    [0x34] = {I_TYPE,   LOAD,     MEM_READ},
    [0x35] = {ERROR,    INVALID,  NO_MEM},
    [0x36] = {ERROR,    INVALID,  NO_MEM},
    [0x37] = {I_TYPE,   LOAD,     MEM_READ},
    [0x38] = {I_TYPE,   STORE,    MEM_WRITE},
    [0x39] = {ERROR,    INVALID,  NO_MEM},
    [0x3A] = {ERROR,    INVALID,  NO_MEM},
    [0x3B] = {ERROR,    INVALID,  NO_MEM},
    [0x3C] = {I_TYPE,   STORE,    MEM_WRITE},
    [0x3D] = {ERROR,    INVALID,  NO_MEM},
    [0x3E] = {ERROR,    INVALID,  NO_MEM},
    [0x3F] = {I_TYPE,   STORE,    MEM_WRITE}
};

Control set_flags(uint8_t opcode) {
    Control set = flags[opcode];
    return set;
}

uint32_t byte_swap(uint32_t inst) {
    return ((inst & 0xFF) << 24) | ((inst & 0xFF00) << 8) |
           ((inst & 0xFF0000) >> 8) | ((inst & 0xFF000000) >> 24);
}

Instruction decode(uint32_t inst) {

    inst = byte_swap(inst);
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
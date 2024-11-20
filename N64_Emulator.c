#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "my_structs.h"
#include "n64_pipeline.h"
#include "mapping.h"
#include "debug_sys.h"
#include "page_table.h"
#include "instruction_control.c"
#include "instruction_control.h"


/*HAZARD LIST:
    MFHI
    MFLO
*/

void IC_stage(unsigned char* rom, N64 *n64);
void RF_stage(CPU *cpu);
void EX_stage(N64 *n64);
bool inRange( uint64_t address, uint64_t start, uint64_t end);
uint32_t map_virtual_address(CPU *cpu, uint32_t virtual_address);
void Copy_write_to_read(CPU *cpu);
int load_roms(N64 **n64, Cartridge *cartridge);
void boot_sim(N64 *n64);
uint32_t direct_map(uint32_t virtual_address, uint32_t segment_start);

int main(int argc, char *argv[]) {

    N64 *n64 = NULL;
    Cartridge cartridge;

    //if (argc < 2) {
    //    fprintf(stderr, "Usage: %s <rom_file>\n", argv[0]);
    //    return 1;
    //}

    if (load_roms(&n64, &cartridge) != 0) {
        printf("Error initializing\n");
        return 1;
    }
    printf("initializing complete!\n");
    memset(n64->cpu.gpr, 0, sizeof(n64->cpu.gpr));
    memset(n64->cpu.fpr, 0, sizeof(n64->cpu.fpr));
    boot_sim(n64);

    n64->cpu.PC = 0xBFC00000;
    print_cpu(n64);

    return 1;
    while(true) {
            
        //initialize
            
        //pif, bootloader, pc, set all specific register values and control signals

        IC_stage(cartridge.ROM, n64);
        RF_stage(&n64->cpu);
        EX_stage(n64);
        //DC_stage(n64);
        //WB_stage(n64);
        //print
        //ReadToWrite();

        //special cases
        // ERET  0100 0010 0000 0000 0000 0000 0001 1000

    }     

}

int load_roms(N64 **n64, Cartridge *cartridge) {

    cartridge->ROM = (unsigned char *)malloc(ROM_SIZE * sizeof(unsigned char));
    if (cartridge->ROM == NULL) {
        fprintf(stderr, "Memory allocation for ROM failed\n");
        return -1;
    }

    //open rom dump file (.bin)
    FILE *binfile = fopen("OcarinaOfTime.bin", "rb");
    if(!binfile) {
        fprintf(stderr, "Failed to open .bin file");
        free(cartridge->ROM);
        return -1;
    }

    size_t bytesRead = fread(cartridge->ROM, sizeof(unsigned char), ROM_SIZE, binfile);
    if (bytesRead != ROM_SIZE) {
        //Handle incomplete read
        fprintf(stderr, "Failed to read the entire cartridge file\n");
        fclose(binfile);
        free(cartridge->ROM);
        return -1;
    }
    fclose(binfile);

    *n64 = (N64*) malloc(sizeof(N64));
    if (*n64 == NULL) {
        fprintf(stderr, "memory allocation for N64 failed\n");
        free(cartridge->ROM);
        return -1;
    }

    (*n64)->pif_rom.data = (unsigned char *)malloc(PIF_ROM_SIZE * sizeof(unsigned char));
    if((*n64)->pif_rom.data == NULL) {
        fprintf(stderr, "Memory allocation for PIF ROM failed\n");
        free(cartridge->ROM);
        free(*n64);
        return -1;
    }

    FILE *pif_file = fopen("n64_pif.bin", "rb");
    if(!pif_file) {
        fprintf(stderr, "Failed to open pif rom file");
        free(cartridge->ROM);
        free(*n64);
        return -1;
    }

    size_t pif_bytes_read = fread((*n64)->pif_rom.data, sizeof(unsigned char), PIF_ROM_SIZE, pif_file);
    if(pif_bytes_read != PIF_ROM_SIZE) {
        fprintf(stderr, "Failed to read PIF ROM\n");
        fclose(pif_file);
        free(cartridge->ROM);
        free(*n64);
        return -1;
    }
    fclose(pif_file);
    
    printf("loaded rom\n");
    return 0; //success
}

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

    n64->cpu.pipeline.EXDC_WRITE.SWValue = n64->cpu.pipeline.RFEX_READ.instruction.rt_val;

    //use both readRegValues
    if (n64->cpu.pipeline.RFEX_READ.ALUSrc == 0) { 

        if (n64->cpu.pipeline.RFEX_READ.instruction.function == 0x20) { //add

            n64->cpu.pipeline.EXDC_WRITE.ALUResult = (n64->cpu.pipeline.RFEX_READ.instruction.rs_val + n64->cpu.pipeline.RFEX_READ.instruction.rt_val);

        } else if (n64->cpu.pipeline.RFEX_READ.instruction.function == 0x22) { //subtract

            n64->cpu.pipeline.EXDC_WRITE.ALUResult = (n64->cpu.pipeline.RFEX_READ.instruction.rs_val - n64->cpu.pipeline.RFEX_READ.instruction.rt_val);

        } else {

            n64->cpu.pipeline.EXDC_WRITE.ALUResult = 0; // used to pass NOPs
        }

    //use readReg1 and SEOffset
    } else if (n64->cpu.pipeline.RFEX_READ.ALUSrc == 1) {

        n64->cpu.pipeline.EXDC_WRITE.ALUResult = (n64->cpu.pipeline.RFEX_READ.instruction.rs_val + (int)n64->cpu.pipeline.RFEX_READ.instruction.SEOffset); //casted to int to sign extend
    } 

    //pass correct writeReg
    if (n64->cpu.pipeline.RFEX_READ.RegDst == 0) {

        n64->cpu.pipeline.EXDC_WRITE.WriteRegNum = n64->cpu.pipeline.RFEX_READ.instruction.rt;

    } else if (n64->cpu.pipeline.RFEX_READ.RegDst == 1) {

        n64->cpu.pipeline.EXDC_WRITE.WriteRegNum = n64->cpu.pipeline.RFEX_READ.instruction.rd;
    }
}

void boot_sim(N64 *n64) {

    //hardcode values to simulate PIF ROM
    n64->cpu.gpr[11] = 0xFFFFFFFFA4000040;
    n64->cpu.gpr[20] = 0x0000000000000001;
    n64->cpu.gpr[22] = 0x000000000000003F;
    n64->cpu.gpr[29] = 0xFFFFFFFFA4000040;

    //hardcode values in COP0
    n64->cpu.cp0.Random = 0x0000001F;
    n64->cpu.cp0.Status = 0x34000000;
    n64->cpu.cp0.PRId =   0x00000B00;
    n64->cpu.cp0.Config = 0x0006E463;
}

bool inRange (uint64_t address, uint64_t start, uint64_t end) {

    return start <= address && address <= end;
}

uint32_t map_virtual_address(CPU *cpu, uint32_t virtual_address) {

    if (inRange(virtual_address, KUSEG_START, KUSEG_END)) {
        //User segment TLB mapped
        return tlb_lookup(cpu, virtual_address);

    } else if (inRange(virtual_address, KSEG0_START, KSEG0_END)) {
        //Kernel segment 0. direct mapped, cached
        return direct_map(virtual_address, KSEG0_START);

    } else if (inRange(virtual_address, KSEG1_START, KSEG1_END)) {
        //Kernel segment 1, direct mapped, no cache
        return direct_map(virtual_address, KSEG1_START);

    } else if (inRange(virtual_address, KSSEG_START, KSSEG_END)) {
        //Kernel supervisor segment. TLB mapped
        return tlb_lookup(cpu, virtual_address);

    } else if (inRange(virtual_address, KSEG3_START, KSEG3_END)) {
        //kernel segment 3. TLB mapped 
        return tlb_lookup(cpu, virtual_address);
    }

    //invalid address
    return -1;
}

uint32_t direct_map(uint32_t virtual_address, uint32_t segment_start) {
    return virtual_address - segment_start;
}

//uint32_t map_physical_address(CPU *cpu, uint32_t physical_address) {

    //if (inRange(physical_address, RDRAM_START, RDRAM_END)) {
        //RDRAM - built in
    //} else if (inRange(physical_address, RDRAMX_START, RDRAMX_END)) {
        //RDRAM - expansion pack
    //} else if (inRange(physical_address, RDRAM_REG_START, RDRAM_REG_END)) {
        //rdram mmio - configures timings, maybe irrelevant
    //} else if (inRange(physical_address, SPDMEM_START, SPDMEM_END)) {
        //RSP data mem
    //} else if (inRange(physical_address, SPIMEM_START, SPIMEM_END)) {
        //RSP instruction mem
    //} else if (inRange(physical_address, SP_REG_START, SP_REG_END)) {
        //control RSP DMA engine, status, program counter
    //} else if (inRange(physical_address, DPCOM_REG_START, DPCOM_REG_END)) {
        //send commands to RDP
    //} else if (inRange(physical_address, DPSPAN_REG_START, DPSPAN_REG_END)) {
        //UNKNOWN
    //} else if (inRange(physical_address, MI_REG_START, MI_REG_END)) {
        //MIPS INTERFACE, SYSTEM INFO, INTERRUPTS
    //} else if (inRange(physical_address, VI_REG_START, VI_REG_END)) {
        //video interface, screen res, framebuff settings
    //} else if (inRange(physical_address, AI_REG_START, AI_REG_END)) {
        //audio subsystem
    //} else if (inRange(physical_address, PI_REG_START, PI_REG_END)) {
        //peripheral interface, set up DMAs cart <==> RDRAM
    //} else if (inRange(physical_address, RI_REG_START, RI_REG_END)) {
        //rdram settings, irrelevant for emulation?
    //} else if (inRange(physical_address, SI_REG_START, SI_REG_END)) {
        //control pif ram <==> rdram dma engine
    //} else if (inRange(physical_address, CART_D2A1_START, CART_D2A1_END)) {
        //n64DD control reg, returns open bus or all 0xff when not present
    //} else if (inRange(physical_address, CART_D1A1_START, CART_D1A1_END)) {
        //n64DD IPL ROM - returns open bus or all 0xff when not present
    //} else if (inRange(physical_address, CART_D2A2_START, CART_D2A2_END)) {
        //SRAM is mapped here
    //} else if (inRange(physical_address, CART_D1A2_START, CART_D1A2_END)) {
        //ROM is mapped here
    //} else if (inRange(physical_address, PIF_BOOT_ROM_START, PIF_BOOT_ROM_END)) {
        //first code run on boot, baked into hardware
    //} else if (inRange(physical_address, PIF_RAM_START, PIF_RAM_END)) {
        //used to communicate with PIF chip (controller and mem cards)
    //} else if (inRange(physical_address, CART_D1A3_START, CART_D1A3_END)) {
        //
    //} else if (inRange(physical_address, EXSYS_DEV_START, EXSYS_DEV_END)) {

    //}
//}

void Copy_write_to_read(CPU *cpu) {

    cpu->pipeline.ICRF_READ.instruction = cpu->pipeline.ICRF_WRITE.instruction;

    cpu->pipeline.RFEX_READ.RegDst = cpu->pipeline.RFEX_WRITE.RegDst;
    cpu->pipeline.RFEX_READ.ALUSrc = cpu->pipeline.RFEX_WRITE.ALUSrc;

    cpu->pipeline.RFEX_READ.instruction = cpu->pipeline.RFEX_WRITE.instruction;
    cpu->pipeline.RFEX_READ.control = cpu->pipeline.RFEX_WRITE.control;
    
    cpu->pipeline.EXDC_READ.instruction = cpu->pipeline.EXDC_WRITE.instruction;
    cpu->pipeline.EXDC_READ.control = cpu->pipeline.EXDC_WRITE.control;
    cpu->pipeline.EXDC_READ.ALUResult = cpu->pipeline.EXDC_WRITE.ALUResult;
    cpu->pipeline.EXDC_READ.SWValue = cpu->pipeline.EXDC_WRITE.SWValue;
    cpu->pipeline.EXDC_READ.WriteRegNum = cpu->pipeline.EXDC_WRITE.WriteRegNum;

    cpu->pipeline.DCWB_READ.instruction = cpu->pipeline.DCWB_WRITE.instruction;
    cpu->pipeline.DCWB_READ.control = cpu->pipeline.DCWB_WRITE.control;
    cpu->pipeline.DCWB_READ.MemToReg = cpu->pipeline.DCWB_WRITE.MemToReg;
    cpu->pipeline.DCWB_READ.RegWrite = cpu->pipeline.DCWB_WRITE.RegWrite;
    cpu->pipeline.DCWB_READ.ALUResult = cpu->pipeline.DCWB_WRITE.ALUResult;
    cpu->pipeline.DCWB_READ.LWDataValue = cpu->pipeline.DCWB_WRITE.LWDataValue;
    cpu->pipeline.DCWB_READ.WriteRegNum = cpu->pipeline.DCWB_WRITE.WriteRegNum;
}
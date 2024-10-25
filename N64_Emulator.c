#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "my_structs.h"
#include "n64_pipeline.h"
#include "mapping.h"
#include "debug_sys.h"


const char* register_names[] = {
    "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
    "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
    "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
    "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};


/*HAZARD LIST:
    MFHI
    MFLO
*/

void IC_stage(unsigned char* rom, N64 *n64);
void RF_stage(CPU *cpu);
void EX_stage(CPU *cpu);
bool inRange( uint32_t address, uint32_t start, uint32_t end);
//uint32_t map_virtual_address(CPU *cpu, uint32_t virtual_address);
void Copy_write_to_read(CPU *cpu);
int load_roms(N64 **n64, Cartridge *cartridge);
void boot_sim(N64 *n64);

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
        //EX_stage(n64);
        //MEM_stage(n64);
        //WB_stage(n64);  
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
        return -11;
    }
    fclose(pif_file);
    
    printf("loaded rom\n");
    return 0; //success
}

void IC_stage(unsigned char *rom, N64 *n64) {

    //n64->cpu.pipeline.IFID_WRITE.instruction = testing[i];
    n64->cpu.pipeline.ICRF_WRITE.instruction = *(uint32_t *)(rom + n64->cpu.PC);
}

void RF_stage(CPU *cpu) {

    cpu->pipeline.RFEX_WRITE.opcode = (cpu->pipeline.ICRF_READ.instruction >> 26) & 0x3F;
    cpu->pipeline.RFEX_WRITE.rs = (cpu->pipeline.ICRF_READ.instruction >> 21) & 0x1F;
    cpu->pipeline.RFEX_WRITE.rt = (cpu->pipeline.ICRF_READ.instruction >> 16) & 0x1F;
    cpu->pipeline.RFEX_WRITE.rd = (cpu->pipeline.ICRF_READ.instruction >> 11) & 0x1F;
    cpu->pipeline.RFEX_WRITE.shamt = (cpu->pipeline.ICRF_READ.instruction >> 6) & 0x1F;
    cpu->pipeline.RFEX_WRITE.function = cpu->pipeline.ICRF_READ.instruction & 0x3F;
    cpu->pipeline.RFEX_WRITE.immediate = cpu->pipeline.ICRF_READ.instruction & 0xFFFF;
    cpu->pipeline.RFEX_WRITE.rs_val = cpu->gpr[cpu->pipeline.RFEX_WRITE.rs];
    cpu->pipeline.RFEX_WRITE.rt_val = cpu->gpr[cpu->pipeline.RFEX_WRITE.rt];
    cpu->pipeline.RFEX_WRITE.SEOffset = (int32_t)(int16_t)cpu->pipeline.RFEX_WRITE.immediate;
    cpu->pipeline.RFEX_WRITE.targetaddr = cpu->PC + 4 + (cpu->pipeline.RFEX_WRITE.SEOffset << 2);

    //condition to handle NOPs
    if (cpu->pipeline.RFEX_WRITE.opcode == 0 && cpu->pipeline.RFEX_WRITE.rs == 0 && 
        cpu->pipeline.RFEX_WRITE.rt == 0 && cpu->pipeline.RFEX_WRITE.SEOffset == 0) { //if the whole instruction is 0

        cpu->pipeline.RFEX_WRITE.RegDst = 0;
        cpu->pipeline.RFEX_WRITE.MemRead = 0;
        cpu->pipeline.RFEX_WRITE.MemToReg = 0;
        cpu->pipeline.RFEX_WRITE.ALUOp = 0;
        cpu->pipeline.RFEX_WRITE.MemWrite = 0;
        cpu->pipeline.RFEX_WRITE.ALUSrc = 0;
        cpu->pipeline.RFEX_WRITE.RegWrite = 0;

    } else if (cpu->pipeline.RFEX_WRITE.opcode == 0) { //set control bits for R-Type instructions

        cpu->pipeline.RFEX_WRITE.RegDst = 1;
        cpu->pipeline.RFEX_WRITE.MemRead = 0;
        cpu->pipeline.RFEX_WRITE.MemToReg = 0;
        cpu->pipeline.RFEX_WRITE.ALUOp = 10;
        cpu->pipeline.RFEX_WRITE.MemWrite = 0;
        cpu->pipeline.RFEX_WRITE.ALUSrc = 0;
        cpu->pipeline.RFEX_WRITE.RegWrite = 1;
        
    } else if (cpu->pipeline.RFEX_WRITE.opcode == 0x20) { //load byte

            cpu->pipeline.RFEX_WRITE.RegDst = 0;
            cpu->pipeline.RFEX_WRITE.MemRead = 1;
            cpu->pipeline.RFEX_WRITE.MemToReg = 1;
            cpu->pipeline.RFEX_WRITE.ALUOp = 00;
            cpu->pipeline.RFEX_WRITE.MemWrite = 0;
            cpu->pipeline.RFEX_WRITE.ALUSrc = 1;
            cpu->pipeline.RFEX_WRITE.RegWrite = 1;

        } else if (cpu->pipeline.RFEX_WRITE.opcode == 0x28) { //store byte

                cpu->pipeline.RFEX_WRITE.RegDst = 0;
                cpu->pipeline.RFEX_WRITE.MemRead = 0;
                cpu->pipeline.RFEX_WRITE.MemToReg = 0;
                cpu->pipeline.RFEX_WRITE.ALUOp = 00;
                cpu->pipeline.RFEX_WRITE.MemWrite = 1;
                cpu->pipeline.RFEX_WRITE.ALUSrc = 1;
                cpu->pipeline.RFEX_WRITE.RegWrite = 0;
        }
}

void MEM_stage(uint32_t address, N64 *n64) {

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

bool inRange (uint32_t address, uint32_t start, uint32_t end) {

    return start <= address && address <= end;
}

//uint32_t map_virtual_address(CPU *cpu, uint32_t virtual_address) {

    //if (inRange(virtual_address, KUSEG_START, KUSEG_END)) {
        //User segment TLB mapped
    //} else if (inRange(virtual_address, KSEG0_START, KSEG0_END)) {
        //Kernel segment 0. direct mapped, cached
    //} else if (inRange(virtual_address, KSEG1_START, KSEG1_END)) {
        //Kernel segment 1, direct mapped, no cache
    //} else if (inRange(virtual_address, KSSEG_START, KSSEG_END)) {
        //Kernel supervisor segment. TLB mapped
    //} else if (inRange(virtual_address, KSEG3_START, KSEG3_END)) {
        //kernel segment 3. TLB mapped 
    //}
//}

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
    cpu->pipeline.RFEX_READ.MemRead = cpu->pipeline.RFEX_WRITE.MemRead;
    cpu->pipeline.RFEX_READ.MemToReg = cpu->pipeline.RFEX_WRITE.MemToReg;
    cpu->pipeline.RFEX_READ.ALUOp = cpu->pipeline.RFEX_WRITE.ALUOp;
    cpu->pipeline.RFEX_READ.MemWrite = cpu->pipeline.RFEX_WRITE.MemWrite;
    cpu->pipeline.RFEX_READ.ALUSrc = cpu->pipeline.RFEX_WRITE.ALUSrc;
    cpu->pipeline.RFEX_READ.RegWrite = cpu->pipeline.RFEX_WRITE.RegWrite;

    cpu->pipeline.RFEX_READ.opcode = cpu->pipeline.RFEX_WRITE.opcode;
    cpu->pipeline.RFEX_READ.rs = cpu->pipeline.RFEX_WRITE.rs;
    cpu->pipeline.RFEX_READ.rt = cpu->pipeline.RFEX_WRITE.rt;
    cpu->pipeline.RFEX_READ.rd = cpu->pipeline.RFEX_WRITE.rd;
    cpu->pipeline.RFEX_READ.shamt = cpu->pipeline.RFEX_WRITE.shamt;
    cpu->pipeline.RFEX_READ.function = cpu->pipeline.RFEX_WRITE.function;
    cpu->pipeline.RFEX_READ.immediate = cpu->pipeline.RFEX_WRITE.immediate;
    cpu->pipeline.RFEX_READ.rs_val = cpu->pipeline.RFEX_WRITE.rs_val;
    cpu->pipeline.RFEX_READ.rt_val = cpu->pipeline.RFEX_WRITE.rt_val;
    cpu->pipeline.RFEX_READ.SEOffset = cpu->pipeline.RFEX_WRITE.SEOffset;
    cpu->pipeline.RFEX_READ.targetaddr = cpu->pipeline.RFEX_WRITE.targetaddr;

    cpu->pipeline.EXDC_READ.MemRead = cpu->pipeline.EXDC_WRITE.MemRead;
    cpu->pipeline.EXDC_READ.MemWrite = cpu->pipeline.EXDC_WRITE.MemWrite;
    cpu->pipeline.EXDC_READ.MemToReg = cpu->pipeline.EXDC_WRITE.MemToReg;
    cpu->pipeline.EXDC_READ.RegWrite = cpu->pipeline.EXDC_WRITE.RegWrite;
    cpu->pipeline.EXDC_READ.ALUResult = cpu->pipeline.EXDC_WRITE.ALUResult;
    cpu->pipeline.EXDC_READ.SWValue = cpu->pipeline.EXDC_WRITE.SWValue;
    cpu->pipeline.EXDC_READ.WriteRegNum = cpu->pipeline.EXDC_WRITE.WriteRegNum;

    cpu->pipeline.DCWB_READ.MemToReg = cpu->pipeline.DCWB_WRITE.MemToReg;
    cpu->pipeline.DCWB_READ.RegWrite = cpu->pipeline.DCWB_WRITE.RegWrite;
    cpu->pipeline.DCWB_READ.ALUResult = cpu->pipeline.DCWB_WRITE.ALUResult;
    cpu->pipeline.DCWB_READ.LWDataValue = cpu->pipeline.DCWB_WRITE.LWDataValue;
    cpu->pipeline.DCWB_READ.WriteRegNum = cpu->pipeline.DCWB_WRITE.WriteRegNum;
}
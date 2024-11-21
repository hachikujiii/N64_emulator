#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "n64.h"
#include "my_structs.h"
#include "pipeline.h"
#include "rom_loading.h"
#include "mapping.h"
#include "debug_sys.h"


/*HAZARD LIST:
    MFHI
    MFLO
*/

bool inRange(uint64_t address, uint64_t start, uint64_t end);
uint32_t map_virtual_address(CPU *cpu, uint32_t virtual_address);
uint32_t direct_map(uint32_t virtual_address, uint32_t segment_start);
void Copy_write_to_read(CPU *cpu);
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
    //boot_sim(n64);


    n64->cpu.PC = 0xBFC00000;
    print_cpu(n64);

    return 1;
    while(true) {  

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

//apparently this is what should be loaded after PIF is done but we'll see...
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
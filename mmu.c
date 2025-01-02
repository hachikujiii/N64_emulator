#include "mmu.h"
#include "rom_loading.h"
#include "cpu.h"
#include "serial_interface.h"
#include "parallel_interface.h"
#include <stdlib.h>

void init_mmu(MMU *mmu, Memory *mem) {

    init_si(mem, &mmu->si);
    init_pi(mem, &mmu->pi);
}

uint32_t cpu_read(MMU *mmu, uint32_t address) {

    uint32_t physical_address = map_virtual(mmu, address);
    return map_physical(mmu, physical_address);
}


bool inRange (uint64_t address, uint64_t start, uint64_t end) {

    return start <= address && address <= end;
}

uint32_t map_virtual(MMU *mmu, uint32_t virtual_address) {

    //if (inRange(virtual_address, KUSEG_START, KUSEG_END)) {
        //User segment TLB mapped
    //    return tlb_lookup(cpu, virtual_address);

    //} else if (inRange(virtual_address, KSEG0_START, KSEG0_END)) {
        //Kernel segment 0. direct mapped, cached
    //    return direct_map(virtual_address, KSEG0_START);

    //} 
    if (inRange(virtual_address, KSEG1_START, KSEG1_END)) {
        //Kernel segment 1, direct mapped, no cache
        return direct_map(virtual_address, KSEG1_START);

    } 
    //else if (inRange(virtual_address, KSSEG_START, KSSEG_END)) {
        //Kernel supervisor segment. TLB mapped
    //    return tlb_lookup(cpu, virtual_address);

    //} else if (inRange(virtual_address, KSEG3_START, KSEG3_END)) {
        //kernel segment 3. TLB mapped 
    //    return tlb_lookup(cpu, virtual_address);
    //}

    //invalid address
    printf("unable to map virtual address\n");
    return -1;
}

uint32_t direct_map(uint32_t virtual_address, uint32_t segment_start) {

    return virtual_address - segment_start;
}

uint32_t map_physical(MMU *mmu, uint32_t physical_address) {

    if (inRange(physical_address, RDRAM_START, RDRAM_END)) {

        //RDRAM - built in
        printf("In range of RDRAM\n");
        return 1;
        
    } else if (inRange(physical_address, RDRAMX_START, RDRAMX_END)) {

        //RDRAM - expansion pack
        printf("In range of RDRAMX\n");
        return 1;

    } else if (inRange(physical_address, RDRAM_REG_START, RDRAM_REG_END)) {

        //rdram mmio - configures timings, maybe irrelevant
        printf("In range of RDRAM_REG\n");
        return 1;
        
    } else if (inRange(physical_address, SPDMEM_START, SPDMEM_END)) {

        //RSP data mem
        printf("In range of SPDMEM\n");
        return 1;
        
    } else if (inRange(physical_address, SPIMEM_START, SPIMEM_END)) {

        //RSP instruction mem
        printf("In range of SPIMEM\n");
        return 1;
        
    } else if (inRange(physical_address, SP_REG_START, SP_REG_END)) {

        //control RSP DMA engine, status, program counter
        printf("In range of SP_REG\n");
        return 1;
        
    } else if (inRange(physical_address, DPCOM_REG_START, DPCOM_REG_END)) {

        //send commands to RDP
        printf("In range of DPCOM_REG\n");
        return 1;
        
    } else if (inRange(physical_address, DPSPAN_REG_START, DPSPAN_REG_END)) {

        //UNKNOWN
        printf("In range of DPSPAN\n");
        return 1;
        
    } else if (inRange(physical_address, MI_REG_START, MI_REG_END)) {

        //MIPS INTERFACE, SYSTEM INFO, INTERRUPTS
        printf("In range of MI_REG\n");
        return 1;
        
    } else if (inRange(physical_address, VI_REG_START, VI_REG_END)) {

        //video interface, screen res, framebuff settings
        printf("In range of VI_REG\n");
        return 1;
        
    } else if (inRange(physical_address, AI_REG_START, AI_REG_END)) {

        //audio subsystem
        printf("In range of AI_REG\n");
        return 1;
        
    } else if (inRange(physical_address, PI_REG_START, PI_REG_END)) {

        //peripheral interface, set up DMAs cart <==> RDRAM
        printf("In range of PI_REG\n");
        return 1;
        
    } else if (inRange(physical_address, RI_REG_START, RI_REG_END)) {

        //rdram settings, irrelevant for emulation?
        printf("In range of RI_REG\n");
        return 1;
        
    } else if (inRange(physical_address, SI_REG_START, SI_REG_END)) {

        //control pif ram <==> rdram dma engine
        printf("In range of SI_REG\n");
        return 1;
        
    } else if (inRange(physical_address, CART_D2A1_START, CART_D2A1_END)) {

        //n64DD control reg, returns open bus or all 0xff when not present
        printf("In range of CART_D2A1\n");
        return 1;
        
    } else if (inRange(physical_address, CART_D1A1_START, CART_D1A1_END)) {

        //n64DD IPL ROM - returns open bus or all 0xff when not present
        printf("In range of CART_D1A1\n");
        return 1;
        
    } else if (inRange(physical_address, CART_D2A2_START, CART_D2A2_END)) {

        //SRAM is mapped here
        printf("In range of CART_D2A2\n");
        return 1;
        
    } else if (inRange(physical_address, CART_D1A2_START, CART_D1A2_END)) {

        //ROM is mapped here
        printf("In range of CART_D1A2\n");
        return 1;
        
    } else if (inRange(physical_address, PIF_BOOT_ROM_START, PIF_BOOT_ROM_END)) {

        //first code run on boot, baked into hardware
        return read_pif(&mmu->si, physical_address);

    } else if (inRange(physical_address, PIF_RAM_START, PIF_RAM_END)) {

        //used to communicate with PIF chip (controller and mem cards)
        printf("In range of PIF_RAM\n");
        return 1;
        
        
    } else if (inRange(physical_address, CART_D1A3_START, CART_D1A3_END)) {
        //
        printf("In range of CART_D1A3\n");
        return 1;
        
    } else if (inRange(physical_address, EXSYS_DEV_START, EXSYS_DEV_END)) {

        printf("In range of EXSYS_DEV\n");
        return 1;

    } else {
        printf("unable to get physical");
        return 1;
    }
   
}
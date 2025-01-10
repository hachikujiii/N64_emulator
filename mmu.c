#include "mmu.h"
#include "rom_loading.h"
#include "cpu.h"

#include <stdlib.h>

void init_mmu(MMU *mmu, RSP *rsp) {

    mmu->rsp = rsp;
}

uint32_t (*read_table[PHYSICAL_RANGE_COUNT])(MMU *, uint32_t physical_address) = {
    [RDRAM] = NULL,
    [RDRAMX] = NULL,
    [RDRAM_REGS] = NULL,
    [SPDMEM] = dmem_read_wrapper,
    [SPIMEM] = imem_read_wrapper,
    [SP_REGS] = sp_read_reg_wrapper,
    [DPCOM] = NULL,
    [DPSPAN] = NULL,
    [MI_REGS] = NULL,
    [VI_REGS] = vi_read_reg_wrapper,
    [AI_REGS] = ai_read_reg_wrapper,
    [PI_REGS] = pi_read_reg_wrapper,
    [RI_REGS] = NULL,
    [SI_REGS] = NULL,
    [CART_D2A1] = NULL,
    [CART_D1A1] = NULL,
    [CART_D2A2] = NULL,
    [CART_D1A2] = NULL,
    [PIF_ROM] = read_pif_wrapper,
    [PIF_RAM] = NULL,
    [CART_D1A3] = NULL,
    [EXSYS_DEV] = NULL,
};

void (*write_table[PHYSICAL_RANGE_COUNT])(MMU *, uint32_t physical_address, uint32_t word) = {
    [RDRAM] = NULL,
    [RDRAMX] = NULL,
    [RDRAM_REGS] = NULL,
    [SPDMEM] = dmem_write_wrapper,
    [SPIMEM] = imem_write_wrapper,
    [SP_REGS] = sp_write_reg_wrapper,
    [DPCOM] = NULL,
    [DPSPAN] = NULL,
    [MI_REGS] = NULL,
    [VI_REGS] = vi_write_reg_wrapper,
    [AI_REGS] = ai_write_reg_wrapper,
    [PI_REGS] = pi_write_reg_wrapper,
    [RI_REGS] = NULL,
    [SI_REGS] = NULL,
    [CART_D2A1] = NULL,
    [CART_D1A1] = NULL,
    [CART_D2A2] = NULL,
    [CART_D1A2] = NULL,
    [PIF_ROM] = NULL,
    [PIF_RAM] = NULL,
    [CART_D1A3] = NULL,
    [EXSYS_DEV] = NULL,
};


uint32_t cpu_bus_request(MMU *mmu, uint32_t address, uint32_t word, bool access) {

    uint32_t physical_address = map_virtual(mmu, address); 
    uint8_t range = map_physical(physical_address); 

    // If access is read (0), call the corresponding read function
    if (access == 0) {
        uint32_t (*read_func)(MMU *mmu, uint32_t physical_address) = read_table[range];
        return read_func(mmu, physical_address);  
    }
    // If access is write (1), call the corresponding write function
    else {
        void (*write_func)(MMU *mmu, uint32_t physical_address, uint32_t data) = write_table[range];
        write_func(mmu, physical_address, word);  
        return 0; 
    }
} 

uint32_t read_pif_wrapper(MMU *mmu, uint32_t physical_address) {

    return read_pif(mmu->si, physical_address);
}


uint32_t sp_read_reg_wrapper(MMU *mmu, uint32_t physical_address) {

    return rsp_read_reg(mmu->rsp, physical_address);
}

void sp_write_reg_wrapper(MMU *mmu, uint32_t physical_address, uint32_t word) {

    rsp_write_reg(mmu->rsp, physical_address, word);
}


uint32_t pi_read_reg_wrapper(MMU *mmu, uint32_t physical_address) {

    return pi_read_reg(mmu->pi, physical_address);
}

void pi_write_reg_wrapper(MMU *mmu, uint32_t physical_address, uint32_t word) {

    pi_write_reg(mmu->pi, physical_address, word);
}


uint32_t vi_read_reg_wrapper(MMU *mmu, uint32_t physical_address) {

    return vi_read_reg(mmu->vi, physical_address);
}

void vi_write_reg_wrapper(MMU *mmu, uint32_t physical_address, uint32_t word) {

    vi_write_reg(mmu->vi, physical_address, word);
}


uint32_t ai_read_reg_wrapper(MMU *mmu, uint32_t physical_address) {

    return ai_read_reg(mmu->ai, physical_address);
}

void ai_write_reg_wrapper(MMU *mmu, uint32_t physical_address, uint32_t word) {

    ai_write_reg(mmu->ai, physical_address, word);
}


uint32_t dmem_read_wrapper(MMU *mmu, uint32_t physical_address) {

    return rsp_read_dmem(mmu->rsp, physical_address);
}

void dmem_write_wrapper(MMU *mmu, uint32_t phyiscal_address, uint32_t word) {

    rsp_write_dmem(mmu->rsp, phyiscal_address, word);
}


uint32_t imem_read_wrapper(MMU *mmu, uint32_t physical_address) {

    return rsp_read_imem(mmu->rsp, physical_address);
}

void imem_write_wrapper(MMU *mmu, uint32_t phyiscal_address, uint32_t word) {

    rsp_write_imem(mmu->rsp, phyiscal_address, word);
}



bool inRange (uint32_t address, uint32_t start, uint32_t end) {

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

uint8_t map_physical(uint32_t physical_address) {

    if (inRange(physical_address, RDRAM_START, RDRAM_END)) {

        //RDRAM - built in
        printf("In range of RDRAM\n");
        return RDRAM;
        
    } else if (inRange(physical_address, RDRAMX_START, RDRAMX_END)) {

        //RDRAM - expansion pack
        printf("In range of RDRAMX\n");
        return RDRAMX;

    } else if (inRange(physical_address, RDRAM_REG_START, RDRAM_REG_END)) {

        //rdram mmio - configures timings, maybe irrelevant
        printf("In range of RDRAM_REG\n");
        return RDRAM_REGS;
        
    } else if (inRange(physical_address, SPDMEM_START, SPDMEM_END)) {

        //RSP data mem
        printf("In range of SPDMEM\n");
        return SPDMEM;
        
    } else if (inRange(physical_address, SPIMEM_START, SPIMEM_END)) {

        //RSP instruction mem
        printf("In range of SPIMEM\n");
        return SPIMEM;
        
    } else if (inRange(physical_address, SP_REG_START, SP_REG_END)) {

        return SP_REGS;
        
    } else if (inRange(physical_address, DPCOM_REG_START, DPCOM_REG_END)) {

        //send commands to RDP
        printf("In range of DPCOM_REG\n");
        return DPCOM;
        
    } else if (inRange(physical_address, DPSPAN_REG_START, DPSPAN_REG_END)) {

        //UNKNOWN
        printf("In range of DPSPAN\n");
        return DPSPAN;
        
    } else if (inRange(physical_address, MI_REG_START, MI_REG_END)) {

        //MIPS INTERFACE, SYSTEM INFO, INTERRUPTS
        printf("In range of MI_REG\n");
        return MI_REGS;
        
    } else if (inRange(physical_address, VI_REG_START, VI_REG_END)) {

        //video interface, screen res, framebuff settings
        printf("In range of VI_REG\n");
        return VI_REGS;
        
    } else if (inRange(physical_address, AI_REG_START, AI_REG_END)) {

        //audio subsystem
        printf("In range of AI_REG\n");
        return AI_REGS;
        
    } else if (inRange(physical_address, PI_REG_START, PI_REG_END)) {

        //peripheral interface, set up DMAs cart <==> RDRAM
        printf("In range of PI_REG\n");
        return PI_REGS;
        
    } else if (inRange(physical_address, RI_REG_START, RI_REG_END)) {

        //rdram settings, irrelevant for emulation?
        printf("In range of RI_REG\n");
        return RI_REGS;
        
    } else if (inRange(physical_address, SI_REG_START, SI_REG_END)) {

        //control pif ram <==> rdram dma engine
        printf("In range of SI_REG\n");
        return SI_REGS;
        
    } else if (inRange(physical_address, CART_D2A1_START, CART_D2A1_END)) {

        //n64DD control reg, returns open bus or all 0xff when not present
        printf("In range of CART_D2A1\n");
        return CART_D2A1;
        
    } else if (inRange(physical_address, CART_D1A1_START, CART_D1A1_END)) {

        //n64DD IPL ROM - returns open bus or all 0xff when not present
        printf("In range of CART_D1A1\n");
        return CART_D1A1;
        
    } else if (inRange(physical_address, CART_D2A2_START, CART_D2A2_END)) {

        //SRAM is mapped here
        printf("In range of CART_D2A2\n");
        return CART_D2A2;
        
    } else if (inRange(physical_address, CART_D1A2_START, CART_D1A2_END)) {

        //ROM is mapped here
        printf("In range of CART_D1A2\n");
        return CART_D1A2;
        
    } else if (inRange(physical_address, PIF_BOOT_ROM_START, PIF_BOOT_ROM_END)) { //first code run on boot, baked into hardware

        return PIF_ROM;
                     
    } else if (inRange(physical_address, PIF_RAM_START, PIF_RAM_END)) {  //used to communicate with PIF chip (controller and mem cards)

        return PIF_RAM;
        
    } else if (inRange(physical_address, CART_D1A3_START, CART_D1A3_END)) {
        
        return CART_D1A3;
        
    } else if (inRange(physical_address, EXSYS_DEV_START, EXSYS_DEV_END)) {

        return EXSYS_DEV;

    } else {
        printf("unable to get physical");
        return 99;
    }
   
}
#include "parallel_interface.h"

void init_pi(Parallel_Interface *pi, Memory *mem){

    pi->cart_rom = mem->cart_rom;
}

uint32_t pi_read_reg(Parallel_Interface *pi, uint32_t addr) {
    switch(addr) {
        case 0x04600000:
            return pi->pi_dram_addr;

        case 0x04600004:
            return pi->pi_cart_addr;

        case 0x04600008:
            return pi->pi_rd_len;

        case 0x0460000C:
            return pi->pi_wr_len;
            
        case 0x04600010:  
            return pi->pi_status;
        
        case 0x04600014:  
            return pi->pi_bsd_domn_lat;
            
        case 0x04600018:  // SP_DMA_BUSY_REG
            return pi->pi_bsd_domn_pwd;
            
        case 0x0460001C:  // SP_SEMAPHORE_REG
            return pi->pi_bsd_domn_pgs;
        case 0x04600020:
            return pi->pi_bsd_domn_rls;
    }
    return 0;  // or handle invalid address
}

void pi_write_reg(Parallel_Interface *pi, uint32_t addr, uint32_t word) {

    switch(addr) {

        case 0x04600000:
            pi->pi_dram_addr = word;
            break;

        case 0x04600004:
            pi->pi_cart_addr = word;
            break;

        case 0x04600008:
            pi->pi_rd_len = word;
            break;

        case 0x0460000C:
            pi->pi_wr_len = word;
            break;

        case 0x04600010:  
            pi->pi_status = word;
            break;

        case 0x04600014:  
            pi->pi_bsd_domn_lat = word;
            break;

        case 0x04600018:  // SP_DMA_BUSY_REG
            pi->pi_bsd_domn_pwd = word;
            break;

        case 0x0460001C:  // SP_SEMAPHORE_REG
            pi->pi_bsd_domn_pgs = word;
            break;

        case 0x04600020:
            pi->pi_bsd_domn_rls = word;
            break;
        
        default:
            printf("out of range PI. exiting.\n");
            exit(1);
    }    
}
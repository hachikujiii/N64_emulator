#include "rsp.h"

uint32_t rsp_read_reg(RSP *rsp, uint32_t addr) {
    switch(addr) {
        case 0x04040000:
            return rsp->sp_dma_spaddr;

        case 0x04040004:
            return rsp->sp_dma_ramaddr;

        case 0x04040008:
            return rsp->sp_dma_rdlen;

        case 0x0404000C:
            return rsp->sp_dma_wrlen;
            
        case 0x04040010:  // SP_STATUS_REG
            return rsp->sp_status;
        
        case 0x04040014:  // SP_DMA_FULL_REG
            return rsp->sp_dma_full;
            
        case 0x04040018:  // SP_DMA_BUSY_REG
            return rsp->sp_dma_busy;
            
        case 0x0404001C:  // SP_SEMAPHORE_REG
            return rsp->sp_semaphore;
    }
    return 0;  // or handle invalid address
}

void rsp_write_reg(RSP *rsp, uint32_t addr, uint32_t word) {

    switch(addr) {
        case 0x04040000:
            rsp->sp_dma_spaddr = word;
            break;
        case 0x04040004:
            rsp->sp_dma_ramaddr = word;
            break;
        case 0x04040008:
            rsp->sp_dma_rdlen = word;
            break;
        case 0x0404000C:
            rsp->sp_dma_wrlen = word;
            break;
        case 0x04040010:  // SP_STATUS_REG
            rsp->sp_status = word;
            break;
        case 0x04040014:  // SP_DMA_FULL_REG
            rsp->sp_dma_full = word;
            break;
        case 0x04040018:  // SP_DMA_BUSY_REG
            rsp->sp_dma_busy = word;
            break;
        case 0x0404001C:  // SP_SEMAPHORE_REG
            rsp->sp_semaphore = word;
            break;
        default:
            printf("unhandled range %X\n", addr);
            exit(1);
    }
}

uint32_t rsp_read_mem(RSP *rsp, uint32_t addr) {
    if (addr >= 0x04000000 && addr <= 0x04000FFF) {
        return *(uint32_t*)&rsp->dmem[addr - 0x04000000];
    }
    else if (addr >= 0x04001000 && addr <= 0x04001FFF) {
        return *(uint32_t*)&rsp->imem[addr - 0x04001000];
    } 

    return 0;
}
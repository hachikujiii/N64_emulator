#include "rsp.h"

void init_rsp(RSP *rsp) {
    
    rsp->sp_status = 1;
}

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

uint32_t rsp_read_dmem(RSP *rsp, uint32_t addr) {
    uint32_t offset;
    uint32_t value;

    offset = addr - 0x04000000;
    value = (rsp->dmem[offset] << 24) |
            (rsp->dmem[offset + 1] << 16) |
            (rsp->dmem[offset + 2] << 8) |
            (rsp->dmem[offset + 3]);
    return value;

}

void rsp_write_dmem(RSP *rsp, uint32_t addr, uint32_t word) {
    uint32_t offset;
    uint32_t value;

    offset = addr - 0x04000000;
    rsp->imem[offset] = word;
}

uint32_t rsp_read_imem(RSP *rsp, uint32_t addr) {
    uint32_t offset;
    uint32_t value;

    offset = addr - 0x04001000;
    value = (rsp->imem[offset] << 24) |
            (rsp->imem[offset + 1] << 16) |
            (rsp->imem[offset + 2] << 8) |
            (rsp->imem[offset + 3]);
    return value;

}

void rsp_write_imem(RSP *rsp, uint32_t addr, uint32_t word) {
    uint32_t offset;
    uint32_t value;

    offset = addr - 0x04001000;
    rsp->imem[offset] = word;
}

   

#ifndef RSP_H
#define RSP_H

#include <stdint.h>

#define IMEM_SIZE 0x1000
#define DMEM_SIZE 0x1000

typedef struct {
    uint32_t rsp_gpr[32];
    __uint128_t vector_regs[32];

    uint8_t imem[IMEM_SIZE];
    uint8_t dmem[DMEM_SIZE];

    uint32_t sp_dma_spaddr;     //c0...c7 (sp_semaphore) these are linked to rsp.cop0
    uint32_t sp_dma_ramaddr;
    uint32_t sp_dma_rdlen;
    uint32_t sp_dma_wrlen;
    uint32_t sp_status;
    uint32_t sp_dma_full;
    uint32_t sp_dma_busy;
    uint32_t sp_semaphore;
    /*also has vector unit or scalar unit? mem accesses can only be DMEM*/
} RSP;

void dma_read(RSP *rsp);
void dma_write(RSP *rsp);

uint32_t rsp_read_reg(RSP *rsp, uint32_t addr);
void rsp__write_reg(RSP *rsp, uint32_t addr, uint32_t value);
uint32_t rsp_read_mem(RSP *rsp, uint32_t addr);
void rsp_write_mem(RSP *rsp, uint32_t addr, uint32_t value);

#endif
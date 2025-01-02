#ifndef RSP_H
#define RSP_H

#include <stdint.h>

typedef struct {
    uint32_t rsp_regs[32];
    /*also has vector unit or scalar unit? mem accesses can only be DMEM*/
} RSP;

void dma_read(RSP *rsp);
void dma_write(RSP *rsp);

int read_sp_mem(void *opaque, uint32_t addr, uint32_t *word);
int read_sp_regs(void *opaque, uint32_t addr, uint32_t *word);
int read_sp_regs2(void *opaque, uint32_t addr, uint32_t *word);
int write_sp_mem(void *opaque, uint32_t addr, uint32_t *word, uint32_t dqm);
int write_sp_regs(void *opaque, uint32_t addr, uint32_t *word, uint32_t dqm);
int write_sp_regs2(void *opaque, uint32_t addr, uint32_t *word, uint32_t dqm);

#endif
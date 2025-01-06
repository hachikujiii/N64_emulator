#ifndef PARALLEL_INTERFACE_H
#define PARALLEL_INTERFACE_H

#include <stdint.h>
#include "memory.h"

typedef struct {

    uint8_t *cart_rom;

    uint32_t pi_dram_addr;
    uint32_t pi_cart_addr;
    uint32_t pi_rd_len;
    uint32_t pi_wr_len;
    uint32_t pi_status;
    uint32_t pi_bsd_domn_lat;
    uint32_t pi_bsd_domn_pwd;
    uint32_t pi_bsd_domn_pgs;
    uint32_t pi_bsd_domn_rls;

} Parallel_Interface;

void init_pi(Parallel_Interface *pi, Memory *mem);
uint32_t pi_read_reg(Parallel_Interface *pi, uint32_t addr);
uint32_t pi_write_reg(Parallel_Interface *pi, uint32_t addr, uint32_t word);

#endif
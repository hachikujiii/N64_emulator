#ifndef MY_STRUCTS_H
#define MY_STRUCTS_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {

} RDP;

typedef struct {
    uint32_t rsp_regs[32];
    /*also has vector unit or scalar unit? mem accesses can only be DMEM*/
} RSP;

typedef struct {
    RSP rsp;
    RDP rdp;
} RCP;

typedef struct {
    double fpr[32];
    uint32_t csr;
} CP1;

#endif
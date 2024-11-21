#ifndef MY_STRUCTS_H
#define MY_STRUCTS_H

#include <stdint.h>
#include <stdbool.h>

#define RDRAM_SIZE         0x800000 //8MB RAM
#define ROM_SIZE           0x2000000 //32MB ROM
#define PIF_ROM_SIZE       0x7C0
#define ICACHE_SIZE        0x4000 //16KB Instruction Cache
#define DCACHE_SIZE        0x2000 //8KB Data Cache
#define init_address       0xBFC00000 //starting address on boot process

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
    unsigned char *data;
} PIF;

typedef struct {
    unsigned char *ROM;
} Cartridge;

#endif
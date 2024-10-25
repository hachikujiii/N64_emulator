#ifndef MY_STRUCTS_H
#define MY_STRUCTS_H

#include <stdint.h>
#include "n64_pipeline.h"
#define RDRAM_SIZE         0x800000 //8MB RAM
#define ROM_SIZE           0x2000000 //32MB ROM
#define PIF_ROM_SIZE       0x7C0
#define ICACHE_SIZE        0x4000 //16KB Instruction Cache
#define DCACHE_SIZE        0x2000 //8KB Data Cache
#define init_address       0xBFC00000 //starting address on boot process
#define TLB_SIZE           32

typedef struct {
    uint32_t virtual_address1; // Virtual address for the first page
    uint32_t physical_address1; // Physical address for the first page
    uint32_t virtual_address2; // Virtual address for the second page
    uint32_t physical_address2; // Physical address for the second page
    uint32_t page_size; // Page size (4 KB or 16 MB)
    uint8_t valid; // Valid bit to indicate if the entry is valid
    // Additional flags and metadata can be added here
} TLBEntry;

typedef struct {
    TLBEntry entries[TLB_SIZE];
} TLB;

typedef struct {
    double fpr[32];
    uint32_t csr;
} CP1;

typedef struct {

} RDP;

typedef struct {
    uint32_t rsp_regs[32];
    /*also has vector unit or scalar unit? mem accesses can only be DMEM*/
} RSP;

typedef struct {

} MMU;

typedef struct {
    RSP rsp;
    RDP rdp;
} RCP;

typedef struct {
    /*32 registers*/
    uint32_t Index;
    uint32_t Random;
    uint32_t EntryLo0;
    uint32_t EntryLo1;
    uint32_t Context;
    uint32_t PageMask;
    uint32_t Wired;
    uint32_t BadVAddr;
    uint32_t Count;
    uint32_t EntryHi;
    uint32_t Compare;
    uint32_t Status;
    uint32_t Cause;
    uint32_t EPC;
    uint32_t PRId;
    uint32_t Config;
    uint32_t LLAddr;
    uint32_t WatchLo;
    uint32_t WatchHi;
    uint32_t XContext;
    uint32_t TagLo;
    uint32_t TagHi;
    uint32_t ErrorEPC;
    /* 7, 21-25, 31 reserved for future use*/

    TLB tlb;
} CP0;

typedef struct {
    Pipeline pipeline;
    MMU mmu;
    CP0 cp0; /*System Control Processor*/
    CP1 cp1; /*Floating Point Unit*/
    uint64_t gpr[32];
    double fpr[32]; 
    uint64_t PC; /*= init_address?*/
    uint64_t HI, LO;
    bool LLbit;
    float FCR0;
    float FCR31;

    uint64_t instruction_cache[ICACHE_SIZE / 8];
    uint64_t data_cache[DCACHE_SIZE / 8];

} CPU;

typedef struct {
    unsigned char *data;
} PIF;

typedef struct {
    PIF pif_rom;
    CPU cpu;
    RCP rcp;
} N64;

typedef struct {
    unsigned char *ROM;
} Cartridge;


#endif
#ifndef TLB_H
#define TLB_H

#include <stdint.h>

#define TLB_SIZE 32

typedef struct {
    uint32_t VPN2;         // Virtual Page Number (high bits of the virtual address)
    uint32_t ASID;         // Address Space Identifier (process isolation)
    uint32_t PFN0;         // Physical Frame Number for even page
    uint32_t PFN1;         // Physical Frame Number for odd page
    uint8_t Valid0;            // Valid bit for even page
    uint8_t Valid1;            // Valid bit for odd page
    uint8_t Dirty0;            // Dirty bit for even page
    uint8_t Dirty1;            // Dirty bit for odd page
    uint8_t Global;             // Global bit (shared across all ASIDs)
    uint8_t page_size;     // Page size (0 = 4 KB, 1 = 16 MB)
} TLBEntry;

typedef struct {
    TLBEntry entries[TLB_SIZE];
} TLB;

#endif
#ifndef TLB_H
#define TLB_H

#include <stdint.h>

#define TLB_SIZE 32

typedef struct {
    uint32_t virtual_address1;  // Virtual address for the first page
    uint32_t physical_address1; // Physical address for the first page
    uint32_t virtual_address2;  // Virtual address for the second page
    uint32_t physical_address2; // Physical address for the second page
    uint32_t page_size;         // Page size (4 KB or 16 MB)
    uint8_t valid;              // Valid bit to indicate if the entry is valid
                                // Additional flags and metadata can be added here
} TLBEntry;

typedef struct {
    TLBEntry entries[TLB_SIZE];
} TLB;

#endif
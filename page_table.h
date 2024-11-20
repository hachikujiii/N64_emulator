#ifndef PAGETABLE_H
#define PAGETABLE_H

#include <stdlib.h>
#include <stdint.h>
#include "my_structs.h"

#define PAGE_SIZE 4096  // 4KB
#define TOTAL_PAGES 8192  // Enough for a 32MB ROM with 4KB pages

// Page table to map virtual pages to ROM pages
uint8_t *page_table[TOTAL_PAGES];

void load_rom_into_pages(uint8_t *rom, size_t rom_size) {
    // Calculate the total number of pages in the ROM
    uint32_t num_pages = rom_size / PAGE_SIZE;
    
    // Ensure we don’t exceed TOTAL_PAGES
    for (uint32_t page = 0; page < num_pages && page < TOTAL_PAGES; page++) {
        page_table[page] = &rom[page * PAGE_SIZE];
    }
}

uint32_t translate_virtual_address(uint32_t virtual_address) {
    // Calculate page number and offset within the page
    uint32_t page_number = virtual_address / PAGE_SIZE;
    uint32_t offset = virtual_address % PAGE_SIZE;

    // Check if the page number is within bounds
    if (page_number >= TOTAL_PAGES || page_table[page_number] == NULL) {
        // Return an error value or handle it appropriately
        return 1;  // Error: page not mapped or out of range
    }

    // Return the translated physical address by adding the offset to the base address of the page
    return (uint32_t)(page_table[page_number] + offset);
}

#endif
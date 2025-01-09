#ifndef AUDIO_INTERFACE_H
#define AUDIO_INTERFACE_H


#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {

    uint32_t ai_dram_addr;
    uint32_t ai_length;
    uint32_t ai_control;
    uint32_t ai_status;
    uint32_t ai_dacrate;
    uint32_t ai_bitrate;
    
} Audio_Interface;

void init_ai(Audio_Interface *ai);
uint32_t ai_read_reg(Audio_Interface *ai, uint32_t addr);
void ai_write_reg(Audio_Interface *ai, uint32_t addr, uint32_t word);

#endif
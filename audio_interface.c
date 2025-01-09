#include "audio_interface.h"

void init_ai(Audio_Interface *ai) {

    ai->ai_dram_addr = 0;
    ai->ai_length = 0;
    ai->ai_control = 0;
    ai->ai_status = 0;
    ai->ai_dacrate = 0;
    ai->ai_bitrate = 0;
}

uint32_t ai_read_reg(Audio_Interface *ai, uint32_t addr) {

    switch(addr) {

        case 0x04500000:
            return ai->ai_dram_addr;
        case 0x04500004:
            return ai->ai_length;
        case 0x04500008:
            return ai->ai_control;
        case 0x0450000C:
            return ai->ai_status;
        case 0x04500010:
            return ai->ai_dacrate;
        case 0x04500014:
            return ai->ai_bitrate;
        default:
            return 0;
    }
}

void ai_write_reg(Audio_Interface *ai, uint32_t addr, uint32_t word) {

    switch(addr) {

        case 0x04500000:
            ai->ai_dram_addr = word;
            break;

        case 0x04500004:
            ai->ai_length = word;
            break;

        case 0x04500008:
            ai->ai_control = word;
            break;

        case 0x0450000C:
            ai->ai_status = word;
            break;

        case 0x04500010:
            ai->ai_dacrate = word;
            break;

        case 0x04500014:
            ai->ai_bitrate = word;
            break;

        default:
            printf("out of range ai. exiting\n");
            exit(1);
    }
}
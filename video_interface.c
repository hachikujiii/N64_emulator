#include "video_interface.h"

uint32_t vi_read_reg(Video_Interface *vi, uint32_t addr) {
    switch(addr) {
        case 0x04400000:
            return vi->vi_ctrl;

        case 0x04400004:
            return vi->vi_origin;

        case 0x04400008:
            return vi->vi_width;

        case 0x0440000C:
            return vi->vi_v_intr;
            
        case 0x04400010:  
            return vi->vi_v_current;
        
        case 0x04400014:  
            return vi->vi_burst;
            
        case 0x04400018:  // SP_DMA_BUSY_REG
            return vi->vi_v_total;
            
        case 0x0440001C:  // SP_SEMAPHORE_REG
            return vi->vi_h_total;

        case 0x04400020:
            return vi->vi_h_total_leap;

        case 0x04400024:
            return vi->vi_h_video;

        case 0x04400028:
            return vi->vi_v_video;

        case 0x0440002C:
            return vi->vi_v_burst;

        case 0x04400030:
            return vi->vi_x_scale;

        case 0x04400034:
            return vi->vi_y_scale;

        case 0x04400038:
            return vi->vi_test_addr;

        case 0x0440003C:
            return vi->vi_staged_data;

        default:
            return 0;
    }

}


void vi_write_reg(Video_Interface *vi, uint32_t addr, uint32_t word) {

    switch(addr) {
        case 0x04400000:
            vi->vi_ctrl = word;
            break;

        case 0x04400004:
            vi->vi_origin = word;
            break;

        case 0x04400008:
            vi->vi_width = word;
            break;

        case 0x0440000C:
            vi->vi_v_intr = word;
            break;

        case 0x04400010:  
            vi->vi_v_current = word;
            break;

        case 0x04400014:  
            vi->vi_burst = word;
            break;

        case 0x04400018:  // SP_DMA_BUSY_REG
            vi->vi_v_total = word;
            break;

        case 0x0440001C:  // SP_SEMAPHORE_REG
            vi->vi_h_total = word;
            break;

        case 0x04400020:
            vi->vi_h_total_leap = word;
            break;

        case 0x04400024:
            vi->vi_h_video = word;
            break;

        case 0x04400028:
            vi->vi_v_video = word;
            break;

        case 0x0440002C:
            vi->vi_v_burst = word;
            break;

        case 0x04400030:
            vi->vi_x_scale = word;
            break;

        case 0x04400034:
            vi->vi_y_scale = word;
            break;

        case 0x04400038:
            vi->vi_test_addr = word;
            break;

        case 0x0440003C:
            vi->vi_staged_data = word;
            break;

        default:
            printf("out of range VI. exiting\n");
            exit(1);
            
    }
}
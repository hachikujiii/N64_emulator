#ifndef VIDEO_INTERFACE_H
#define VIDEO_INTERFACE_H

#include <stdint.h>

typedef struct {

    uint32_t vi_ctrl;
    uint32_t vi_origin;
    uint32_t vi_width;
    uint32_t vi_v_intr;
    uint32_t vi_v_current;
    uint32_t vi_burst;
    uint32_t vi_v_total;
    uint32_t vi_h_total;
    uint32_t vi_h_total_leap;
    uint32_t vi_h_video;
    uint32_t vi_v_video;
    uint32_t vi_v_burst;
    uint32_t vi_x_scale;
    uint32_t vi_y_scale;
    uint32_t vi_test_addr;
    uint32_t vi_staged_data;
    
} Video_Interface ;

#endif
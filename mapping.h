#ifndef MAPPING_H
#define MAPPING_H

//Virtual Address map
#define KUSEG_START        0x00000000
#define KUSEG_END          0x7FFFFFFF
#define KSEG0_START        0x80000000
#define KSEG0_END          0x9FFFFFFF
#define KSEG1_START        0xA0000000
#define KSEG1_END          0xBFFFFFFF
#define KSSEG_START        0xC0000000
#define KSSEG_END          0xDFFFFFFF
#define KSEG3_START        0xE0000000
#define KSEG3_END          0xFFFFFFFF

//Phyiscal Address map
#define RDRAM_START        0x00000000
#define RDRAM_END          0x003EFFFF
#define RDRAMX_START       0x00400000
#define RDRAMX_END         0x007FFFFF
#define RDRAM_REG_START    0x03F00000
#define RDRAM_REG_END      0x03FFFFFF
#define SPDMEM_START       0x04000000
#define SPDMEM_END         0x04000FFF
#define SPIMEM_START       0x04001000
#define SPIMEM_END         0x04001FFF
#define SP_REG_START       0x04040000
#define SP_REG_END         0x040FFFFF
#define DPCOM_REG_START    0x04100000
#define DPCOM_REG_END      0x041FFFFF
#define DPSPAN_REG_START   0x04200000
#define DPSPAN_REG_END     0x042FFFFF
#define MI_REG_START       0x04300000
#define MI_REG_END         0x043FFFFF
#define VI_REG_START       0x04400000
#define VI_REG_END         0x044FFFFF
#define AI_REG_START       0x04500000
#define AI_REG_END         0x045FFFFF
#define PI_REG_START       0x04600000
#define PI_REG_END         0x046FFFFF
#define RI_REG_START       0x04700000
#define RI_REG_END         0x047FFFFF
#define SI_REG_START       0x04800000
#define SI_REG_END         0x048FFFFF
#define CART_D2A1_START    0x05000000
#define CART_D2A1_END      0x05FFFFFF
#define CART_D1A1_START    0x06000000
#define CART_D1A1_END      0x07FFFFFF
#define CART_D2A2_START    0x08000000
#define CART_D2A2_END      0x0FFFFFFF
#define CART_D1A2_START    0x10000000
#define CART_D1A2_END      0x1FBFFFFF
#define PIF_BOOT_ROM_START 0x1FC00000
#define PIF_BOOT_ROM_END   0x1FC007BF
#define PIF_RAM_START      0x1FC007C0
#define PIF_RAM_END        0x1FC007FF
#define CART_D1A3_START    0x1FD00000
#define CART_D1A3_END      0x7FFFFFFF
#define EXSYS_DEV_START    0x80000000
#define EXSYS_DEV_END      0xFFFFFFFF

#endif
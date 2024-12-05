#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#define ROM_SIZE 0x2000000 //32MB ROM

typedef struct {
    unsigned char *ROM;
} Cartridge;

#endif
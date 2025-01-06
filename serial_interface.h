#ifndef SERIAL_INTERFACE_H
#define SERIAL_INTERFACE_H

#include <stdint.h>
#include "memory.h"

typedef struct {

    uint8_t *pif;

} Serial_Interface;

void init_si(Serial_Interface *si, Memory *mem);
uint32_t read_pif(Serial_Interface *si, uint32_t address);

#endif
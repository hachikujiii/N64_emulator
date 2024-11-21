#ifndef CP1_H
#define CP1_H

#include <stdint.h>

typedef struct {
    double fpr[32];
    uint32_t csr;
} CP1;

#endif
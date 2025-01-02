#include "pipeline.h"
#include <string.h>

void init_pipeline(Pipeline *pipeline) {
    
    memset(&pipeline->ICRF_WRITE, 0, sizeof(ICRF_Pipeline));
    memset(&pipeline->ICRF_READ, 0, sizeof(ICRF_Pipeline));

    memset(&pipeline->RFEX_WRITE, 0, sizeof(RFEX_Pipeline));
    memset(&pipeline->RFEX_READ, 0, sizeof(RFEX_Pipeline));

    memset(&pipeline->EXDC_WRITE, 0, sizeof(EXDC_Pipeline));
    memset(&pipeline->EXDC_READ, 0, sizeof(EXDC_Pipeline));

    memset(&pipeline->DCWB_WRITE, 0, sizeof(DCWB_Pipeline));
    memset(&pipeline->DCWB_READ, 0, sizeof(DCWB_Pipeline));

    pipeline->RFEX_WRITE.control.format_type = NOP;
    pipeline->RFEX_WRITE.control.op_type = INVALID;
    pipeline->RFEX_WRITE.control.mem_access = NO_ACCESS;
    pipeline->RFEX_READ.control.format_type = NOP;
    pipeline->RFEX_READ.control.op_type = INVALID;
    pipeline->RFEX_READ.control.mem_access = NO_ACCESS;

    pipeline->EXDC_WRITE.control.format_type = NOP;
    pipeline->EXDC_WRITE.control.op_type = INVALID;
    pipeline->EXDC_WRITE.control.mem_access = NO_ACCESS;
    pipeline->EXDC_READ.control.format_type = NOP;
    pipeline->EXDC_READ.control.op_type = INVALID;
    pipeline->EXDC_READ.control.mem_access = NO_ACCESS;

    pipeline->DCWB_WRITE.control.format_type = NOP;
    pipeline->DCWB_WRITE.control.op_type = INVALID;
    pipeline->DCWB_WRITE.control.mem_access = NO_ACCESS;
    pipeline->DCWB_READ.control.format_type = NOP;
    pipeline->DCWB_READ.control.op_type = INVALID;
    pipeline->DCWB_READ.control.mem_access = NO_ACCESS;
    return;
}  


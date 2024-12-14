#include "pipeline.h"

void init_pipeline(Pipeline *pipeline) {
    
    pipeline->ICRF_WRITE.instruction = 0;
    pipeline->ICRF_READ.instruction = 0;

    pipeline->RFEX_WRITE.control.f_type = NOP;
    pipeline->RFEX_WRITE.control.type = INVALID;
    pipeline->RFEX_WRITE.control.memAccess = NO_ACCESS;
    pipeline->RFEX_READ.control.f_type = NOP;
    pipeline->RFEX_READ.control.type = INVALID;
    pipeline->RFEX_READ.control.memAccess = NO_ACCESS;

    pipeline->EXDC_WRITE.control.f_type = NOP;
    pipeline->EXDC_WRITE.control.type = INVALID;
    pipeline->EXDC_WRITE.control.memAccess = NO_ACCESS;
    pipeline->EXDC_READ.control.f_type = NOP;
    pipeline->EXDC_READ.control.type = INVALID;
    pipeline->EXDC_READ.control.memAccess = NO_ACCESS;

    pipeline->DCWB_WRITE.control.f_type = NOP;
    pipeline->DCWB_WRITE.control.type = INVALID;
    pipeline->DCWB_WRITE.control.memAccess = NO_ACCESS;
    pipeline->DCWB_READ.control.f_type = NOP;
    pipeline->DCWB_READ.control.type = INVALID;
    pipeline->DCWB_READ.control.memAccess = NO_ACCESS;
    return;
}  
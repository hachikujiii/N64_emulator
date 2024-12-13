#include "pipeline.h"

void init_pipeline(Pipeline *pipeline) {
    
    pipeline->ICRF_WRITE.instruction = 0;
    pipeline->ICRF_READ.instruction = 0;

    pipeline->RFEX_WRITE.control.f_type = NOP;
    pipeline->RFEX_READ.control.f_type = NOP;

    pipeline->EXDC_WRITE.control.f_type = NOP;
    pipeline->EXDC_READ.control.f_type = NOP;

    pipeline->DCWB_WRITE.control.f_type = NOP;
    pipeline->DCWB_READ.control.f_type = NOP;
    return;
}  
#ifndef DEBUG_SYS_H 
#define DEBUG_SYS_H 

#include "mystructs.h" 
#include <stdio.h> 
#include "cpu.h"

extern const char* register_names[]; 
void print_regs(CPU *cpu); 
void print_cpu(CPU *cpu); 
void print_pif_ram(Memory *mem);
void print_instruction(Pipeline *pipeline);
const char* format_type_to_string(Format_Type f_type); 
const char* instruction_type_to_string(Operation_Type type); 
const char* access_type_to_string(Access_Type memAccess); 
void print_pipeline(CPU *cpu);
void print_rsp(RSP *rsp);

#endif
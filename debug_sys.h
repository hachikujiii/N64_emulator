#ifndef DEBUG_SYS_H 
#define DEBUG_SYS_H 

#include "mystructs.h" 
#include <stdio.h> 
#include "cpu.h"

extern const char* register_names[]; 
void print_regs(CPU *cpu); 
void print_cpu(CPU *cpu); 
const char* format_type_to_string(Format_Type f_type); 
const char* instruction_type_to_string(Instruction_Type type); 
const char* access_type_to_string(Access_Type memAccess); 
void print_pipeline(CPU *cpu);

#endif
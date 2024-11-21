#ifndef DEBUG_SYS_H 
#define DEBUG_SYS_H 

#include "my_structs.h" 
#include "pipeline.h" 
#include "instruction_control.h" 
#include <stdio.h> 

extern const char* register_names[]; 
void print_regs(N64 *n64); 
void print_cpu(N64 *n64); 
const char* format_type_to_string(Format_Type f_type); 
const char* instruction_type_to_string(Instruction_Type type); 
const char* access_type_to_string(Access_Type memAccess); 
void print_pipeline(N64 *n64);

#endif
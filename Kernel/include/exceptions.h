#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include "include/registers.h"
#include <lib.h>
void exception_dispatcher(int exception, register_struct_t * registers);

 void zero_division();
 void invalid_opcode();
#endif
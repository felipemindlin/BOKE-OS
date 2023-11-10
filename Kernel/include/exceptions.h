#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include "include/registers.h"
#include <lib.h>
void exceptionDispatcher(int exception, registerStructT * registers);

 void zero_division();
 void invalid_opcode();
#endif
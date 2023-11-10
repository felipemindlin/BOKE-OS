#ifndef _REGISTERS_H
#define _REGISTERS_H

#include <stdint.h>
#include "interrupts.h"

//Estructura de los registros que se van a guardar en el snapshot y en las interrupciones
typedef struct registerStrucT{
	uint64_t rax, rbx, rcx, rdx;
	uint64_t rsi, rdi, rbp, rsp;
	uint64_t  r8,  r9, r10, r11;
	uint64_t r12, r13, r14, r15;
	uint64_t ss, cs, rflags, rip;
} registerStructT;

extern registerStructT registers;

extern int flag_snapshot_taken;

void printRegisters(registerStructT * dumpedRegisters);
#endif /* _REGISTERS_H */
#include <registers.h>
#include "../drivers/include/videoDriver.h"
int flag_snapshot_taken = 0;
void printRegisters(registerStructT * registers) {
	if (flag_snapshot_taken == 0){
		drawWordColor(RED, "Error - No hay snapshot de registros\nPresione(';') para crear uno\n");
		return;
	}
	drawWord("RIP = 0x");
	drawRegisters(registers->rip);
	drawWord("RAX = 0x");
	drawRegisters(registers->rax);
	drawWord("RBX = 0x");
	drawRegisters(registers->rbx);
	drawWord("RCX = 0x");
	drawRegisters(registers->rcx);
	drawWord("RDX = 0x");
	drawRegisters(registers->rdx);
	drawWord("RSP = 0x");
	drawRegisters(registers->rsp);
	drawWord("RBP = 0x");
	drawRegisters(registers->rbp);
	drawWord("RSI = 0x");
	drawRegisters(registers->rsi);
	drawWord("RDI = 0x");
	drawRegisters(registers->rdi);
	drawWord("R8  = 0x");
	drawRegisters(registers->r8);
	drawWord("R9  = 0x");
	drawRegisters(registers->r9);
	drawWord("R10 = 0x");
	drawRegisters(registers->r10);
	drawWord("R11 = 0x");
	drawRegisters(registers->r11);
	drawWord("R12 = 0x");
	drawRegisters(registers->r12);
	drawWord("R13 = 0x");
	drawRegisters(registers->r13);
	drawWord("R14 = 0x");
	drawRegisters(registers->r14);
	drawWord("R15 = 0x");
	drawRegisters(registers->r15);
}
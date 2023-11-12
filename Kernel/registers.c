#include <registers.h>
#include "../drivers/include/videoDriver.h"
int flag_snapshot_taken = 0;
void printRegisters(register_struct_t * registers) {
	if (flag_snapshot_taken == 0){
		draw_word_color(RED, "Error - No hay snapshot de registros\nPresione(';') para crear uno\n");
		return;
	}
	draw_word("RIP = 0x");
	drawRegisters(registers->rip);
	draw_word("RAX = 0x");
	drawRegisters(registers->rax);
	draw_word("RBX = 0x");
	drawRegisters(registers->rbx);
	draw_word("RCX = 0x");
	drawRegisters(registers->rcx);
	draw_word("RDX = 0x");
	drawRegisters(registers->rdx);
	draw_word("RSP = 0x");
	drawRegisters(registers->rsp);
	draw_word("RBP = 0x");
	drawRegisters(registers->rbp);
	draw_word("RSI = 0x");
	drawRegisters(registers->rsi);
	draw_word("RDI = 0x");
	drawRegisters(registers->rdi);
	draw_word("R8  = 0x");
	drawRegisters(registers->r8);
	draw_word("R9  = 0x");
	drawRegisters(registers->r9);
	draw_word("R10 = 0x");
	drawRegisters(registers->r10);
	draw_word("R11 = 0x");
	drawRegisters(registers->r11);
	draw_word("R12 = 0x");
	drawRegisters(registers->r12);
	draw_word("R13 = 0x");
	drawRegisters(registers->r13);
	draw_word("R14 = 0x");
	drawRegisters(registers->r14);
	draw_word("R15 = 0x");
	drawRegisters(registers->r15);
}
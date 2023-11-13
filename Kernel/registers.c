#include <registers.h>
#include "../drivers/include/videoDriver.h"
int flag_snapshot_taken = 0;
void print_registers(register_struct_t * registers){
	if (flag_snapshot_taken == 0){
		draw_word_color(RED, "Error - No hay snapshot de registros\nPresione(';') para crear uno\n");
		return;
	}
	draw_word("RIP = 0x");
	draw_registers(registers->rip);
	draw_word("RAX = 0x");
	draw_registers(registers->rax);
	draw_word("RBX = 0x");
	draw_registers(registers->rbx);
	draw_word("RCX = 0x");
	draw_registers(registers->rcx);
	draw_word("RDX = 0x");
	draw_registers(registers->rdx);
	draw_word("RSP = 0x");
	draw_registers(registers->rsp);
	draw_word("RBP = 0x");
	draw_registers(registers->rbp);
	draw_word("RSI = 0x");
	draw_registers(registers->rsi);
	draw_word("RDI = 0x");
	draw_registers(registers->rdi);
	draw_word("R8  = 0x");
	draw_registers(registers->r8);
	draw_word("R9  = 0x");
	draw_registers(registers->r9);
	draw_word("R10 = 0x");
	draw_registers(registers->r10);
	draw_word("R11 = 0x");
	draw_registers(registers->r11);
	draw_word("R12 = 0x");
	draw_registers(registers->r12);
	draw_word("R13 = 0x");
	draw_registers(registers->r13);
	draw_word("R14 = 0x");
	draw_registers(registers->r14);
	draw_word("R15 = 0x");
	draw_registers(registers->r15);
}
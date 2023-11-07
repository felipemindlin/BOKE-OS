#include <time.h>
#include <stdint.h>
#include "include/defs.h"
#include "naiveConsole.h"
#include "include/interrupts.h"
#include "include/exceptions.h"
#include "registers.h"
#include "drivers/include/videoDriver.h"
#include "keyboard_buffer.h"
#include "include/lib.h"
#include "time.h"
#include "drivers/include/soundDriver.h"
#include "drivers/include/keyboard_driver.h"
#include "syscalls.h"
#include <scheduler.h>
#include <process.h>
#include <mysemaphore.h>
#include <libasm.h>

static void int_20();
static void int_21();
static int int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);

typedef void (*InterruptHandler)(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9);
InterruptHandler interruption[256] = {
    [0] = &int_20,
    [1] = &int_21,
    [96] = (InterruptHandler)int_80,
};

//maneja las interrupciones y recibe el numero de la interrupcion y los registros en el momento de la interrupcion
void irqDispatcher(uint64_t irq, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {
    if (irq >= 0 && irq < 256 && interruption[irq] != NULL) {
        InterruptHandler handler = interruption[irq];
        handler(rdi, rsi, rdx, rcx, r8, r9);
		return;
    }
}

void int_20() {
	timer_handler();
}

void int_21() {
	keyboard_handler();
}

//maneja las syscalls y recibe el numero de la syscall y los registros en el momento de la syscall
int int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9) {

    switch (rdi)
	{
	case 1:
		sys_write((char*)rsi, (int) rdx, (int) rcx);
		break;
	
	case 2:
		sys_read((char*)rsi, (int) rdx, (int) rcx);
		break;
	case 3:
		TimeClock((char*)rsi);
		break;
	case 4:
		printRegAsm();
		break;
	case 5:
		paintScreen((uint64_t)rsi);
		break;
	case 6:
		boke();
		break;
	case 7:
		drawRectangle(rsi, rdx, rcx, r8, r9);
		break;
	case 8:
		drawBall(rsi, rdx, rcx, r8);
		break;
	case 9:
		clearColor(rsi);
		break;
	case 10:
		put_square(rsi, rdx, rcx, r8);
		break;
	case 11:
		 return getBufferPosition();
		 break;
	case 12:
		return getCharAt(rsi);
		break;
	case 13:
		sleepms(rsi);
		break;
	case 14:
		return ticks_elapsed();
		break;
	case 15:
		setFontSize(rsi);
		break;
	case 16:
		drawWordColorAt(rsi, (char*)rdx, rcx, r8);
		break;
	case 17:
		characterAt(rsi, (char)rdx, rcx, r8);
		break;
	case 18:
		beep(rsi, rdx);
		break;
	case 19:
		add_process_to_creation_queue(1, "ps", 0x0000000000001000, 0x0000000000001000, &print_process, NULL);
		break;
	case 20:
		add_process_to_creation_queue(1, "print_mem", 0x0000000000001000, 0x0000000000001000, &printMem, NULL);
		break;
	case 21:
		kill_process(rsi);
		break;
	case 22:
		change_process_priority(rsi, rdx);
		break;
	case 23:
		block_process(rsi);
		break;
	case 24:
		loop();
		break;
	case 25:
		int to_ret = create_and_insert_process_from_current(rsi, rdx, rcx, r8, r9);
		return to_ret;
		break;
	case 26:
		return my_sem_wait(rsi);
		break;
	case 27:
		return my_sem_post(rsi);
		break;
	case 28:
		my_sem_close(rsi);
		break;
	case 29:
		return my_sem_open(rsi, rdx);
		break;
	case 30:
		finish_current_tick();
		break;
	case 31:
		return waitpid(rsi);
		break;
	case 32:
		return get_current_pcb()->process->pid;
		break;
	default:
		return 0;
	}
	return 0;
}
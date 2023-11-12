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
#include "include/pipe.h"

//static void example_func(int rdi, int rsi, int rdx, int rcx, int r8, int r9, int r10);
static uint64_t int_20();
static uint64_t int_21();
static uint64_t int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t r10);

typedef uint64_t (*InterruptHandler)(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t r10);
InterruptHandler interruption[256] = {
    [0] = &int_20,
    [1] = &int_21,
    [96] = (InterruptHandler)int_80,
};

//maneja las interrupciones y recibe el numero de la interrupcion y los registros en el momento de la interrupcion
uint64_t irqDispatcher(uint64_t irq, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t r10) {
    if (irq >= 0 && irq < 256 && interruption[irq] != NULL) {
        InterruptHandler handler = interruption[irq];
        return handler(rdi, rsi, rdx, rcx, r8, r9, r10);
    }
	return -1;
}
uint64_t int_20() {
	timer_handler();
	return 0;
}

uint64_t int_21() {
	keyboard_handler();
	return 0;
}
int fd[2] = {0,0};
//maneja las syscalls y recibe el numero de la syscall y los registros en el momento de la syscall
uint64_t int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t r10) {

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
		add_process_to_creation_queue(1, 1, "ps", 0x0000000000001000, 0x0000000000001000, &print_process, NULL,fd);
		break;
	case 20:
		add_process_to_creation_queue(1, 1, "print_mem", 0x0000000000001000, 0x0000000000001000, &printMem, NULL,fd);
		break;
	case 21:
		// return kill_process(rsi);
		return force_kill(rsi);
		break;
	case 22:
		change_process_priority(rsi, rdx);
		break;
	case 23:
		return block_process(rsi);
		break;
	case 24:
		return force_kill(rsi);
		break;
	case 25:
		return create_and_insert_process_from_current_standard((char*)rsi, rdx, (size_t*)rcx,(void*) r8,(void*) r9,(int*) r10); // foreground must be a parameter
		break;
	case 26:
		return my_sem_wait((char*)rsi);
		break;
	case 27:
		return my_sem_post((char*)rsi);
		break;
	case 28:
		my_sem_close((char*)rsi);
		break;
	case 29:
		return my_sem_open(rsi, (char*)rdx);
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
	case 33:
		return (uint64_t) malloc((uintptr_t)rsi);
		break;
	case 34:
		free((void*)rsi);
		break;
	case 35:
		return getPipeID(rsi);
		break;
	case 36:
		close_pipe(rsi);
		break;
	case 37:
		return create_pipe(rsi);
		break;
	case 38:
		return create_pipe_anonymous();
		break;
	case 39:
		drawWordColor(rsi, (char*)rdx);;
		break;
	case 40:
		finish_current_tick();
		break;
	default:
		return 0;
		break;
	}
	return 0;
}
/*
static void example_func(int rdi, int rsi, int rdx, int rcx, int r8, int r9, int r10){
	drawWord("\nr10: ");
	drawNumber(r10);
	drawWord("\nr9: ");
	drawNumber(r9);
	drawWord("\nr8: ");
	drawNumber(r8);
	drawWord("\nrcx: ");
	drawNumber(rcx);
	drawWord("\nrdx: ");
	drawNumber(rdx);
	drawWord("\nrsi: ");
	drawNumber(rsi);
	drawWord("\nrdi: ");
	drawNumber(rdi);
}
*/

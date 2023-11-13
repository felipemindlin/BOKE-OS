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


static uint64_t int_20();
static uint64_t int_21();
static uint64_t int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t r10);

typedef uint64_t (*interrupt_handler)(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t r10);
interrupt_handler interruption[256] = {
    [0] = &int_20,
    [1] = &int_21,
    [96] = (interrupt_handler)int_80,
};


uint64_t irq_dispatcher(uint64_t irq, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t r10){
    if (irq >= 0 && irq < 256 && interruption[irq] != NULL){
        interrupt_handler handler = interruption[irq];
        return handler(rdi, rsi, rdx, rcx, r8, r9, r10);
    }
	return -1;
}
uint64_t int_20(){
	timer_handler();
	return 0;
}

uint64_t int_21(){
	keyboard_handler();
	return 0;
}
int fd[2] = {0, 0};

uint64_t int_80(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx, uint64_t r8, uint64_t r9, uint64_t r10){

    switch (rdi)
	{
	case 1:
		sys_write((char*)rsi, (int) rdx, (int) rcx);
		break;
	
	case 2:
		sys_read((char*)rsi, (int) rdx, (int) rcx);
		break;
	case 3:
		Time_clock((char*)rsi);
		break;
	case 4:
		print_reg_asm();
		break;
	case 5:
		paint_screen((uint64_t)rsi);
		break;
	case 6:
		boke();
		break;
	case 7:
		draw_rectangle(rsi, rdx, rcx, r8, r9);
		break;
	case 8:
		draw_ball(rsi, rdx, rcx, r8);
		break;
	case 9:
		clear_color(rsi);
		break;
	case 10:
		put_square(rsi, rdx, rcx, r8);
		break;
	case 11:
		 return get_buffer_position();
		 break;
	case 12:
		return get_char_at(rsi);
		break;
	case 13:
		sleepms(rsi);
		break;
	case 14:
		return ticks_elapsed();
		break;
	case 15:
		set_font_size(rsi);
		break;
	case 16:
		draw_word_colorAt(rsi, (char*)rdx, rcx, r8);
		break;
	case 17:
		character_at(rsi, (char)rdx, rcx, r8);
		break;
	case 18:
		beep(rsi, rdx);
		break;
	case 19:
		add_process_to_creation_queue(1, 1, "ps", 0x0000000000001000, 0x0000000000001000, &print_process, NULL, fd);
		break;
	case 20:
		add_process_to_creation_queue(1, 1, "print_mem", 0x0000000000001000, 0x0000000000001000, &print_mem, NULL, fd);
		break;
	case 21:
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
		return create_and_insert_process_from_current_standard((char*)rsi, rdx, (size_t*)rcx,(void*) r8,(void*) r9,(int*) r10); 		break;
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
		return get_pipe_id(rsi);
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
		draw_word_color(rsi, (char*)rdx);;
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

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <init_info.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include "../memory_manager/include/memory_manager.h"
#include <scheduler.h>
#include "drivers/include/videoDriver.h"
#include "include/interrupts.h"
#include "include/libasm.h"
#include <idle.h>
#include "./include/mysemaphore.h"
#include "include/pipe.h"

static void * const sample_code_module_Address = (void*)0x400000;
static void * const sample_data_module_address = (void*)0x500000;

int fdDefault[] = {0, 0};

void clear_bss(void * bss_address, uint64_t bss_size){
	memset(bss_address, 0, bss_size);
}

void * ret_userland(){
	return sample_code_module_Address;
}

void * get_stack_base(){
	return (void*)((uint64_t)&end_of_kernel 
	+ page_size * 8 - sizeof(uint64_t));
}

void * initialize_kernel_binary(){
	void * module_addresses[] = {
		sample_code_module_Address,
		sample_data_module_address
	};

	load_modules(&end_of_kernelBinary, module_addresses);

	clear_bss(&bss, &end_of_kernel - &bss);

	return get_stack_base();
	
}

extern uint64_t test_sync(char *argv[]);

int main(){
	load_idt();
	set_font_size(1);
	init_mm((void *)0x0000000000600000, 0x0000000002700000);
	init_scheduler(1);
	initialize_sems();
	init_keyboard_sem();
	init_pipes();

	int shell_pid = create_and_insert_process(1, 1, "shell", 0x0000000000001000, 0x0000000000001000, ret_userland(), NULL, 0, 0); 	change_process_priority(create_and_insert_process(1, 0, "idle", 1,  0x0000000000001000, &idle, NULL, 0, 0), IDLE_PRIORITY); 
	enable_multitasking(shell_pid);

	while(1);
	return 0;
}


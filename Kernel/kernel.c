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

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

int fdDefault[] = {0, 0};

void clear_bss(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * ret_userland(){
	return sampleCodeModuleAddress;
}

void * get_stack_base()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	load_modules(&endOfKernelBinary, moduleAddresses);

	clear_bss(&bss, &endOfKernel - &bss);

	return get_stack_base();
	
}

extern uint64_t test_sync(char *argv[]);

int main()
{
	load_idt();
	set_font_size(1);
	init_mm((void *)0x0000000000600000, 0x0000000002700000);
	init_scheduler(2);
	initialize_sems();
	init_keyboard_sem();
	init_pipes();

	// SHOULD WE CREATE AN "ALMIGHTY" PROCESS that is the ancestor of all processes?
	
	// SHELL IS INIT PROCESS

	int shell_pid = create_and_insert_process(1, 1, "shell", 0x0000000000001000, 0x0000000000001000, ret_userland(), NULL,0,0); // id=1 indicates OS created it
	change_process_priority(create_and_insert_process(1, 0, "idle",1,  0x0000000000001000, &idle, NULL,0,0), IDLE_PRIORITY); // id=1 indicates OS created it

	//create_and_insert_process(1, 1, "test", 1, 0x0000000000001000, &function1, args,0,0); // id=1 indicates OS created it

	//create_and_insert_process(1, 0, "write",4096,4096,&write_to_pipe,NULL,0,0);
	//create_and_insert_process(1, 0, "read",4096,4096,&read_from_pipe,NULL,0,0);

	enable_multitasking(shell_pid);

	draw_word("SOMETHING WENT WRONG\n");
	while(1);
	return 0;
}


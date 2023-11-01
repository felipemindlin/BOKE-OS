#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <init_info.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <memory_manager.h>
#include <scheduler.h>
#include "drivers/include/videoDriver.h"
#include "include/interrupts.h"
#include "include/libasm.h"
#include <idle.h>


static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;
static void *const systemVar = (void *)0x0000000000005A00;

typedef int (*EntryPoint)();

int a=1;

void clearBSS(void * bssAddress, uint64_t bssSize)
{
	memset(bssAddress, 0, bssSize);
}

void * retUserland(){
	return sampleCodeModuleAddress;
}

void * getStackBase()
{
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * 8				//The size of the stack itself, 32KiB
		- sizeof(uint64_t)			//Begin at the top of the stack
	);
}

void * initializeKernelBinary()
{
    

/*	
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();*/
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
/*	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();
*/
	clearBSS(&bss, &endOfKernel - &bss);
/*
	ncPrint("  text: 0x");
	ncPrintHex((uint64_t)&text);
	ncNewline();
	ncPrint("  rodata: 0x");
	ncPrintHex((uint64_t)&rodata);
	ncNewline();
	ncPrint("  data: 0x");
	ncPrintHex((uint64_t)&data);
	ncNewline();
	ncPrint("  bss: 0x");
	ncPrintHex((uint64_t)&bss);
	ncNewline();

	ncPrint("[Done]");
	ncNewline();
	ncNewline();
	*/
	return getStackBase();
	
}

int function1(){
	int c=0;
	int b=0;
	while(1){
		c += ticks_elapsed();
		if(c % 10000000 == 1){
			drawWordColor(RED,"asd");
			drawNumberColor(RED,c);
			b++;
			if(b==2){
				break;
			}
			c+=2;
		}
	}
	return 0;
}
uint64_t test_sync(int argc, char *argv[]);
int main()
{
	load_idt();
	setFontSize(1);
	init_mm((void *)0x0000000000600000, 0x0000000000020000);
	init_scheduler(2);
	
	/*
	uint32_t *mem_amount = (void *)(systemVar + 132);       // MiB
    uint64_t mem_amount_bytes = (*mem_amount) * (1 << 20);  // bytes
    uint32_t *userlandSize = (uint32_t *)0;
	init_mm(*userlandSize, mem_amount_bytes);*/

	// SHOULD WE CREATE AN "ALMIGHTY" PROCESS that is the ancestor of all processes?
	
	int shell_pid = create_process(0, "shell", 0x0000000000001000, 0x0000000000001000, retUserland(), NULL); // id=0 indicates OS created it
	create_process(0, "test", 1, 0x0000000000001000, &function1, NULL); // id=0 indicates OS created it
	change_process_priority(create_process(0, "idle",1,  0x0000000000001000, &idle, NULL), IDLE_PRIORITY); // id=0 indicates OS created it
		char *test_args[] = {"3", "1"}; // Test with 10 iterations and semaphores enabled
    test_sync(2, test_args);

    // Now run the test_sync function without semaphores
    char *test_args_no_sem[] = {"3", "0"}; // Test with 10 iterations and semaphores disabled
    test_sync(2, test_args_no_sem);
	drawWord("\n paso");
	enable_multitasking(shell_pid);

	drawWord("SOMETHING WENT WRONG\n");
	while(1);
	return 0;
}

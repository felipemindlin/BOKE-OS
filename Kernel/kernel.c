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
int funcion(){
	while(1){
		if(a!=0){
			drawWord("Peru\n");
			a=0;
		}
	}
}

int funcion2(){
	while(1){
		if(a!=1){
			drawWord("Argentina\n");
			a=1;
		}
	}
}

int funcion3(){
	while(1){
		if(a!=2){
			drawWord("Chile\n");
			a=2;
		}
	}
}

int funcion4(){
	while(1){
		if(a!=3){
			drawWord("Brasil\n");
			a=3;
		}
	}
}

int main()
{
	load_idt();
	setFontSize(1);
	init_mm((void *)0x0000000000600000, 0x0000000000020000);
	init_scheduler(4);
	
	/*
	uint32_t *mem_amount = (void *)(systemVar + 132);       // MiB
    uint64_t mem_amount_bytes = (*mem_amount) * (1 << 20);  // bytes
    uint32_t *userlandSize = (uint32_t *)0;
	init_mm(*userlandSize, mem_amount_bytes);*/

	// SHOULD WE CREATE AN "ALMIGHTY" PROCESS that is the ancestor of all processes?
	// int shell_pid = create_process("shell", 0x0000000000001000, 0x0000000000001000, retUserland(), NULL);
	int shell_pid = create_process("shell", 0x0000000000001000, 0x0000000000001000, &funcion3, NULL);
	//create_process("test",0x0000000000010000,  0x0000000000001000, &funcion, NULL);
	create_process("peru", 0x0000000000001000, 0x0000000000001000, &funcion, NULL);
	change_process_priority(create_process("idle",1,  0x0000000000001000, &idle, NULL), IDLE_PRIORITY);
	create_process("arg", 0x0000000000001000, 0x0000000000001000, &funcion2, NULL);
	create_process("brasil", 0x0000000000001000, 0x0000000000001000, &funcion4, NULL);
	// CON TEST1 EL SCHEDULER NO FUNCIONA BIEN. SI AGREGO TEST2, NECESITAR
	//start_next_process();
	enable_multitasking(shell_pid);
	// create_process("test",0x0000000000010000,  0x0000000000001000, &funcion, NULL);
	
	
	
	// process_t *shell_process = create_process("shell", 0x0000000000001000, 0x0000000000001000, retUserland(), NULL);
	// enable_multitasking(shell_process);

	
	//((EntryPoint)sampleCodeModuleAddress)();
	drawWord("SOMETHING WENT WRONG\n");
	while(1);
	return 0;
}

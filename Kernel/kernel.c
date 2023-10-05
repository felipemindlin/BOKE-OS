#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <init_info.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <idtLoader.h>
#include <memory_manager.h>
#include "drivers/include/videoDriver.h"
#include "include/interrupts.h"
#include "include/libasm.h"

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();


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
	char buffer[10];

	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("CPU Vendor:");
	ncPrint(cpuVendor(buffer));
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

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
	return getStackBase();
	
}

int main()
{
	load_idt();
	setFontSize(1);
	initializeHeap();

	drawWord("\nchar *a = malloc(19);\n");
 	char *a = malloc(19);
	for(int i=0; i<19; i++){
		a[i] = (char) i;
	}
    printMem();
	for(int i=0; i<19; i++){
		drawNumber(a[i]);
		drawWord(", ");
	}
	drawWord("\n\n");
	while(1);
    drawWord("\n\n");
	memcpy(a, "asdasdasd", 10);
	drawWord(a);
	free(a);
	drawWord("\n\n");
	char *b = malloc(5);
	memcpy(b, "asda", 5);
	drawWord(b);
	newline();
	drawNumber(text);
	newline();
	drawNumber(rodata);
	newline();
	drawNumber(data);
	newline();
	drawNumber(bss);
	newline();
	drawNumber(endOfKernelBinary);
	newline();
	drawNumber(endOfKernel);
    // drawWord("\nfree(a);\n");
    // free(a);
    // printMem();

    // drawWord("\nchar *c = malloc(215);\n");
    // char *c = malloc(215);
    // printMem();

	// drawWord("\nfree(b);\n");
    // free(b);
    // printMem();

    // drawWord("\nchar *d = malloc(340);\n");
    // char *d = malloc(340);
	// if (d == (void*)-1){
	// 	drawWordColor(RED,"NOT ENOUGH SPACE");
	// 	return 1;
	// }
    // printMem();

    // drawWord("\nchar *e = malloc(120);\n");
    // char *e = malloc(120);
    // printMem();

    // drawWord("\nchar *f = malloc(170);\n");
    // char *f = malloc(170);
    // printMem();

    // drawWord("\nfree(d);\n");
	// free(d);
	// printMem();

    // drawWord("\nchar *g = malloc(370);\n");
    // char *g = malloc(370);
    // printMem();

    // drawWord("\nfree(e);\n");
    // free(e);
    // printMem();

    // drawWord("\nfree(f);\n");
    // free(g);
    // printMem();

    // drawWord("\nfree(c);\n");
	// free(c);
    // printMem();

	// char * a = malloc(99);
	// char * b = malloc(75);
	// memcpy(a,"aaaaaaaaaaaaaa", 15);
	// printMem();
	// drawWord("\n\n");
	// free(a);
	// printMem();
	// drawWord("\n\n");
	// char * c = malloc(115);
	// printMem();
	// drawWord("\n\n");
	// char * d = malloc(240);
	// if (d == (void*)-1)
	// 	return 0;
	// printMem();
	// drawWord("\n\n");
	// free(b);
    // printMem();
    // drawWord("\n\n");

    // free(c);
    // printMem();
    // drawWord("\n\n");

    // free(d);
    // printMem();
    // drawWord("\n\n");

	// a[3] = 1;
	// ((EntryPoint)sampleCodeModuleAddress)();
	
	while(1);
	return 0;
}

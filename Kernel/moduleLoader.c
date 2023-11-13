#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>

static void load_module(uint8_t ** module, void * targetModuleAddress);
static uint32_t read_uint_32(uint8_t ** address);

void load_modules(void * payload_start, void ** targetModuleAddress){
	int i;
	uint8_t * currentModule = (uint8_t*)payload_start;
	uint32_t moduleCount = read_uint_32(&currentModule);

	for (i = 0; i < moduleCount; i++)
		load_module(&currentModule, targetModuleAddress[i]);
}

static void load_module(uint8_t ** module, void * targetModuleAddress){
	uint32_t moduleSize = read_uint_32(module);

	nc_print("  Will copy module at 0x");
	nc_print_hex((uint64_t)*module);
	nc_print(" to 0x");
	nc_print_hex((uint64_t)targetModuleAddress);
	nc_print(" (");
	nc_print_dec(moduleSize);
	nc_print(" bytes)");

	memcpy(targetModuleAddress, *module, moduleSize);
	*module += moduleSize;

	nc_print(" [Done]");
	nc_newline();
}

static uint32_t read_uint_32(uint8_t ** address){
	uint32_t result = *(uint32_t*)(*address);
	*address += sizeof(uint32_t);
	return result;
}

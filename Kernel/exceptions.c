

#include "include/exceptions.h"
#include "drivers/include/videoDriver.h"
#include "registers.h"
#include "syscalls.h"

#define ZERO_EXCEPTION_ID 0
#define INVALID_OP_EXCEPTION_ID 1
#define GENERAL_PROTECTION_EXCEPTION_ID 2

//maneja las excepciones y recibe el numero de la excepcion y los registros en el momento de la excepcion
void exception_dispatcher(int exception, register_struct_t * registers){
	if (exception == ZERO_EXCEPTION_ID){
		zero_division();
	}
	else if(exception == INVALID_OP_EXCEPTION_ID){
		invalid_opcode();
	} else if(exception == GENERAL_PROTECTION_EXCEPTION_ID){
		guru_meditation();
	}
	printRegisters(registers);
	draw_word_color(RED, "Press any key to continue");
	char c;
	sys_read(&c, 1, 0);
}
//excepcion de division por cero
 void zero_division(){
	flag_snapshot_taken = 1;
	draw_word_color(RED, "FATAL ERROR: No se puede dividir por 0");
	newline();
}
//excepcion de codigo invalido
 void invalid_opcode(){
	flag_snapshot_taken = 1;
	draw_word_color(RED, "FATAL ERROR: Invalid opcode");
	newline();
}

void guru_meditation(){
	clear_color(BLUE);
	draw_word_colorLen(RED, "GURU MEDITATION\n", 17);
	draw_word_colorLen(RED, "FATAL ERROR IN KERNEL\n", 24);
	while(1);
}
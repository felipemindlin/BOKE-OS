#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "utils.h"
#include "UserSyscalls.h"

#define COMMAND_LEN 19
#define COLOR_LEN 5
#define MAX_COMMAND_LENGTH 256
enum Commands {
    HELP,
    TIME,
    REG_STATE,
    PONG,
    SETCOLOR,
    DIV0,
    INVALOP,
    BOKE,
    PS,
    MEM,
    KILL,
    NICE,
    BLOCK,
    CAT,
    WC,
    PHYLO,
    FILTER,
    LOOP,
    SEM,
};
enum Colors {
    GREEN_,
    BLUE_,
    BLACK_,
    YELLOW_,
    ORANGE_,
};



void __seek_command__(char * command); //Busca el comando ingresado por el usuario
void __call_command__(int i, char * command); //Llama al comando ingresado por el usuario
void help(); //Imprime la lista de comandos
void time(); //Imprime la hora actual
void setbg(char * command); //Cambia el color de fondo de la pantalla
void setbgEnum(int i); 
void findColor(char * color); //Busca el color ingresado por el usuario
void call_boke(); 
void Div0(); //Divide por 0 y genera una excepcion
void invalidOp(); //Genera una excepcion de operacion invalida
void call_ps();
void parse_command(char *command, char *parsed_command, int *pid, int *priority);
void invalid_pid();
void cat();
void wc();
int is_vowel(char c);
void filter();
#endif /*__COMMANDS_H__*/
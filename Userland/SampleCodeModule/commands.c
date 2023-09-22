#include "commands.h"
#include "UserSyscalls.h"
#include "stdint.h"
#include "utils.h"
#include "pong.h"
#include "funcAsm.h"
#include "colors.h"
#include "shell.h"
static char command_list[COMMAND_LEN][10] = {"HELP", "TIME", "REGSTATE","PONG", "SETCOLOR","DIV0", "INVALOP", "BOKE"};

//busca el comando en la lista de comandos y llama a la funcion correspondiente
void __seek_command__(char * command){
    

    for (int i = 0; i < COMMAND_LEN; i++){
        if (strcmpspace(command_list[i],command) == 0){
                __call_command__(i, command);
                return;
        }
    }
    __call_command__(-1, command);
}

void __call_command__(int i, char * command){
    switch (i)
    {
    case HELP:;
        help();
        return;
    case TIME:;
        time();
        return;
    case REG_STATE:;
        call_regState();
        return;
    case PONG:;
        Pong();
        __shell_init__();    
        return;
    case SETCOLOR:;
        setbg(command);
        return;
    case DIV0:;
        Div0();
        return;
    case INVALOP:;
        invalidOp();
        return;
    case BOKE:;
        call_boke();
        return;
    default:;
        call_sys_write("ERROR - Comando no reconocido",30,2);
        putC('\n');
        return;
    }
}

//imprime la lista de comandos disponibles
void help(){
    call_sys_write("Lista de commandos disponibles:\n", 34,1);
    for (int i = 0; i < COMMAND_LEN; i++){
        call_sys_write(command_list[i],40,1);
        call_sys_write("\n",1,1);
    }
}

void time(){
    char timeClock[50];
    call_timeClock(timeClock);
    char c;
    for (int i = 0; (c = timeClock[i]) != 0; i++){
        putC(c);
    }
    putC('\n');
}


void setbgEnum(int i){
    switch (i){
        case GREEN_:; call_paintScreen(GREEN);
            return;
        case BLUE_:; call_paintScreen(BLUE);
            return;
        case BLACK_: ;call_paintScreen(BLACK);
            return;
        case YELLOW_: ;call_paintScreen(YELLOW);
            return;
        case ORANGE_:; call_paintScreen(ORANGE);
            return;
        default:
            return;
    }
}
static char hexArr[COLOR_LEN][10] = {"GREEN","BLUE","BLACK","YELLOW","ORANGE"};
void findColor(char * color){
    
   
    for (int i = 0; i < COMMAND_LEN; i++){
        if (strcmp(hexArr[i],color) == 0){
                setbgEnum(i);
                return;
        }
    }
    setbgEnum(-1);
}


//setea el color de fondo de la pantalla segun el comando ingresado
void setbg(char * command){
    char partition[20];
    int i=0;
    for (; !null_or_space(command[i]); i++);
    i++;
    int j = 0;
    for (; command[i] != '\0'; i++){
        partition[j++]=command[i];
    }
    partition[j]=0;
    findColor(partition);
}
void Div0(){
    int a = 0;
    int b = 1;
     a = b/a;
}
void invalidOp(){
    invalidOpAsm();
}
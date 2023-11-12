#include <shell.h>
#include <UserSyscalls.h>
#include <commands.h>
#include <buffer.h>
#include "colors.h"

static char buffer[BUFFER_SIZE] = {0};

//Lee linea y la manda a __seek_command__
void lineRead(char * line){
    __seek_command__(line);
    //---
    clear_buffer(buffer);
}

void putLineStart()
{   
    char s[] = "O] ";
    print(s);
    clear_buffer(buffer);
}

/*
La función bufferize() lee caracteres de entrada y los almacena en un búfer hasta que se cumplan ciertas condiciones: 
retroceso (\b), salto de línea (\n) o el búfer alcanza su capacidad máxima. 
Si se detecta un retroceso, se retrocede una posición en el búfer si es posible. 
Si se detecta un salto de línea, muestra el caracter en la salida, 
almacena el búfer en una línea y retorna. 
En caso contrario, si el búfer no ha alcanzado su capacidad máxima, 
se almacena el caracter en el búfer y se muestra en la salida. 
La función finaliza cuando se completa el búfer o no hay más caracteres de entrada.
*/
void bufferize (){
    
    int i = 0;
    int end_of_buffer = 0;
    int put_falg = 0;
    while (i < BUFFER_SIZE) {
        char c = get_c();
        end_of_buffer = (i == BUFFER_SIZE-1);
        if (c == '\b'){
            if ( i > 0)
                i--;
            else put_falg = 1;
        } else if (c == '\n'){
            put_c(c);
            if ( i == 0 ){
                clear_buffer(buffer);
                return;
            }
            buffer[i]=0;
            lineRead(buffer);
            return;
        } else{
            if (!end_of_buffer)
                buffer[i++] = c;
            else
                put_falg = 1;}
        if (!put_falg)
            put_c(c);
        put_falg = 0;
    }
    return;
}

void welcome(){
    call_paint_screen(BLACK);
     char WELCOME_MESSAGE[] = "Welcome to BOKE OS\nEnter a command to proceed:\n(Enter HELP to view all commands)\n";
    for (int j=0; WELCOME_MESSAGE[j] != 0; j++){
        put_c(WELCOME_MESSAGE[j]);
    }
}

int __shell_init__(){
    welcome();
    while (1){
        putLineStart();
        bufferize();
    }
    return 0;
}

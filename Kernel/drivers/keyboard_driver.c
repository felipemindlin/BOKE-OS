#include <lib.h>
#include <naiveConsole.h>
#include "include/keyboard_driver.h"
#include "include/videoDriver.h"
#include <keyboard_buffer.h>
#include "defs.h"
#include "interrupts.h"
#include "registers.h"
#include "include/ScanCodes.h"
#include "../include/pipe.h"
#include <mysemaphore.h>
#include "include/syscalls.h"

void keyboard_handler() {
    uint16_t key = getKey();  // Obtiene el valor de la tecla presionada
    static int shift_pressed = 0, ctrl_pressed = 0;

    
    if (key == NULL)  // Si no se presionó ninguna tecla, retorna
        return;
        
    if (key == 0x2A || key == 0x36)
        shift_pressed = 1;
    else if (key == 0xAA || key == 0xB6)
        shift_pressed = 0;
    else if (key == 0x1D)
        ctrl_pressed = 1;
    else if (key == 0x9D)
        ctrl_pressed = 0;
    
    uint16_t * buff = getBufferAddress();  
    int buff_pos = getBufferPosition();  
    if(ctrl_pressed){
         if(ScanCodes[key] == 'C'){
            drawWord("contorl C");
            kill_foreground_process(get_process_foreground_pid());
            clear_buffer();
            return;
        }
        else if(ScanCodes[key] == 'D'){
            drawWord("contorl D");
            if(get_pcb_entry(get_process_foreground_pid())->process->fr == SHELL){
                key = _EOF_;
                shift_pressed = 0;
            } else if(ScanCodes[key] == 'P'){
                print_process();
            } else if(ScanCodes[key] == 'L'){
            clear();
            return;
            } else {
                send_eof_to_foreground();
                return;
            }
        }
    }
    if(shift_pressed){
        if(ScanCodes[key] == '7'){
            key=0x54;
        }
        else if(ScanCodes[key] == '\\'){
            key=0x55;
        }
    }
   
    // Verifica si hay espacio suficiente en el búfer para almacenar el valor de la tecla
    if (buff_pos+1 < BUFF_SIZE){
        setPos(buff_pos+1); 
        buff[buff_pos+1] = 0;  
    } else {
        setPos(0);  
        buff[0] = 0;  
    }
    
    buff[buff_pos] = key;  // Almacena el valor de la tecla en el búfer
    
    // Verifica si la tecla presionada es un punto y coma (';')
    if (ScanCodes[key] == ';'){
        saveState();  // Guarda el estado actual
        flag_snapshot_taken = 1;  // Establece la bandera indicando que se tomó una instantánea
    }
    
    return; 
}






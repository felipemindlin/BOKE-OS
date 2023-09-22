#include <lib.h>
#include <naiveConsole.h>
#include "include/keyboard_driver.h"
#include "include/videoDriver.h"
#include <keyboard_buffer.h>
#include "defs.h"
#include "interrupts.h"
#include "registers.h"
#include "include/ScanCodes.h"



void keyboard_handler() {
    uint16_t key = getKey();  // Obtiene el valor de la tecla presionada
    
    if (key == NULL)  // Si no se presionó ninguna tecla, retorna
        return;
    
    uint16_t * buff = getBufferAddress();  
    int buff_pos = getBufferPosition();  
    
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






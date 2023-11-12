
#ifndef KEYBOARD_DRIVER_H
#define KEYBOARD_DRIVER_H

#include <stdint.h>


void keyboard_handler(); // Función que maneja las interrupciones del teclado
extern int get_key(); // Función que obtiene el valor de la tecla presionada


#endif /* KEYBOARD_DRIVER_H */
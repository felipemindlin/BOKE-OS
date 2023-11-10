#ifndef _TIME_H_
#define _TIME_H_

void timer_handler(); //Handler de la interrupcion del timer
unsigned long ticks_elapsed(); //Devuelve la cantidad de ticks que pasaron desde que se inicio el sistema
int seconds_elapsed(); //Devuelve la cantidad de segundos que pasaron desde que se inicio el sistema

#endif

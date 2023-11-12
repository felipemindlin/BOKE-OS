#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>

void nc_print(const char * string);
void nc_print_char(char character);
void nc_print_char_color(char character, char color, char back);
void ncNewline();
void nc_printDec(uint64_t value); //Imprime un numero en base 10
void nc_print_hex(uint64_t value); //Imprime un numero en base 16
void nc_printBin(uint64_t value); //Imprime un numero en base 2
void nc_print_base(uint64_t value, uint32_t base); //Imprime un numero en la base que se le pase
void ncClear(); //Limpia la pantalla
void ncBackspace(); //Borra el ultimo caracter
char * TimeClock(); //Devuelve la hora actual
 uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base); //Convierte un numero a la base que se le pase
#endif /* NAIVE_CONSOLE_H */
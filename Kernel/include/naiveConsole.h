#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>

void ncPrint(const char * string);
void ncPrintChar(char character);
void ncPrintCharColor(char character, char color, char back);
void ncNewline();
void ncPrintDec(uint64_t value); //Imprime un numero en base 10
void ncPrintHex(uint64_t value); //Imprime un numero en base 16
void ncPrintBin(uint64_t value); //Imprime un numero en base 2
void ncPrintBase(uint64_t value, uint32_t base); //Imprime un numero en la base que se le pase
void ncClear(); //Limpia la pantalla
void ncBackspace(); //Borra el ultimo caracter
char * TimeClock(); //Devuelve la hora actual
 uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base); //Convierte un numero a la base que se le pase
#endif /* NAIVE_CONSOLE_H */
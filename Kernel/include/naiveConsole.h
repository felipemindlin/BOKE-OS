#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>

void nc_print(const char * string);
void nc_print_char(char character);
void nc_print_char_color(char character, char color, char back);
void ncNewline();
void nc_printDec(uint64_t value); void nc_print_hex(uint64_t value); void nc_printBin(uint64_t value); void nc_print_base(uint64_t value, uint32_t base); void ncClear(); void ncBackspace(); char * TimeClock();  uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base); 
#endif
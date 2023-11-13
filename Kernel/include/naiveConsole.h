#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>

void nc_print(const char * string);
void nc_print_char(char character);
void nc_print_char_color(char character, char color, char back);
void nc_newline();
void nc_print_dec(uint64_t value); void nc_print_hex(uint64_t value); void nc_print_bin(uint64_t value); void nc_print_base(uint64_t value, uint32_t base); void nc_clear(); void nc_backspace(); char * Time_clock();  uint32_t uint_to_base(uint64_t value, char * buffer, uint32_t base); 
#endif
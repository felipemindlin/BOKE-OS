#ifndef _VIDEO_DRIVER_H
#define _VIDEO_DRIVER_H

#include <stdint.h>
#define RED 0xFF0000
#define GREEN 0x00FF00
#define BLUE 0x0000FF
#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define YELLOW 0xFFFF00
#define ORANGE 0xFFA500

#define TO_RED(hex) hex & 0xFF
#define TO_BLUE(hex) (hex >> 8) & 0xFF
#define TO_GREEN(hex) (hex >> 16) & 0xFF

#define FROM_RGB(r, g, b) (r << 16) | (g << 8) | b

#define is_minusc(x) ((x)<='a'?(((x)>='z')?1:0):0)
#define DEFAULT_FONT_SIZE 2
#define EOF 0x01

void paint_screen(uint64_t hexColor);
void fill_section(uint64_t hexColor, int startY, int endY);
void boke();
void draw_rectangle(uint64_t hexColor, uint32_t b, uint32_t h, uint32_t x, uint32_t y);


void draw_circle(uint64_t hexColor, uint32_t b, uint32_t radius, uint32_t x, uint32_t y);

void put_square(uint32_t x, uint32_t y, uint32_t size, uint64_t hexColor);

void change_size(uint32_t new_size);
uint32_t *get_cursor_x();
  
uint32_t *get_cursor_y();

unsigned int get_max_width();

unsigned int get_max_height();

void newline();
void backspace();
void tab();
void character(uint64_t hexColor, char c);
void character_at(uint64_t hexColor, char c, uint32_t x, uint32_t y);
void draw_word_color(uint64_t hexColor, char* word);
void draw_word_colorAt(uint64_t hexColor, char* word, uint32_t x, uint32_t y);
void set_font_size(uint32_t new_size);
uint32_t get_font_size();
void draw_word(char* word);
void invalid_fd(); void clear();
void clear_color(uint64_t hexColor);

void draw_number_color(int value, uint64_t hexColor);

void drawNumber(int value);

void drawRegisters(int value);
void draw_number_colorAt(uint64_t hexColor, int value,  uint32_t x, uint32_t y);

void draw_ball(uint64_t color, int size, int x, int y);
void draw_word_colorLen(uint64_t color, char * buff, int len);
void draw_wordLen(char * buff, int len);
void draw_number_padded(uint64_t hexColor, int number, int width);
void draw_word_padded(uint64_t hexColor,  char* word, int width);
#endif /* _VIDEO_DRIVER_H */
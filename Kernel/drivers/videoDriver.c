#include "include/videoDriver.h"
#include "include/font.h"
#include "../include/naiveConsole.h"
#include <naiveConsole.h>
#include <lib.h>

unsigned int SCREEN_WIDTH = 1024;
unsigned int SCREEN_HEIGHT = 768;
unsigned int BPP = 3;

void draw_char(uint64_t hex_color, char character);
void put_pixel(uint64_t hex_color, uint32_t x, uint32_t y);
void check_line_up();

struct vbe_mode_info_structure { 
	uint16_t attributes;			uint8_t window_a;				uint8_t window_b;				uint16_t granularity;			uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;			uint16_t pitch;				uint16_t width;				uint16_t height;				uint8_t w_char;				uint8_t y_char;				uint8_t planes;
	uint8_t bpp;				uint8_t banks;				uint8_t memory_model;
	uint8_t bank_size;			uint8_t image_pages;
	uint8_t reserved0;
 
	uint8_t red_mask;
	uint8_t red_position;
	uint8_t green_mask;
	uint8_t green_position;
	uint8_t blue_mask;
	uint8_t blue_position;
	uint8_t reserved_mask;
	uint8_t reserved_position;
	uint8_t direct_color_attributes;
 
	uint32_t framebuffer;			uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;		uint8_t reserved1[206];
} __attribute__ ((packed));

typedef struct vbe_mode_info_structure * VBEInfoPtr;

VBEInfoPtr VBE_mode_info = (VBEInfoPtr) 0x0000000000005C00; 

uint64_t bg_color = 0x000000;
uint64_t font_color = 0xFFFFFF;

uint32_t cursorX  = 0;
uint32_t cursorY  = 0;
uint32_t size = DEFAULT_FONT_SIZE;

void set_font_size(uint32_t new_size){
    size = new_size;
}
uint32_t get_font_size(){
    return size;
}

void put_pixel(uint64_t hex_color, uint32_t x, uint32_t y){
	uint8_t * screen = (uint8_t *) ((uint64_t) (VBE_mode_info->framebuffer));
    uint32_t offset = VBE_mode_info->pitch*y + x*3;
    
    screen[offset] = TO_RED(hex_color);
    screen[offset+1] = TO_BLUE(hex_color);
    screen[offset+2] = TO_GREEN(hex_color);
}
uint64_t get_pixel_hex(uint32_t x, uint32_t y){
	uint8_t * screen = (uint8_t *) ((uint64_t) (VBE_mode_info->framebuffer));
    uint32_t offset = VBE_mode_info->pitch*y + x*3;
    
    int b = screen[offset];
    int g = screen[offset+1];
    int r = screen[offset+2];
    return FROM_RGB(r, g, b);
}

void draw_rectangle(uint64_t color, uint32_t x, uint32_t y, uint32_t width, uint32_t height){
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            put_pixel(color, x + j, y + i);
        }
    }
    return;
}

void draw_circle(uint64_t hex_color, uint32_t b, uint32_t radius, uint32_t x, uint32_t y);

void drawSquare(uint64_t hex_color, uint32_t side_length, uint32_t x, uint32_t y){
	draw_rectangle(hex_color, side_length, side_length, x, y);
}

void fill_section(uint64_t hex_color, int startY, int endY){
    for (int y = startY; y < endY; y++){
        for (int x = 0; x < VBE_mode_info->width; x++){
            put_pixel(hex_color, x, y);
        }
    }
}

void boke(){
    int height = VBE_mode_info->height / 3 ;

    fill_section(BLUE, 0, height);


    fill_section(YELLOW, height, height * 2);

 
    fill_section(BLUE, height * 2, VBE_mode_info->height);
}
void clear(){
	clear_color(bg_color);
	return;
}
void clear_color(uint64_t hex_color){
	for (int x = 0; x < VBE_mode_info->width; x++){
		for (int y = 0; y < VBE_mode_info-> height; y++){
			put_pixel(hex_color, x, y);
		}
	}
    cursorX=0;
    cursorY=0;
	return;
}


void paint_screen(uint64_t hex_color){
    bg_color = hex_color;
	for (int x = 0; x < VBE_mode_info->width; x++){
		for (int y = 0; y < VBE_mode_info-> height; y++){
            if (get_pixel_hex(x, y) != font_color && get_pixel_hex(x, y) != RED)
			    put_pixel(hex_color, x, y);
		}
	}
	return;
}

void change_size(uint32_t new_size){
    size=new_size;
}

uint32_t* get_cursor_x(){
    return &cursorX;
}

uint32_t* get_cursor_y(){
    return &cursorY;
}

uint32_t* getSize(){
    return &size;
}

unsigned int get_max_height(){
	return SCREEN_HEIGHT;
}

unsigned int get_max_width(){
	return SCREEN_WIDTH;
}


void put_square(uint32_t x, uint32_t y, uint32_t size, uint64_t hex_color){
    for (uint32_t i = 0; i < size; i++){
        for (uint32_t j = 0; j < size; j++){
            put_pixel(hex_color, x + i, y + j);
        }
    }
}

void backspace(){
     if (cursorX > 0){
            cursorX -= size*8;
        } else if (cursorY > 0 && cursorX == 0){                         cursorY -= size*16;
            cursorX = get_max_width() - size*8;             
        }
        		draw_rectangle(bg_color, cursorX, cursorY, size*8, size*16);
}

void newline(){
    cursorX = 0;
    cursorY += (size*16);
    check_line_up();
    return;
}

void tab(){     int tab_width = 5;
        int spaces = tab_width - (cursorX / (size*8)) % tab_width;

        for (int i = 0; i < spaces; i++){
            draw_char(WHITE, ' ');
            cursorX += size*8;
        }
        return;
}

void draw_number_color(int value, uint64_t hex_color){
    char buffer[256] = {0};
    uint_to_base(value, buffer, 10);
    draw_word_color(hex_color, buffer);
}


void draw_number(int value){
    draw_number_color(value, WHITE);
}

void draw_registers(int value){
        char buffer[256] = {0};
        uint_to_base(value, buffer, 16);
        draw_word_color(WHITE, buffer);
     newline();
}

void draw_number_color_at(uint64_t hex_color, int value,  uint32_t x, uint32_t y){
    char buffer[256] = {0};
    uint_to_base(value, buffer, 10);
    draw_word_colorAt(hex_color, buffer, x, y);
}

void move_one_line_up(){
    char* dst = (char*)(uintptr_t)(VBE_mode_info->framebuffer);     char* src = dst + VBE_mode_info->pitch * size * 16;     uint64_t numBytes = VBE_mode_info->pitch * (VBE_mode_info->height - size * 16); 
    memcpy(dst, src, numBytes);         draw_rectangle(bg_color, 0, VBE_mode_info->height - size*16, 1024, size*16 );
    cursorY -= (size * 16); }

int is_white_space(char c){
 return  c==' ' || c=='\v' || c=='\r' || c=='\a' || c=='\f';
}

void character(uint64_t hex_color, char c){
        if (c == '\b'){             backspace();
            return;
        }else if (is_white_space(c)){             cursorX += size*8;             return;
        } else if (c == '\n'){             newline();
            return;
        }
        else if(c == '\t'){
            tab();
            return;
        }
                check_line_up();
        draw_char(hex_color, c);
        cursorX += size*8;
        return;
}

void check_line_up(){
    if (cursorX >= get_max_width()){
        cursorX = 0;
        cursorY += size*16;
    }
    if (cursorY >= get_max_height()){ 
        cursorX = 0;
        move_one_line_up();
    }
}

void character_at(uint64_t hex_color, char c, uint32_t x, uint32_t y){
    uint32_t auxX = cursorX;
    uint32_t auxY = cursorY;
    cursorX = x;
    cursorY = y;
    character(hex_color, c);
    cursorX = auxX;
    cursorY = auxY;
}

void draw_word_color(uint64_t hex_color, char* word){
    for (int i=0; word[i] != 0; i++){
        character(hex_color, word[i]);
    }
}

void draw_word_colorAt(uint64_t hex_color, char* word, uint32_t x, uint32_t y){
    uint32_t auxX = cursorX;
    uint32_t auxY = cursorY;
    cursorX = x;
    cursorY = y;
    draw_word_color(hex_color, word);
    cursorX = auxX;
    cursorY = auxY;
}

void draw_word(char* word){
    draw_word_color(WHITE, word);
}

void draw_word_colorLen(uint64_t color, char * buff, int len){
 for (int i=0; buff[i] != 0 && i<len; i++){
        character(color, buff[i]);
    }
}
void draw_word_len(char * buff, int len){
    draw_word_colorLen(WHITE, buff, len);
}

void draw_char(uint64_t hex_color, char character){
    int a = cursorX;      int x = a;      int y = cursorY;      int start = character - 33;      if(start>92*32 || start<0 ){
        start = 3;
    }
        if (is_minusc(character))
        start = character - 'a';
    
    if (character == ' '){
        return;      }

    for (int i = 0; i < 32; i++){
        if (i % 2 == 0 && i != 0){
            y += size;              a = x;          }
        
                font[i + (start * 32)] & (char)0x01 ? put_square(a, y, size, hex_color) : 0;
        
        a += size;          
        uint8_t aux = 0x02;
        for (int j = 0; j < 8; j++){
                        ((uint8_t)font[i + (start * 32)] & (uint8_t)aux) >> j ? put_square(a, y, size, hex_color) : 0;
            a += size;              aux <<= 1;          }
    }
}

void invalid_fd(){
    draw_word_color(RED, "Invalid file descriptor");
    return;
}

void draw_ball(uint64_t color, int size, int x, int y){
    int radius = size / 2;
    int center_x = x + radius;
    int center_y = y + radius;
    int squared_radius = radius * radius;

    for (int y = -radius; y <= radius; y++){
        for (int x = -radius; x <= radius; x++){
            if ((x * x) + (y * y) <= squared_radius){
                put_pixel(color, center_x + x, center_y + y);
            }
        }
    }
}

void draw_word_padded(uint64_t hex_color, char* word, int width){
    int word_length = str_len(word); 
    draw_word_color(hex_color, word); 
    for (int i = word_length; i < width; i++){
        character(hex_color, ' '); 
    }
}

void draw_number_padded(uint64_t hex_color, int number, int width){
    char buffer[256];
    uint_to_base(number, buffer, 10); 
    draw_word_padded(hex_color, buffer, width);
}

#include "include/videoDriver.h"
#include "include/font.h"
#include "../include/naiveConsole.h"
#include <naiveConsole.h>
#include <lib.h>

unsigned int SCREEN_WIDTH = 1024;
unsigned int SCREEN_HEIGHT = 768;
unsigned int BPP = 3;

void drawChar(uint64_t hexColor, char character);
void putPixel(uint64_t hexColor, uint32_t x, uint32_t y);
void checkLineUp();

struct vbe_mode_info_structure { 
	uint16_t attributes;		// deprecated, only bit 7 should be of interest to you, and it indicates the mode supports a linear frame buffer.
	uint8_t window_a;			// deprecated
	uint8_t window_b;			// deprecated
	uint16_t granularity;		// deprecated; used while calculating bank numbers
	uint16_t window_size;
	uint16_t segment_a;
	uint16_t segment_b;
	uint32_t win_func_ptr;		// deprecated; used to switch banks from protected mode without returning to real mode
	uint16_t pitch;			// number of bytes per horizontal line
	uint16_t width;			// width in pixels
	uint16_t height;			// height in pixels
	uint8_t w_char;			// unused...
	uint8_t y_char;			// ...
	uint8_t planes;
	uint8_t bpp;			// bits per pixel in this mode
	uint8_t banks;			// deprecated; total number of banks in this mode
	uint8_t memory_model;
	uint8_t bank_size;		// deprecated; size of a bank, almost always 64 KB but may be 16 KB...
	uint8_t image_pages;
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
 
	uint32_t framebuffer;		// physical address of the linear frame buffer; write here to draw to the screen
	uint32_t off_screen_mem_off;
	uint16_t off_screen_mem_size;	// size of memory in the framebuffer but not being displayed on the screen
	uint8_t reserved1[206];
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

// funcion que dibuja un pixel en la pantalla
void putPixel(uint64_t hexColor, uint32_t x, uint32_t y) {
	uint8_t * screen = (uint8_t *) ((uint64_t) (VBE_mode_info->framebuffer));
    uint32_t offset = VBE_mode_info->pitch*y + x*3;
    
    screen[offset] = TO_RED(hexColor);
    screen[offset+1] = TO_BLUE(hexColor);
    screen[offset+2] = TO_GREEN(hexColor);
}
uint64_t getPixelHex(uint32_t x, uint32_t y) {
	uint8_t * screen = (uint8_t *) ((uint64_t) (VBE_mode_info->framebuffer));
    uint32_t offset = VBE_mode_info->pitch*y + x*3;
    
    int b = screen[offset];
    int g = screen[offset+1];
    int r = screen[offset+2];
    return FROM_RGB(r,g,b);
}

void draw_rectangle(uint64_t color, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            putPixel(color, x + j, y + i);
        }
    }
    return;
}

void draw_circle(uint64_t hexColor, uint32_t b, uint32_t radius, uint32_t x, uint32_t y);

void drawSquare(uint64_t hexColor, uint32_t side_length, uint32_t x, uint32_t y){
	draw_rectangle(hexColor, side_length, side_length, x, y);
}

void fill_section(uint64_t hexColor, int startY, int endY) {
    for (int y = startY; y < endY; y++) {
        for (int x = 0; x < VBE_mode_info->width; x++) {
            putPixel(hexColor, x, y);
        }
    }
}

void boke() {
    int height = VBE_mode_info->height / 3 ;

    fill_section(BLUE, 0, height);


    fill_section(YELLOW, height, height * 2);

 
    fill_section(BLUE, height * 2, VBE_mode_info->height);
}
// Funcion que limpia la pantalla
void clear(){
	clear_color(bg_color);
	return;
}
// Funcion que limpia la pantalla con un color
void clear_color(uint64_t hexColor){
	for (int x = 0; x < VBE_mode_info->width; x++){
		for (int y = 0; y < VBE_mode_info-> height; y++){
			putPixel(hexColor,x,y);
		}
	}
    cursorX=0;
    cursorY=0;
	return;
}


// Funcion que pinta la pantalla con un color
void paint_screen(uint64_t hexColor){
    bg_color = hexColor;
	for (int x = 0; x < VBE_mode_info->width; x++){
		for (int y = 0; y < VBE_mode_info-> height; y++){
            if (getPixelHex(x,y) != font_color && getPixelHex(x,y) != RED)
			    putPixel(hexColor,x,y);
		}
	}
	return;
}

void change_size(uint32_t new_size){
    size=new_size;
}

uint32_t* get_cursor_x() {
    return &cursorX;
}

uint32_t* get_cursor_y() {
    return &cursorY;
}

uint32_t* getSize() {
    return &size;
}

unsigned int get_max_height() {
	return SCREEN_HEIGHT;
}

unsigned int get_max_width() {
	return SCREEN_WIDTH;
}


void put_square(uint32_t x, uint32_t y, uint32_t size, uint64_t hexColor) {
    for (uint32_t i = 0; i < size; i++) {
        for (uint32_t j = 0; j < size; j++) {
            putPixel(hexColor, x + i, y + j);
        }
    }
}

void backspace(){
     if (cursorX > 0) {
            cursorX -= size*8;
        } else if (cursorY > 0 && cursorX == 0) { // El cursor está al principio de una línea
            // Borra el último carácter de la línea anterior
            cursorY -= size*16;
            cursorX = get_max_width() - size*8; // Establece el cursorX al último carácter de la línea anterior
            
        }
        // uint64_t hex_backspace = bg_color;
		draw_rectangle(bg_color, cursorX, cursorY, size*8, size*16);
}

void newline(){
    cursorX = 0;
    cursorY += (size*16);
    checkLineUp();
    return;
}

void tab(){ //descontinuado por mal funcionamiento
    int tabWidth = 5;
        int spaces = tabWidth - (cursorX / (size*8)) % tabWidth;

        for (int i = 0; i < spaces; i++) {
            drawChar(WHITE, ' ');
            cursorX += size*8;
        }
        return;
}

void draw_number_color(int value, uint64_t hexColor){
    char buffer[256] = {0};
    uintToBase(value, buffer, 10);
    draw_word_color(hexColor, buffer);
}


void drawNumber(int value){
    draw_number_color(value, WHITE);
}

void drawRegisters(int value){
        char buffer[256] = {0};
        uintToBase(value, buffer, 16);
        draw_word_color(WHITE, buffer);
     newline();
}

void draw_number_colorAt(uint64_t hexColor, int value,  uint32_t x, uint32_t y){
    char buffer[256] = {0};
    uintToBase(value, buffer, 10);
    draw_word_colorAt(hexColor, buffer, x, y);
}

void moveOneLineUp() {
    char* dst = (char*)(uintptr_t)(VBE_mode_info->framebuffer); // Puntero al framebuffer
    char* src = dst + VBE_mode_info->pitch * size * 16; // Puntero a la línea de origen
    uint64_t numBytes = VBE_mode_info->pitch * (VBE_mode_info->height - size * 16); // Cantidad de bytes a copiar

    memcpy(dst, src, numBytes); // Copia los bytes desde la línea de origen a la línea de destino
    // memset((void*)(uintptr_t)(VBE_mode_info->framebuffer + VBE_mode_info->pitch * (VBE_mode_info->height - size * 16)), 0, VBE_mode_info->pitch * size * 16); // Rellena con ceros la parte de la línea de destino copiada
    draw_rectangle(bg_color, 0, VBE_mode_info->height - size*16, 1024, size*16 );
    cursorY -= (size * 16); // Actualiza la posición del cursor en el eje Y
}

int isWhiteSpace(char c){
 return  c==' ' || c=='\v' || c=='\r' || c=='\a' || c=='\f';
}

void character(uint64_t hexColor, char c){
        if (c == '\b') { // backspace
            backspace();
            return;
        }else if (isWhiteSpace(c)) { // Tab=space
            cursorX += size*8; //tab = 1 espacio
            return;
        } else if (c == '\n') { // Salto de línea
            newline();
            return;
        }
        else if(c == '\t'){
            tab();
            return;
        }
        //Carácter
        checkLineUp();
        drawChar(hexColor, c);
        cursorX += size*8;
        return;
}

void checkLineUp(){
    if (cursorX >= get_max_width()) {
        cursorX = 0;
        cursorY += size*16;
    }
    if (cursorY >= get_max_height()){ 
        cursorX = 0;
        moveOneLineUp();
    }
}

void character_at(uint64_t hexColor, char c, uint32_t x, uint32_t y){
    uint32_t auxX = cursorX;
    uint32_t auxY = cursorY;
    cursorX = x;
    cursorY = y;
    character(hexColor, c);
    cursorX = auxX;
    cursorY = auxY;
}

void draw_word_color(uint64_t hexColor, char* word) {
    for (int i=0; word[i] != 0; i++) {
        character(hexColor, word[i]);
    }
}

void draw_word_colorAt(uint64_t hexColor, char* word, uint32_t x, uint32_t y){
    uint32_t auxX = cursorX;
    uint32_t auxY = cursorY;
    cursorX = x;
    cursorY = y;
    draw_word_color(hexColor, word);
    cursorX = auxX;
    cursorY = auxY;
}

void draw_word(char* word) {
    draw_word_color(WHITE, word);
}

void draw_word_colorLen(uint64_t color, char * buff, int len){
 for (int i=0; buff[i] != 0 && i<len; i++) {
        character(color, buff[i]);
    }
}
void draw_wordLen(char * buff, int len){
    draw_word_colorLen(WHITE, buff, len);
}

void drawChar(uint64_t hexColor, char character) {
    int a = cursorX;  // Posición horizontal actual
    int x = a;  // Posición horizontal actual
    int y = cursorY;  // Posición vertical actual
    int start = character - 33;  // Índice de inicio en el vector de fuentes
    if(start>92*32 || start<0 ){
        start = 3;
    }
    // Si el carácter es minúscula, ajusta el índice de inicio
    if (is_minusc(character))
        start = character - 'a';
    
    if (character == ' ') {
        return;  // Si es un espacio, no se dibuja nada
    }

    for (int i = 0; i < 32; i++) {
        if (i % 2 == 0 && i != 0) {
            y += size;  // Salto a la siguiente fila de píxeles
            a = x;  // Reinicia la posición horizontal al inicio
        }
        
        // Comprueba el bit correspondiente en la fuente para determinar si se debe dibujar un píxel
        font[i + (start * 32)] & (char)0x01 ? put_square(a, y, size, hexColor) : 0;
        
        a += size;  // Avanza a la siguiente posición horizontal
        
        uint8_t aux = 0x02;
        for (int j = 0; j < 8; j++) {
            // Comprueba cada bit de la fuente y dibuja un píxel si está activo
            ((uint8_t)font[i + (start * 32)] & (uint8_t)aux) >> j ? put_square(a, y, size, hexColor) : 0;
            a += size;  // Avanza a la siguiente posición horizontal
            aux <<= 1;  // Desplaza el bit auxiliar hacia la izquierda
        }
    }
}

void invalid_fd(){
    draw_word_color(RED, "Invalid file descriptor");
    return;
}

void draw_ball(uint64_t color, int size, int x, int y) {
    int radius = size / 2;
    int centerX = x + radius;
    int centerY = y + radius;
    int squaredRadius = radius * radius;

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if ((x * x) + (y * y) <= squaredRadius) {
                putPixel(color, centerX + x, centerY + y);
            }
        }
    }
}

void draw_word_padded(uint64_t hexColor, char* word, int width) {
    int wordLength = str_len(word); 
    draw_word_color(hexColor, word); 
    for (int i = wordLength; i < width; i++) {
        character(hexColor, ' '); 
    }
}

void draw_number_padded(uint64_t hexColor, int number, int width) {
    char buffer[256];
    uintToBase(number, buffer, 10); 
    draw_word_padded(hexColor, buffer, width);
}

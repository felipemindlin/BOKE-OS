#include "include/videoDriver.h"
#include "include/font.h"
#include "../include/naiveConsole.h"
#include <naiveConsole.h>
#include <lib.h>

unsigned int SCREEN_WIDTH = 1024;
unsigned int SCREEN_HEIGHT = 768;
unsigned int BPP = 3;

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

void setFontSize(uint32_t new_size){
    size = new_size;
}
uint32_t getFontSize(){
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

void drawRectangle(uint64_t color, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            putPixel(color, x + j, y + i);
        }
    }
    return;
}

void drawCircle(uint64_t hexColor, uint32_t b, uint32_t radius, uint32_t x, uint32_t y);

void drawSquare(uint64_t hexColor, uint32_t side_length, uint32_t x, uint32_t y){
	drawRectangle(hexColor, side_length, side_length, x, y);
}

void fillSection(uint64_t hexColor, int startY, int endY) {
    for (int y = startY; y < endY; y++) {
        for (int x = 0; x < VBE_mode_info->width; x++) {
            putPixel(hexColor, x, y);
        }
    }
}

void boke() {
    int height = VBE_mode_info->height / 3 ;

    fillSection(BLUE, 0, height);


    fillSection(YELLOW, height, height * 2);

 
    fillSection(BLUE, height * 2, VBE_mode_info->height);
}
// Funcion que limpia la pantalla
void clear(){
	clearColor(bg_color);
	return;
}
// Funcion que limpia la pantalla con un color
void clearColor(uint64_t hexColor){
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
void paintScreen(uint64_t hexColor){
    bg_color = hexColor;
	for (int x = 0; x < VBE_mode_info->width; x++){
		for (int y = 0; y < VBE_mode_info-> height; y++){
            if (getPixelHex(x,y) != font_color && getPixelHex(x,y) != RED)
			    putPixel(hexColor,x,y);
		}
	}
	return;
}

void changeSize(uint32_t new_size){
    size=new_size;
}

uint32_t* getCursorX() {
    return &cursorX;
}

uint32_t* getCursorY() {
    return &cursorY;
}

uint32_t* getSize() {
    return &size;
}

unsigned int getMaxHeight() {
	return SCREEN_HEIGHT;
}

unsigned int getMaxWidth() {
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
            cursorX = getMaxWidth() - size*8; // Establece el cursorX al último carácter de la línea anterior
            
        }
        // uint64_t hex_backspace = bg_color;
		drawRectangle(bg_color, cursorX, cursorY, size*8, size*16);
}

void newline(){
    cursorX = 0;
    cursorY += (size*16);
    return;
}

void tab(){
    int tabWidth = 15;
        int spaces = tabWidth - (cursorX / size*8) % tabWidth;

        for (int i = 0; i < spaces; i++) {
            drawChar(WHITE, ' ');
            cursorX += size*8;
        }
        return;
}

void drawNumberColor(int value, uint64_t hexColor){
    char buffer[256] = {0};
    uintToBase(value, buffer, 10);
    drawWordColor(hexColor, buffer);
}


void drawNumber(int value){
    drawNumberColor(value, WHITE);
}

void drawRegisters(int value){
        char buffer[256] = {0};
        uintToBase(value, buffer, 16);
        drawWordColor(WHITE, buffer);
     newline();
}

void drawNumberColorAt(uint64_t hexColor, int value,  uint32_t x, uint32_t y){
    char buffer[256] = {0};
    uintToBase(value, buffer, 10);
    drawWordColorAt(hexColor, buffer, x, y);
}

void moveOneLineUp() {
    char* dst = (char*)(uintptr_t)(VBE_mode_info->framebuffer); // Puntero al framebuffer
    char* src = dst + VBE_mode_info->pitch * size * 16; // Puntero a la línea de origen
    uint64_t numBytes = VBE_mode_info->pitch * (VBE_mode_info->height - size * 16); // Cantidad de bytes a copiar

    memcpy(dst, src, numBytes); // Copia los bytes desde la línea de origen a la línea de destino
    // memset((void*)(uintptr_t)(VBE_mode_info->framebuffer + VBE_mode_info->pitch * (VBE_mode_info->height - size * 16)), 0, VBE_mode_info->pitch * size * 16); // Rellena con ceros la parte de la línea de destino copiada
    drawRectangle(bg_color, 0, VBE_mode_info->height - size*16, 1024, size*16 );
    cursorY -= (size * 16); // Actualiza la posición del cursor en el eje Y
}


void character(uint64_t hexColor, char c){
        if (c == '\b') { // backspace
            backspace();
            return;
        }
        if (c == '\t') { // Tab
            tab();
            return;
        }
        if (c == '\n') { // Salto de línea
            newline();
            return;
        }
        if (c == ' '){
            cursorX += size*8;
            return;
        }
        //Carácter
        if (cursorX >= getMaxWidth()) {
            cursorX = 0;
            cursorY += size*16;
        }
        if (cursorY >= getMaxHeight()){ 
            cursorX = 0;
            moveOneLineUp();
        }
        drawChar(hexColor, c);
        cursorX += size*8;
        return;
}

void characterAt(uint64_t hexColor, char c, uint32_t x, uint32_t y){
    uint32_t auxX = cursorX;
    uint32_t auxY = cursorY;
    cursorX = x;
    cursorY = y;
    character(hexColor, c);
    cursorX = auxX;
    cursorY = auxY;
}

void drawWordColor(uint64_t hexColor, char* word) {
    for (int i=0; word[i] != 0; i++) {
        character(hexColor, word[i]);
    }
}

void drawWordColorAt(uint64_t hexColor, char* word, uint32_t x, uint32_t y){
    uint32_t auxX = cursorX;
    uint32_t auxY = cursorY;
    cursorX = x;
    cursorY = y;
    drawWordColor(hexColor, word);
    cursorX = auxX;
    cursorY = auxY;
}

void drawWord(char* word) {
    drawWordColor(WHITE, word);
}

void drawWordColorLen(uint64_t color, char * buff, int len){
 for (int i=0; buff[i] != 0 && i<len; i++) {
        character(color, buff[i]);
    }
}
void drawWordLen(char * buff, int len){
    drawWordColorLen(WHITE, buff, len);
}

void drawChar(uint64_t hexColor, char character) {
    int a = cursorX;  // Posición horizontal actual
    int x = a;  // Posición horizontal actual
    int y = cursorY;  // Posición vertical actual
    int start = character - 33;  // Índice de inicio en el vector de fuentes
    
    // Si el carácter es minúscula, ajusta el índice de inicio
    if (isMinusc(character))
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

void invalidFd(){
    drawWordColor(RED, "Invalid file descriptor");
    return;
}

void drawBall(uint64_t color, int size, int x, int y) {
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


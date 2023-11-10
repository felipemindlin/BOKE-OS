#include "keyboard_buffer.h"
#include "defs.h"
static struct keyboard_buffer buff = { 0, 0, {0} };

static buff_ptr ptr = &buff;

int getBufferPosition(){
    return ptr->bufferPos;
}
int setPos(int new_pos){
    ptr->bufferPos = new_pos;
    return new_pos;
}

uint16_t * getBufferAddress(){
    return ptr->buff;
}

uint16_t getCharAt(int pos){
    return ptr->buff[pos];
}


void consumeBuffAt(int pos){
    ptr->buff[pos] = 0;
}

void clear_buffer(){
    uint16_t * buffer = ptr->buff;
    for (int i = 0; i < BUFF_SIZE; i++){
        buffer[i] = '\0';
    }
}

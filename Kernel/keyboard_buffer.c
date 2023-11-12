#include "keyboard_buffer.h"
#include "defs.h"
static struct keyboard_buffer buff = { 0, 0, {0} };

static buff_ptr ptr = &buff;

int get_buffer_position(){
    return ptr->bufferPos;
}
int set_pos(int new_pos){
    ptr->bufferPos = new_pos;
    return new_pos;
}

uint16_t * get_buffer_address(){
    return ptr->buff;
}

uint16_t get_char_at(int pos){
    return ptr->buff[pos];
}


void consume_buff_at(int pos){
    ptr->buff[pos] = 0;
}

void clear_buffer(){
    uint16_t * buffer = ptr->buff;
    for (int i = 0; i < BUFF_SIZE; i++){
        buffer[i] = '\0';
    }
}

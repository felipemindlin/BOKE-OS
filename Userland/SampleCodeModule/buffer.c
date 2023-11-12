#include "include/buffer.h"
void clear_buffer(char * buff){     for (int i = 0; i<BUFFER_SIZE;i++){
        buff[i]=0;
    }
}
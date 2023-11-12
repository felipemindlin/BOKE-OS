#include "include/buffer.h"
void clear_buffer(char * buff){ //limpia el buffer
    for (int i = 0; i<BUFFER_SIZE;i++){
        buff[i]=0;
    }
}
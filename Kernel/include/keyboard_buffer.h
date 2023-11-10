

#include <stdint.h>
#define BUFF_SIZE 4096
typedef struct keyboard_buffer* buff_ptr; 

struct keyboard_buffer { 
    int bufferPos;
    int bufferLen;
    uint16_t buff[BUFF_SIZE];
};

int getBufferPosition();
int setPos(int new_len);
int getBufferLen();
int setLen(int new_len);
uint16_t * getBufferAddress();
uint16_t getCharAt(int pos);
void cleanBuffer();
void consumeBuffAt(int pos);
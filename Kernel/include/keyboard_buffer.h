

#include <stdint.h>
#define BUFF_SIZE 4096
typedef struct keyboard_buffer* buff_ptr; 

struct keyboard_buffer { 
    int bufferPos;
    int bufferLen;
    uint16_t buff[BUFF_SIZE];
};

int get_buffer_position();
int set_pos(int new_len);
int get_buffer_len();
int set_len(int new_len);
uint16_t * get_buffer_address();
uint16_t get_char_at(int pos);
void clear_buffer();
void consume_buff_at(int pos);
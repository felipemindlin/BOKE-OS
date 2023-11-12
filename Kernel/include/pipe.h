#include <stdint.h>

#define PIPE_BUFFER_SIZE 1024
#define MAX_PIPES 10
#define _EOF_ 1
typedef struct pipe{
    int name;
    char * read_sem_id;
    char * write_sem_id;
    unsigned int write_pos;
    unsigned int read_pos;
    char *pipe_buffer;
    uint8_t eof;
    int left_to_read;
} pipe;

void init_pipes();

int create_pipe(int name);

int create_pipe_anonymous();

int pipe_write(int id, const char *message, unsigned int size);

int pipe_read(int id, char * dest, unsigned int size);

void close_pipe(int id);

int get_pipe_id(int name);

void send_eof_to_foreground();

pipe get_pipe(int id);

int eof_sent(int id);
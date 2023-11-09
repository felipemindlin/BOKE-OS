#include <stdint.h>

#define PIPE_BUFFER_SIZE 1024
#define MAX_PIPES 10
#define EOF -1
typedef struct pipe{
    int name;
    int readSemId;
    int writeSemId;
    unsigned int writePos;
    unsigned int readPos;
    char *pipeBuffer;
    uint8_t eof;
    int leftToRead;
} pipe;

void init_pipes();

int create_pipe(int name);

int create_pipe_anonymous();

int pipe_write(int id, const char *message, unsigned int size);

int pipe_read(int id, char * dest, unsigned int size);

void close_pipe(int id);

int getPipeID(int name);

void send_eof_to_foreground();

pipe getPipe(int id);
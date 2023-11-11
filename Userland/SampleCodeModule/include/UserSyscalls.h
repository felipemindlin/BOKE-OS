#include "stdint.h"
#include <stdint.h>
#include <stddef.h>
#ifndef USERSYSCALLS_H
#define USERSYSCALLS_H

#define STDIN 0
#define STDOUT 1
#define STDERR 2

void call_sys_read(char *buf, int size, int fd);
void call_sys_write(char *buf, int size, int fd);
void call_timeClock(char *buf);
void call_regState();
void call_paintScreen(uint64_t hex);
void call_drawRectangle(uint64_t color, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
void call_drawBall( uint64_t color, uint64_t x, uint64_t y, uint64_t size);
int call_getBufferPosition();
int call_getCharAt(int pos);
void call_put_square(uint32_t x, uint32_t y, uint32_t size, uint64_t hexColor);
void call_sleepms(int mseconds);
int call_ticks_elapsed();
void call_setFontSize(int size);
void call_drawWordColorAt(uint64_t hexColor, char* word, uint32_t x, uint32_t y);
void call_characterAt(uint64_t hexColor, char c, uint32_t x, uint32_t y);
void call_beep(int freq, int time);
void call_clearColor(uint64_t hexColor);
void call_ps();
void call_mem();
int call_kill(int pid);
void call_nice(int pid, int priority);
int call_block(int pid);
void call_loop();
int call_force_kill(int pid);
int  call_create_process(const char * name, uint8_t foreground, size_t heap_and_stack[2], void * entry_point, void * argv,int fd[2]);
uint64_t call_sem_open(uint64_t start_value, uint64_t id);
uint64_t call_sem_wait(uint64_t id);
uint64_t call_sem_post(uint64_t id);
void call_sem_close(uint64_t id);
void call_forceTimer();
int call_waitpid(int pid);
int get_pid();
void *user_malloc(uintptr_t bytes);
void user_free(void *mem);
int call_pipe_open(int name);
void call_pipe_close(int name);
int call_pipe_create(int name);
int call_pipe_create_anonymous();
void call_read_pipe(int id, char* dest, int len);
void call_print_word_color(uint64_t hexColor, char* word);
void call_yield();      
#endif /* USERSYSCALLS_H */
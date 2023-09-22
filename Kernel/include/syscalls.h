#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#define STDIN 0
#define STDOUT 1
#define STDERR 2

void sys_write(char *buf, int len, int filedescriptor);
void sys_read(char *buf, int len, int filedescriptor);

#endif // _KEYBOARD_H_
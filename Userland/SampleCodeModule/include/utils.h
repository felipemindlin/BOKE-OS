#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdarg.h>
#include <UserSyscalls.h>
#include <buffer.h>

#define _EOF_ 1

char get_c();
void put_c(char c);
void put_int(int num);
void put_hex_dir(uint64_t num);
void put_string(char * str);
int own_scanf(char * format, ...);
int read_int(int* d);
int read_string(char *s);
int read_hex_int(int* d);
void print(char * format, ...);
int strcmp(char * str1, char * str2);
int strcmpspace(char * str1, char * str2, uint8_t * is_fg);
int null_or_space(char c);
int null_or_newline(char c);
char * strtok(char * str, char delim);
char * strncpy(char * dest, char * src, int n);
int atoi(char * str);
int64_t satoi(char *str);
char* itoa(int num, char* str, int base);
int isspace(char c);
char * concat(char * str1, char * str2);
int str_len(const char * str);
#endif 
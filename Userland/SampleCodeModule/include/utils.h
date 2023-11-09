#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdarg.h>
#include <UserSyscalls.h>
#include <buffer.h>

#define _EOF_ 1

char getC();
void putC(char c);
void putInt(int num);
void putHexDir(uint64_t num);
void putString(char * str);
int own_scanf(char * format, ...);
int readInt(int* d);
int readString(char *s);
int readHexInt(int* d);
void print(char * format, ...);
int strcmp(char * str1, char * str2);
int strcmpspace(char * str1, char * str2);
int null_or_space(char c);
int null_or_newline(char c);
char * strtok(char * str, char delim);
char * strncpy(char * dest, char * src, int n);
int atoi(char * str);
int64_t satoi(char *str);
char* itoa(int num, char* str, int base);
#endif /* UTILS_H */
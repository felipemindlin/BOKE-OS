#include <stdio.h>
#include <stdarg.h>
#include <utils.h>
#include <UserSyscalls.h>
#include <buffer.h>


int null_or_space(char c){
    return (c == '\0' || c == ' ');
}

int null_or_newline(char c){
    return (c == '\0' || c == '\n');
}
char getC(){
    char c;
    call_sys_read(&c, 1, STDIN);
    return c;
}

void putC(char c){
    call_sys_write(&c, 1, STDOUT);
}

int own_scanf(char * format, ...){
    va_list args;
    va_start(args, format);
    int toRet = 0;

    while(*format != '\0' ){

        if(*format == '%'){
            format++; 
            switch(*format){
                case 'c': {
                    char* c = va_arg(args, char*);
                    *c = getC();
                    toRet++;
                    break;
                }
                case 'd': {
                    int* d = va_arg(args, int*);
                    toRet += readInt(d);
                    break;
                }
                case 's': {
                    char* s = va_arg(args, char*);
                    toRet += readString(s);
                    break;
                }
                case 'x': {
                    int* d = va_arg(args, int*);
                    toRet += readHexInt(d);
                    break;
                }
                default:
                break;
            }
        }
        format++;
    }
    va_end(args);

    return toRet;
}

int readInt(int* d){
    int value = 0;
    int sign = 1; //en principio positivo
    char c = getC();

    if(c == '-'){
        sign = -1;
        c = getC();
    }

    while((c != '\0') && (c >= '0' && c <= '9')){
        value = (c - '0') + value*10;
        c = getC();
    }

    *d = value * sign;
    return 1;
}

int readString(char *s){
    int i = 0;
    char c = getC();

    while(c != '\0' && c != '\n'){
        s[i++] = c;
        c = getC();
    }
    s[i] = '\0'; //null terminated
    return i;
}


int readHexInt(int* d){
    int value = 0;
    int sign = 1;
    char c = getC();

    if(c == '-'){
        sign = -1;
        c = getC();
    }

    while(c != '\0' && ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))){
        if(c >= 'A' && c <= 'F'){
            c = c - 'A' + 10;
        }else{
            c = c - '0';
        }
        value = value * 16 + c;
        c = getC();
    }

    *d = value * sign;
    return 1;
}


void print(const char * format, ...){
    va_list args;
    va_start(args, format);


    while(*format != '\0'){
        if(*format == '%'){
            format++;

            switch(*format){
                case 'c': {
                    char *c = va_arg(args, char*);
                    putC(*c);
                    break;
                }
                case 'd': {
                    int *d = va_arg(args, int*);
                    putInt(*d);
                    break;
                }
                case 's': {
                    char* s = va_arg(args, char*);
                    putString(s);
                    break;
                }
            }
        } else {
            putC(*format);
        }
        format++;
    }

    va_end(args);
    return;
}

void putString(char * str){
    while(*str != '\0'){
        putC(*str);
        str++;
    }
}

void putInt(int num) {
    if (num < 0) {
        putC('-');
        num = -num;
    }

    int divisor = 1;
    while (num / divisor >= 10) {
        divisor *= 10;
    }

    while (divisor > 0) {
        int digit = num / divisor;
        putC('0' + digit);
        num %= divisor;
        divisor /= 10;
    }
}

int strcmp(char * str1, char * str2){
	int i = 0;
	for (; str1[i] != '\0' && str2[i] != '\0'; i++){
		if (str1[i] > str2[i]){
			return 1;
		} else if (str1[i] < str2[i]){
			return -1;
		}
	}
	if (str1[i] == '\0' && str2[i] == '\0') {
        return 0;
    } else if (str1[i] == '\0') {
        return -1; 
    } else return 1;

}

// Similar a strcmp pero solo compara hasta el primer espacio o el fin de la cadena (lo que ocurra primero)
int strcmpspace(char * str1, char * str2){
    char partition[BUFFER_SIZE];
    int j = 0;
    for (; str2[j] != '\0' && str2[j] != ' ';j++){
        partition[j]=str2[j];
    }
    partition[j+1]=0;
	int i = 0;
	for (; str1[i] != '\0' && !null_or_space(partition[i]); i++){
		if (str1[i] > str2[i]){
			return 1;
		} else if (str1[i] < str2[i]){
			return -1;
		}
	}
	if (str1[i] == '\0' && null_or_space(partition[i])) {
        return 0;
    } else if (null_or_space(partition[i])) {
        return -1; 
    } else return 1;

}

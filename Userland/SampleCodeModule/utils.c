#include <utils.h>
#include <stdbool.h>

void vprint(char * format, va_list ap);

int null_or_space(char c){
    return (c == '\0' || c == ' ');
}

int null_or_newline(char c){
    return (c == '\0' || c == '\n');
}
char get_c(){
    char c;
    call_sys_read(&c, 1, STDIN);
    return c;
}

void put_c(char c){
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
                    *c = get_c();
                    toRet++;
                    break;
                }
                case 'd': {
                    int* d = va_arg(args, int*);
                    toRet += read_int(d);
                    break;
                }
                case 's': {
                    char* s = va_arg(args, char*);
                    toRet += read_string(s);
                    break;
                }
                case 'x': {
                    int* d = va_arg(args, int*);
                    toRet += read_hex_int(d);
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

int read_int(int* d){
    int value = 0;
    int sign = 1; //en principio positivo
    char c = get_c();

    if(c == '-'){
        sign = -1;
        c = get_c();
    }

    while((c != '\0') && (c >= '0' && c <= '9')){
        value = (c - '0') + value*10;
        c = get_c();
    }

    *d = value * sign;
    return 1;
}

int read_string(char *s){
    int i = 0;
    char c = get_c();

    while(c != '\0' && c != '\n'){
        s[i++] = c;
        c = get_c();
    }
    s[i] = '\0'; //null terminated
    return i;
}


int read_hex_int(int* d){
    int value = 0;
    int sign = 1;
    char c = get_c();

    if(c == '-'){
        sign = -1;
        c = get_c();
    }

    while(c != '\0' && ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F'))){
        if(c >= 'A' && c <= 'F'){
            c = c - 'A' + 10;
        }else{
            c = c - '0';
        }
        value = value * 16 + c;
        c = get_c();
    }

    *d = value * sign;
    return 1;
}

void print(char * format, ...){
	va_list ap;
    va_start(ap, format);
    vprint(format, ap);
    va_end(ap);
}


void vprint(char * format, va_list ap){


    while(*format != '\0'){
        if(*format == '%'){
            format++;

            switch(*format){
                case 'c': {
                    char c = (char) va_arg(ap, int);
                    put_c(c);
                    break;
                }
                case 'd': {
                    int d = va_arg(ap, int);
                    put_int(d);
                    break;
                }
                case 's': {
                    char* s = va_arg(ap, char*);
                    put_string(s);
                    break;
                }
                case 'x': {
                    uint64_t x = (uint64_t) va_arg(ap, uint64_t);
                    put_hex_dir(x);
                    break;
                }
            }
        } else {
            put_c(*format);
        }
        format++;
    }

    return;
}

void put_string(char * str){
    while(*str != '\0'){
        put_c(*str);
        str++;
    }
}

void put_int(int num){
    if (num < 0){
        put_c('-');
        num = -num;
    }

    int divisor = 1;
    while (num / divisor >= 10){
        divisor *= 10;
    }

    while (divisor > 0){
        int digit = num / divisor;
        put_c('0' + digit);
        num %= divisor;
        divisor /= 10;
    }
}

void put_hex_dir(uint64_t num){
    char * hex = "0123456789ABCDEF";
    char toPrint[19]={0};
    toPrint[0] = '0';
    toPrint[1] = 'x';
    toPrint[18] = '\0';
    int i = 17;
    while (num > 0 && i > 1){
        toPrint[i] = hex[num%16];
        num /= 16;
        i--;
    }
    while (i > 1){
        toPrint[i] = '0';
        i--;
    }
    put_string(toPrint);
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
	if (str1[i] == '\0' && str2[i] == '\0'){
        return 0;
    } else if (str1[i] == '\0'){
        return -1; 
    } else return 1;

}

// Similar a strcmp pero solo compara hasta el primer espacio o el fin de la cadena (lo que ocurra primero)
int strcmpspace(char * str1, char * str2, uint8_t * is_fg){
    char partition[BUFFER_SIZE];
    int j=0;
    while (str2[j] != '\0' && str2[j] != ' '){
        partition[j] = str2[j];
        j++;
    }
    partition[j] = '\0';  // Null-terminate the partition string

    // Compare str1 and the partition string
    int i = 0;
    while (str1[i] != '\0' && !null_or_space(partition[i])){
        if (str1[i] > partition[i]){
            return 1;
        } else if (str1[i] < partition[i]){
            return -1;
        }
        i++;
    }

    // Check for equality
    if (str1[i] == '\0' && null_or_space(partition[i])){
        return 0;
    } else if (null_or_space(partition[i])){
        return -1;
    } else {
        return 1;
    }

}

int isspace(char c){
    return c == ' ' || c == '\n' || c == '\0';
}

int strncmp(char * str1, char * str2, int n){
    int i = 0;
    for (; str1[i] != '\0' && str2[i] != '\0' && i < n; i++){
        if (str1[i] > str2[i]){
            return 1;
        } else if (str1[i] < str2[i]){
            return -1;
        }
    }
    if (i == n){
        return 0;
    } else if (str1[i] == '\0'){
        return -1; 
    } else return 1;

}
/*
int strcmpspace(char *str1, char *str2, uint8_t *is_fg){
    // Skip leading whitespaces
    while (isspace(*str1)) str1++;
    while (isspace(*str2)) str2++;

    // Find the end of the first word in both strings
    char *end1 = str1;
    char *end2 = str2;

    while (*end1 && !isspace(*end1)) end1++;
    while (*end2 && !isspace(*end2)) end2++;

    int max_len = end1 - str1 > end2 - str2 ? end1 - str1 : end2 - str2;

    // Compare the first word
    int cmp = strncmp(end1, end2, max_len);

    // If the first word doesn't match, return the result
    if (cmp != 0){
        return cmp;
    }

    str1 = end1;

    while(*str1);
    str1--;

    *is_fg = *str1 == '&';

    return cmp;
}*/

char * strtok(char * str, char delim){
    static char * static_str;
    if (str != NULL){
        static_str = str;
    }
    if (*static_str == '\0'){
        return NULL;
    }
    char * toRet = static_str;
    while (*static_str != delim && *static_str != '\0'){
        static_str++;
    }
    if (*static_str == delim){
        *static_str = '\0';
        static_str++;
    }
    return toRet;
}

char * strncpy(char * dest, char * src, int n){
    int i = 0;
    for (; i < n && src[i] != '\0'; i++){
        dest[i] = src[i];
    }
    for (; i < n; i++){
        dest[i] = '\0';
    }
    return dest;
}


int atoi(char * str){
    int value = 0;
    int sign = 1; //en principio positivo
    int i = 0;
    if(str[0] == '-'){
        sign = -1;
        i++;
    }
    for (; str[i] != '\0'; i++){
        value = (str[i] - '0') + value*10;
    }
    return value * sign;
}


int64_t satoi(char *str){
  uint64_t i = 0;
  int64_t res = 0;
  int8_t sign = 1;

  if (!str)
    return 0;

  if (str[i] == '-'){
    i++;
    sign = -1;
  }

  for (; str[i] != '\0'; ++i){
    if (str[i] < '0' || str[i] > '9')
      return 0;
    res = res * 10 + str[i] - '0';
  }

  return res * sign;
}



void reverse(char str[], int length){
    int start = 0;
    int end = length - 1;
    while (start < end){
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

char* itoa(int num, char* str, int base){
    int i = 0;
    bool isNegative = false;

    // Handle 0 explicitly, otherwise empty string is printed
    if (num == 0){
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    // Handle negative numbers if base is 10
    if (num < 0 && base == 10){
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0){
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // Add negative sign for negative numbers
    if (isNegative){
        str[i++] = '-';
    }

    str[i] = '\0'; // Null-terminate the string

    // Reverse the string
    reverse(str, i);

    return str;
}
int str_len(const char * str){
    int i = 0;
    while (str[i] != '\0'){
        i++;
    }
    return i;
}


char * concat(char * str1, char * str2){
    int len1 = str_len(str1);
    int len2 = str_len(str2);
    for(int i = 0; i < len2; i++){
        str1[len1 + i] = str2[i];
    }
    return str1;
}
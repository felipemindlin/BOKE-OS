#include <stdint.h>
#include "include/defs.h"
#include "include/lib.h"
#include "include/interrupts.h"


void * memset(void * destination, int32_t c, uint64_t length){
	uint8_t chr = (uint8_t)c;
	char * dst = (char*)destination;

	while(length--)
		dst[length] = chr;

	return destination;
}

void * memcpy(void * destination, const void * source, uint64_t length){
		uint64_t i;

	if ((uint64_t)destination % sizeof(uint32_t) == 0 &&
		(uint64_t)source % sizeof(uint32_t) == 0 &&
		length % sizeof(uint32_t) == 0)
	{
		uint32_t *d = (uint32_t *) destination;
		const uint32_t *s = (const uint32_t *)source;

		for (i = 0; i < length / sizeof(uint32_t); i++)
			d[i] = s[i];
	}
	else
	{
		uint8_t * d = (uint8_t*)destination;
		const uint8_t * s = (const uint8_t*)source;

		for (i = 0; i < length; i++)
			d[i] = s[i];
	}

	return destination;
}

int str_len(const char * str){
	int len = 0;
	while (str[len])
		len++;
	return len;
}

int strcmp(const char * str1, const char * str2){
	int i = 0;
	while (str1[i] && str2[i] && str1[i] == str2[i])
		i++;
	return str1[i] - str2[i];
}

int str_cpy(char * dst, const char * src){
	int i = 0;
	while (src[i])
	{
		dst[i] = src[i];
		i++;
	}
	dst[i] = 0;
	return i;
}

void sleep(uint32_t seconds){
	uint32_t startTime = seconds_elapsed();
	while (seconds > seconds_elapsed() - startTime) _hlt();
};

void sleepms(int mseconds){
	int startTime = ticks_elapsed();
	while (mseconds > ticks_elapsed()*18 - startTime*18)_hlt();
};

void nanoms(int nseconds){
	int startTime = ticks_elapsed();
	while (nseconds > ticks_elapsed()*18000 - startTime*18000)_hlt();
};
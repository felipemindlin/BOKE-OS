#include <naiveConsole.h>


static char buffer[64] = { '0' };
static uint8_t * const video = (uint8_t*)0xB8000;
static uint8_t * currentVideo = (uint8_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25 ;
#define deafultcolor 0x0f
#define defaultback 0x00

//borra el ultimo caracter
void ncBackspace(){
	if(*currentVideo>=0xB8002){ // si no es el primer caracter
		currentVideo -= 2;
		*currentVideo = ' ';
	}
}

//imprime un string
void nc_print(const char * string){
	int i;

	for (i = 0; string[i] != 0; i++)
		nc_print_char(string[i]);
}
//imprime un string con un color de letra y de fondo
void nc_printColor(const char * string, char color, char back){
	int i;
	for (i = 0; string[i] != 0; i++)
		nc_print_char_color(string[i], color, back);

}

//imprime un caracter con un color de letra y de fondo
void nc_print_char_color(char character, char color, char back){
	
	*currentVideo = character;
	char font = color | back;   
	currentVideo++;
	*currentVideo = font;  
	currentVideo++;
	

}

void nc_print_char(char character){
	
	nc_print_char_color(character, deafultcolor, defaultback);
}

void ncNewline(){
	do
	{
		nc_print_char(' ');
	}
	while((uint64_t)(currentVideo - video) % (width * 2) != 0);
}

void nc_printDec(uint64_t value){
	nc_print_base(value, 10);
}

void nc_print_hex(uint64_t value){
	nc_print_base(value, 16);
}

void nc_printBin(uint64_t value){
	nc_print_base(value, 2);
}

void nc_print_base(uint64_t value, uint32_t base){
    uintToBase(value, buffer, base);
    nc_print(buffer);
}

void ncClear(){
	int i;

	for (i = 0; i < height * width; i++)
		video[i * 2] = ' ';
	currentVideo = video;
}

//convierte un entero a una base dada y lo guarda en un buffer
 uint32_t uintToBase(uint64_t value, char * buffer, uint32_t base){
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

	//Calculate characters for each digit
	do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

	// Terminate string in buffer.
	*p = 0;

	//Reverse string in buffer.
	p1 = buffer;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}

	return digits;
}

int get_hours();
int get_minutes();
int get_seconds();
int get_weekDay();
int get_monthDay();
int get_month();
int get_year();

void convertToGMTMinus3(int *hours, int *days, int *month, int *year){
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int monthDays = daysInMonth[(*month - 1) % 12];
	
    if ( *year % 4 == 0 && (*year % 100 != 0 || *year % 400 == 0)){
        daysInMonth[1] = 29;
    }
    *hours -= 3;
    if (*hours < 0){
        *hours += 24;
        *days -= 1;
        if (*days < 1){
            *month -= 1;
			monthDays=daysInMonth[(*month - 1) % 12];
            if (*month < 1){
                *month = 12;
                *year -= 1;
            }
            *days = monthDays;
        }
    }
}

 
char * TimeClock(char * buffer){
	char * days[]={"sun", "lun", "mar", "mie", "jue", "vie", "sab"};
	int  hours = get_hours();
	int  minutes = get_minutes();
	int  seconds = get_seconds();
	int  weekday = get_weekDay();
	int  monthDay = get_monthDay();
	int  month = get_month();
	int  year = get_year();
    convertToGMTMinus3(&hours, &monthDay, &month, &year);
	int digits = uintToBase(hours, buffer, 10);
	buffer[digits++] = ':';
	digits += uintToBase(minutes, buffer+digits, 10);
	buffer[digits++] = ':';
	digits += uintToBase(seconds, buffer+digits, 10);
	buffer[digits++] =' ';
	for(int i = 0; i < 3; i++)
		buffer[digits++] = days[weekday-1][i];
	buffer[digits++] =' ';
	digits += uintToBase(monthDay, buffer+digits, 10);
	buffer[digits++] ='/';
	digits += uintToBase(month, buffer+digits, 10);
	buffer[digits++] ='/';
	digits += uintToBase(year, buffer+digits, 10);
	buffer[digits++] =' ';
	buffer[digits] = 0;
	return buffer;
}

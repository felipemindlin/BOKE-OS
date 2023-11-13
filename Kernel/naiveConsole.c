#include <naiveConsole.h>


static char buffer[64] = { '0' };
static uint8_t * const video = (uint8_t*)0xB8000;
static uint8_t * current_video = (uint8_t*)0xB8000;
static const uint32_t width = 80;
static const uint32_t height = 25 ;
#define deafultcolor 0x0f
#define defaultback 0x00

void nc_backspace(){
	if(*current_video>=0xB8002){ 		current_video -= 2;
		*current_video = ' ';
	}
}

void nc_print(const char * string){
	int i;

	for (i = 0; string[i] != 0; i++)
		nc_print_char(string[i]);
}
void nc_printColor(const char * string, char color, char back){
	int i;
	for (i = 0; string[i] != 0; i++)
		nc_print_char_color(string[i], color, back);

}

void nc_print_char_color(char character, char color, char back){
	
	*current_video = character;
	char font = color | back;   
	current_video++;
	*current_video = font;  
	current_video++;
	

}

void nc_print_char(char character){
	
	nc_print_char_color(character, deafultcolor, defaultback);
}

void nc_newline(){
	do
	{
		nc_print_char(' ');
	}
	while((uint64_t)(current_video - video) % (width * 2) != 0);
}

void nc_print_dec(uint64_t value){
	nc_print_base(value, 10);
}

void nc_print_hex(uint64_t value){
	nc_print_base(value, 16);
}

void nc_print_bin(uint64_t value){
	nc_print_base(value, 2);
}

void nc_print_base(uint64_t value, uint32_t base){
    uint_to_base(value, buffer, base);
    nc_print(buffer);
}

void nc_clear(){
	int i;

	for (i = 0; i < height * width; i++)
		video[i * 2] = ' ';
	current_video = video;
}

 uint32_t uint_to_base(uint64_t value, char * buffer, uint32_t base){
	char *p = buffer;
	char *p1, *p2;
	uint32_t digits = 0;

		do
	{
		uint32_t remainder = value % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'A' - 10;
		digits++;
	}
	while (value /= base);

		*p = 0;

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
int get_week_day();
int get_month_day();
int get_month();
int get_year();

void convert_to_gtm_minus_3(int *hours, int *days, int *month, int *year){
    int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int month_days = days_in_month[(*month - 1) % 12];
	
    if ( *year % 4 == 0 && (*year % 100 != 0 || *year % 400 == 0)){
        days_in_month[1] = 29;
    }
    *hours -= 3;
    if (*hours < 0){
        *hours += 24;
        *days -= 1;
        if (*days < 1){
            *month -= 1;
			month_days=days_in_month[(*month - 1) % 12];
            if (*month < 1){
                *month = 12;
                *year -= 1;
            }
            *days = month_days;
        }
    }
}

 
char * Time_clock(char * buffer){
	char * days[]={"sun", "lun", "mar", "mie", "jue", "vie", "sab"};
	int  hours = get_hours();
	int  minutes = get_minutes();
	int  seconds = get_seconds();
	int  weekday = get_week_day();
	int  monthDay = get_month_day();
	int  month = get_month();
	int  year = get_year();
    convert_to_gtm_minus_3(&hours, &monthDay, &month, &year);
	int digits = uint_to_base(hours, buffer, 10);
	buffer[digits++] = ':';
	digits += uint_to_base(minutes, buffer+digits, 10);
	buffer[digits++] = ':';
	digits += uint_to_base(seconds, buffer+digits, 10);
	buffer[digits++] =' ';
	for(int i = 0; i < 3; i++)
		buffer[digits++] = days[weekday-1][i];
	buffer[digits++] =' ';
	digits += uint_to_base(monthDay, buffer+digits, 10);
	buffer[digits++] ='/';
	digits += uint_to_base(month, buffer+digits, 10);
	buffer[digits++] ='/';
	digits += uint_to_base(year, buffer+digits, 10);
	buffer[digits++] =' ';
	buffer[digits] = 0;
	return buffer;
}

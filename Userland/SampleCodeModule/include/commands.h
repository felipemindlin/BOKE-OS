#ifndef __COMMANDS_H__
#define __COMMANDS_H__

#include "utils.h"
#include "UserSyscalls.h"

#define COMMAND_LEN 20
#define COLOR_LEN 5
#define MAX_COMMAND_LENGTH 256
enum Commands {
    HELP,
    TIME,
    REG_STATE,
    PONG,
    SETCOLOR,
    DIV0,
    INVALOP,
    BOKE,
    PS,
    MEM,
    KILL,
    NICE,
    BLOCK,
    CAT,
    WC,
    PHYLO,
    FILTER,
    LOOP,
    TESTS,
    CLEAR,
};
enum Colors {
    GREEN_,
    BLUE_,
    BLACK_,
    YELLOW_,
    ORANGE_,
};



void __seek_command__(char * command); void __call_command__(int i, char * command, uint8_t is_fg, char * argv[], int fd[2]); void help(); void time(); void setbg(char * command); uint64_t setbg_enum(int i); 
void find_color(char * color); void call_boke(); 
void Div0(); void invalid_op(); void call_ps();
void parse_command(char *command, char *parsed_command, int *pid, int *priority);
void invalid_pid();
void cat();
void wc();
int is_vowel(char c);
void filter();
void write();
void loop();
void * get_phylo();
#endif 
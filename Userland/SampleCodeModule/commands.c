#include "commands.h"
#include "UserSyscalls.h"
#include "stdint.h"
#include "utils.h"
#include "pong.h"
#include "funcAsm.h"
#include "colors.h"
#include "shell.h"
#include <test_util.h> 
uint64_t color = BLACK;
static char command_list[COMMAND_LEN][10] = {"HELP", "TIME", "REGSTATE","PONG", "SETCOLOR","DIV0", "INVALOP", "BOKE","PS", "MEM", "KILL", "NICE", "BLOCK", "CAT", "WC", "PHYLO","FILTER", "LOOP","TESTS", "CLEAR"};
char *test_args[] = {"3", "1"}; // Test with 10 iterations and semaphores enabled
static char command_descriptions[COMMAND_LEN][300] = {
    "Display the list of available commands",
    "Display the current system time",
    "Display the register state",
    "Play the Pong game",
    "Set the background color (usage: SETCOLOR <color>, available colors: GREEN, BLUE, BLACK, YELLOW, ORANGE)",
    "Trigger a division by zero exception",
    "Trigger an invalid operation exception",
    "Draw the Club Atletico Boca Juniors flag",
    "Display information about running processes",
    "Display memory information",
    "Terminate a process by PID (usage: KILL <pid>)",
    "Change the priority of a process (usage: NICE <pid> <priority>)",
    "Block a process by PID (usage: BLOCK <pid>)",
    "Display the contents of Stdin",
    "Count the number of lines in the input",
    "Run the dining philosophers solution",
    "Filter out vowels from the input",
    "Run an infinite loop process",
    "Run system tests (opens a menu with the following options:\n\t\t- Test memory manager\n\t\t- Test priority\n\t\t- Test processes\n\t\t- Test synchronization\n\t\t- Test without synchronization\n",
    "Clears the screen"
};

//busca el comando en la lista de comandos y llama a la funcion correspondiente
void __seek_command__(char * command){
    for (int i = 0; i < COMMAND_LEN; i++){
        if (strcmpspace(command_list[i],command) == 0){
                __call_command__(i, command);
                return;
        }
    }
    __call_command__(-1, command);
}


void parse_command(char *command, char *parsed_command, int *pid, int *priority) {
    char cpid[5]={0};
    char cprio[5]={0};
    *pid=-1;
    *priority=-1;
    int k=0;
    for(int i=0; command[i]; i++) {
        if( *pid==-1 && command[i]==' '){
            for(int j=i+1; *pid==-1 && command[j] && command[j]!=' '; j++){
                cpid[k++]=command[j];
            }
            if(*pid==-1){
                cpid[k]='\0';
                *pid=atoi(cpid);
            }
        }
        else if( *priority==-1 && command[i]==' '){
            k=0;
            for(int j=i+1; *priority==-1 && command[j] && command[j]!=' '; j++){
                cprio[k++]=command[j];
            }
            if(*priority==-1){
                cprio[k]='\0';
                *priority=atoi(cprio);
            }
        }
    }
}

void invalid_pid(){
    call_sys_write("ERROR - PID invalido",20,2);
    putC('\n');
}
int loppPid=0;
int phylo();
void __call_command__(int i, char * command){
    char parsed_command[MAX_COMMAND_LENGTH];
    int pid;
    int priority;
    parse_command(command, parsed_command, &pid, &priority);
    switch (i)
    {
    case HELP:
        help();
        return;
    case TIME:
        time();
        return;
    case REG_STATE:
        call_regState();
        return;
    case PONG:
        Pong();
        __shell_init__();    
        return;
    case SETCOLOR:
        setbg(command);
        return;
    case DIV0:
        Div0();
        return;
    case INVALOP:
        invalidOp();
        return;
    case BOKE:
        call_boke();
        return;
    case PS:
        call_ps();
        return;
    case MEM:
        call_mem();
        return;
    case KILL:
        if(pid==-1){
            invalid_pid();
            return;
        }
        call_kill(pid);
        return;
    case NICE:
      if(pid==-1 || priority==-1){
            invalid_pid();
            return;
        }
        if(priority>4){
            priority=4;
        }
        call_nice(pid, priority);
        return;
    case BLOCK:
      if(pid==-1){
            invalid_pid();
            return;
        }
        call_block(pid);
        return;
    case CAT:
        cat();
        return;
    case WC:;
        wc();
        return;
    case PHYLO:
        phylo();;
        return;
    case FILTER:
        filter();
        return;
    case LOOP:
        loppPid = call_create_process( "loop", 4096, 4096, &loop, NULL);
        return;
    case TESTS:
        tests();
        return;
    case CLEAR:
        call_clearColor(color);
        return;
    default:
        call_sys_write("ERROR - Comando no reconocido",30,2);
        putC('\n');
        return;
    }
}
#define CYAN 0x00FFFF
//imprime la lista de comandos disponibles
void help() {
    call_sys_write("List of available commands:\n", 29, 1);
    for (int i = 0; i < COMMAND_LEN; i++) {
        call_print_word_color(CYAN, command_list[i]);
        print(":\t%s\n\n", command_descriptions[i]);
    }
}

void time(){
    char timeClock[50];
    call_timeClock(timeClock);
    char c;
    for (int i = 0; (c = timeClock[i]) != 0; i++){
        putC(c);
    }
    putC('\n');
}


void loop(){
    while(1){
        if(call_ticks_elapsed() % 18 == 0){
            print("PID:%d\n",loppPid);
        }
    }

}

uint64_t setbgEnum(int i){
    switch (i){
        case GREEN_:
            call_paintScreen(GREEN);
            return GREEN;
        case BLUE_:
            call_paintScreen(BLUE);
            return BLUE;
        case BLACK_:
            call_paintScreen(BLACK);
            return BLACK;
        case YELLOW_:
            call_paintScreen(YELLOW);
            return YELLOW;
        case ORANGE_:
            call_paintScreen(ORANGE);
            return ORANGE;
        default:
            return;
    }
}

static char hexArr[COLOR_LEN][10] = {"GREEN","BLUE","BLACK","YELLOW","ORANGE"};
void findColor(char * color){
    for (int i = 0; i < COMMAND_LEN; i++){
        if (strcmp(hexArr[i],color) == 0){
               color = setbgEnum(i);
                return;
        }
    }
    setbgEnum(-1);
}


//setea el color de fondo de la pantalla segun el comando ingresado
void setbg(char * command){
    char partition[20];
    int i=0;
    for (; !null_or_space(command[i]); i++);
    i++;
    int j = 0;
    for (; command[i] != '\0'; i++){
        partition[j++]=command[i];
    }
    partition[j]=0;
    findColor(partition);
}
void Div0(){
    int a = 0;
    int b = 1;
     a = b/a;
}
void invalidOp(){
    invalidOpAsm();
}


void cat() {
  char c;
  char comm[MAX_COMMAND_LENGTH]={0};
  int i=0;
  while ((c = getC()) != 0){
    putC(c);
    comm[i++]=c;
    if(c=='\n'){
        comm[i]='\0';
        print("%s\n", comm);
        comm[0]='\0';
        i=0;
    }
  }
}

void wc() {
  int lines = 0;
  char c;
  while ((c = getC()) != 0) {
    if (c == '\n' ) {
      lines++;
    }
    putC(c);
  }
  print("Line count: %d\n", lines); 
  putC('\n');
}


int is_vowel(char c) {
  return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
          c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');
}

void filter() {
  char c;
  while ((c = getC()) != 0) {
    if (!is_vowel(c)) {
      putC(c);
    }
  }
  putC('\n');
  return;
}
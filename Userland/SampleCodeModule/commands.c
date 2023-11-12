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
int fd[2] = {0, 0};
size_t heap_and_stack[2] = {0x0000000000001000, 0x0000000000001000};
static char command_list[COMMAND_LEN][10] = {"HELP", "TIME", "REGSTATE","PONG", "SETCOLOR","DIV0", "INVALOP", "BOKE","PS", "MEM", "KILL", "NICE", "BLOCK", "CAT", "WC", "PHYLO","FILTER", "LOOP","TESTS", "CLEAR"};
static int command_args[COMMAND_LEN] = {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 2, 1, 0, 0, 0, 0, 0, 0, 0};
#define START_SIZE 80
#define COMMAND_SIZE 10
char *test_args[] = {"3", "1"}; // Test with 10 iterations and semaphores enabled
static char command_descriptions[COMMAND_LEN][300] = {
    "Display the list of available commands",
    "Display the current system time",
    "Display the register state",
    "Play the pong game",
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
//busca el comando en la lista de comandos y llama a la funcion correspondiente
void __seek_command__(char * command){
    uint8_t is_fg1=1;
    uint8_t is_fg2=1;

    //int the_one = -1;
    char ** args = user_malloc(START_SIZE);
    for (int k = 0; k < START_SIZE/8; k++){
        args[k] = user_malloc(COMMAND_SIZE);
    }

    int default_fd[2] = {0, 0};

    int w = 0;
    int j=0;
    int end_first_com = 0;
    int arg_count = 0;

    int pipe_id = call_pipe_create_anonymous();
    int fd1[2] = {0, pipe_id};
    int fd2[2] = {pipe_id, 0};

    for (int i = 0; i < COMMAND_LEN; i++){

        args[w][j] = command[i];
        j++;

        if (command[i] == ' '){
            args[w][j-1] = '\0';
            j=0;
            w++;
        }

        if (command[i] == '\0'){
            w++;
            break;
        }

        if (command[i] == '|'){
            end_first_com = w;
            i++; 
            j=0;
            is_fg1 = 0;
        }

        if (command[i] == '&'){
            is_fg1 = 0;
            break;
        }
        
    }

    int index_second_command = 1;

    char * argv1[2];
    


    

    for (int i = 0; i < COMMAND_LEN; i++){
        
        if (strcmp(command_list[i], args[0]) == 0){
                arg_count = command_args[i];
                for (int k = 1; k <= arg_count; k++){
                    argv1[k-1] = args[k];
                }
                index_second_command += arg_count;
                if (end_first_com != 0)
                    __call_command__(i, args[0], is_fg1, argv1, fd1);
                else {
                    __call_command__(i, args[0], is_fg1, argv1, default_fd);
                    for (int k = 0; k < START_SIZE/8; k++){
                        user_free(args[k]);
                    }
                    user_free(args);
                    return;
                }

                i=COMMAND_LEN;
        }
    }

    // if (end_first_com == 0)
    //     return;
    

    char * argv2[2];

     
         
    for (int i = 0; i < COMMAND_LEN; i++){
     
        if (strcmp(command_list[i], args[index_second_command]) == 0){
                arg_count = command_args[i];
                for (int k = 1; k <= arg_count; k++){
                    argv2[k-1] = args[index_second_command+k];
                }
                __call_command__(i, args[index_second_command], is_fg2, argv2, fd2);
               
                i=COMMAND_LEN;
        }
    }
}


void invalid_pid(){
    call_sys_write("ERROR - PID invalido", 20, 2);
    put_c('\n');
}

void __call_command__(int i, char * command, uint8_t is_fg, char * argv[], int fd[2]){
    void * fun;
    //void * args=argv;
    int pid;
    int priority;
    switch (i){
    case HELP:
        fun = help;
        break;
    case TIME:
        fun = time;
        break;
    case REG_STATE:
        call_regState();
        return;
    case PONG:
        fun = get_pong();
        break;
    case SETCOLOR:
        setbg(command);
        return;
    case DIV0:
        Div0();
        return;
    case INVALOP:
        invalid_op();
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
        pid = satoi(argv[0]);
        if(pid==-1){
            invalid_pid();
            return;
        }
        call_kill(pid);
        return;
    case NICE:
        pid = satoi(argv[0]);
        priority = satoi(argv[1]);
      if(pid==-1 || priority==-1){
            invalid_pid();
            return;
        }
        return;
    case BLOCK:
        pid = satoi(argv[0]);
        if (pid == -1){
            invalid_pid();
            return;
        }
        call_block(pid);
        return;
    case CAT:
        fun = cat;
        break;
    case WC:
        fun = wc;
        break;
    case PHYLO:
        fun = get_phylo();
        break;
    case FILTER:
        fun = filter;
        break;
    case LOOP:
        fun = loop; 
        break;
    case TESTS:
        fun = get_tests();
        break;
    case CLEAR:
        call_clear_color(color);
        return;
    default:
        call_sys_write("ERROR - Comando no reconocido", 30, 2);
        put_c('\n');
        return;
    }
    call_create_process(command_list[i], is_fg, heap_and_stack, fun, argv, fd);

    return;
} 
#define CYAN 0x00FFFF
//imprime la lista de comandos disponibles
void help(){
    call_sys_write("List of available commands:\n", 29, 1);
    for (int i = 0; i < COMMAND_LEN; i++){
        call_print_word_color(CYAN, command_list[i]);
        print(":\t%s\n\n", command_descriptions[i]);
    }

}

void time(){
    char timeClock[50];
    call_timeClock(timeClock);
    char c;
    for (int i = 0; (c = timeClock[i]) != 0; i++){
        put_c(c);
    }
    put_c('\n');
}


void loop(){
    int flag=1;
    int ticks=0;
    while(1){
        ticks = call_ticks_elapsed();
        if(ticks % 18 == 0 && flag){
            print("asd");
            flag=0;
        }
        if(ticks % 18 == 1 && !flag){
            flag=1;
        }
    }

}

uint64_t setbg_enum(int i){
    switch (i){
        case GREEN_:
            call_paint_screen(GREEN);
            return GREEN;
        case BLUE_:
            call_paint_screen(BLUE);
            return BLUE;
        case BLACK_:
            call_paint_screen(BLACK);
            return BLACK;
        case YELLOW_:
            call_paint_screen(YELLOW);
            return YELLOW;
        case ORANGE_:
            call_paint_screen(ORANGE);
            return ORANGE;
        default:
            return -1;
    }
}

static char hexArr[COLOR_LEN][10] = {"GREEN","BLUE","BLACK","YELLOW","ORANGE"};
void find_color(char * color){
    for (int i = 0; i < COMMAND_LEN; i++){
        if (strcmp(hexArr[i], color) == 0){
               color = (char*) setbg_enum(i);
                return;
        }
    }
    setbg_enum(-1);
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
    find_color(partition);
}
void Div0(){
    int a = 0;
    int b = 1;
     a = b/a;
}
void invalid_op(){
    invalid_opAsm();
}


void cat(){
    char c;
    char comm[MAX_COMMAND_LENGTH]={0};
    int i=0;
    while ((c = get_c()) != _EOF_){
        put_c(c);
        comm[i++]=c;
        if(c=='\n'){
            comm[i]='\0';
            print("%s\n", comm);
            comm[0]='\0';
            i=0;
        }
    }
    put_c(c);
    return;
    comm[i++]=c;
}

void wc(){
  int lines = 1;
  char c;
  while ((c = get_c()) != _EOF_){
    if (c == '\n' ){
      lines++;
    }
    put_c(c);
  }
  put_c('\n');

  print("Line count: %d\n", lines); 
  put_c('\n');
}


int is_vowel(char c){
  return (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' ||
          c == 'A' || c == 'E' || c == 'I' || c == 'O' || c == 'U');
}

void filter(){
  char c;
  while ((c = get_c()) != _EOF_){
    if (!is_vowel(c)){
      put_c(c);
    }
  }
  put_c('\n');
  return;
}
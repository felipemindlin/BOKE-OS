#include "commands.h"
#include "UserSyscalls.h"
#include "stdint.h"
#include "utils.h"
#include "pong.h"
#include "funcAsm.h"
#include "colors.h"
#include "shell.h"
#include "buffer.h"

static char command_list[COMMAND_LEN][10] = {"HELP", "TIME", "REGSTATE","PONG", "SETCOLOR","DIV0", "INVALOP", "BOKE","PS", "MEM", "KILL", "NICE", "BLOCK", "CAT", "WC", "PHYLO","FILTER", "LOOP","SEM","WRITE"};

static char **parse_args(char *commandBuffer, int *argc, int *isBackground);
void (*command_functions[32])(int, char **) = {help,time,call_regState,Pong,setbg,Div0,invalidOp,call_boke,call_ps,call_mem,call_kill,call_nice,call_block,cat,wc,wc,filter,call_loop};

int has_a_pipe(char * command){
     for (int i = 0; i < BUFFER_SIZE; i++){
        if (command[i] == '|')
        {
            return 1;
        }
    }
    return 0;
}

//busca el comando en la lista de comandos y llama a la funcion correspondiente
void __seek_command__(char * command){
    
    if (has_a_pipe(command))
    {   char * aux = "PS | WC";
        command_listener(aux);
        return;
    }

    for (int i = 0; i < COMMAND_LEN; i++){
        if (strcmpspace(command_list[i],command) == 0){
                __call_command__(i, command);
                return;
        }
    }
    __call_command__(-1, command);
}


int strcmp1(const char* str1, const char* str2){
    while(*str1 != '\0' && *str1 == *str2){
        str1++;
        str2++;
    }
    
    return *str1 - *str2;
}

void * get_function(char * command){
    for (int i = 0; i < COMMAND_LEN; i++){
        if (strcmpspace(command_list[i],command) == 0){
                print("Funcion: %s",command_list[i]);
                return &command_functions[i];
        }
    }
}

char* find_character(const char* cadena, char caracter) {
    while (*cadena != '\0') {
        if (*cadena == caracter) {
            return (char*)cadena; 
        }
        cadena++;
    }
    return NULL; 
}

void command_listener(char * command){
    //print("Command listener\n");
    //char command_buffer[BUFFER_SIZE + 1];

    //Parse for pipes
    char * left_command = command;
    char * right_command = NULL;

    //Left command
    char ** left_args;
    int left_argc;
    int left_is_background;
    int left_program_index = -1;

    //Right command
    char ** right_args;
    int right_argc;
    int right_is_background;
    int right_program_index = -1;
    //print("Comando:%s\n",command);

    char * pipe_location = find_character(command,'|');

    //If there is a pipe, splite the command in two
    if(pipe_location != NULL){
        *pipe_location = '\0';
        right_command = pipe_location +1;

        //Parse right command
        right_args = parse_args(right_command,&right_argc,&right_is_background);
    }

    //Parse left command for arguments
    left_args = parse_args(left_command, &left_argc,&left_is_background);
/*
    print("Left command : %s\n",left_command);
    for (int i = 0; i < left_argc; i++)
    {
        print("Left args : %s\n",left_args[i]);
    }
    
    print("Right command : %s\n",right_command);
    
    for (int i = 0; i < right_argc; i++)
    {
        print("Right args : %s\n",right_args[i]);
    }
 */   

    //Check there is a command on each side of the pipe
    if(pipe_location != NULL && (right_args == NULL || left_args == NULL)){
        print("Usage: command1 | command2");
        return;
    } else if(left_args == NULL)
        return;


    int flag1,flag2 = 0;
    //Check if it there is a program
    for(int i =0; i< PROGRAMS;i++){

        //If we have a pipe and we found the program, save the index
        if(right_command != NULL && !flag1 && strcmp1(left_args[0],command_list[i])==0){
            left_program_index = i;
            flag1 = 1;
        }
        if(right_command != NULL && !flag2 && strcmp1(right_args[0],command_list[i])==0){
            right_program_index = i;
            flag2 = 1;
        }
    }

    // If we have a pipe and we found both programs, execute them
    if (right_command != NULL && left_program_index != -1 && right_program_index != -1)
    {
        //print("Creando el pipe...\n");
        //Create pipe
        int pipe_id = call_pipe_create_anonymous();
        int fd1[2] = {0,pipe_id};
        int fd2[2] = {pipe_id,0};
        int hs[2] = {1,1};

        //void * entry_point_1 = get_function(left_command);
        //void * entry_point_2 = get_function(right_command);

        int pid2 = call_create_process("wc",hs,wc,NULL,fd2);
        int pid1 = call_create_process("ps",hs,&call_ps,NULL,fd1);
        
        call_kill(pid1);
        call_kill(pid2);
    }
    
}

// Finds each argument in the commandBuffer, and puts it in a new array
static char **parse_args(char *commandBuffer, int *argc, int *isBackground) {
    print("Entro a parse args\n");
    int argsDim = 0;
    char **args = call_malloc(MAX_ARGS * sizeof(char *));

    // Parse the commandBuffer
    for (int i = 0; commandBuffer[i] != '\0';) {

        // Skip spaces
        while (commandBuffer[i] == ' ') {
            i++;
        }

        // If we reached the end of the string, break
        if (commandBuffer[i] == '\0') {
            break;
        }

        // If we found an argument, copy it to a new array
        args[argsDim] = call_malloc(MAX_ARG_SIZE * sizeof(char));
        int j = 0;
        while (commandBuffer[i] != ' ' && commandBuffer[i] != '\0') {
            args[argsDim][j] = commandBuffer[i];
            i++;
            j++;
        }
        args[argsDim][j] = '\0';
        //print("Argumento: %s\n",args[argsDim]);
        //args[argsDim] = sysrealloc(args[argsDim], (j + 1) * sizeof(char));
        argsDim++;
    }

    // If the last argument is an ampersand, remove it and set flag
    if (strcmp(args[argsDim - 1], "&") == 0) {
        call_free(args[argsDim - 1]);
        args[argsDim - 1] = NULL;
        argsDim--;
        *isBackground = TRUE;
    } else {
        *isBackground = FALSE;
    }

    // If we found no arguments, free the array
    // Else reallocate the array to the correct size
    if (argsDim == 0) {
        call_free(args);
        args = NULL;
    } else {
        //args = sysrealloc(args, argsDim * sizeof(char *));
    }

    *argc = argsDim;
    print("Salgo de print args\n");
    return args;
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
void test_sync(char *argv[]);
void __call_command__(int i, char * command){
    char parsed_command[MAX_COMMAND_LENGTH];
    int pid;
    int priority;
   // print("%s",command);
    parse_command(command, parsed_command, &pid, &priority);
    //print("pid :%d, priority: %d\n",pid, priority);

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
        //phylo();
        return;
    case FILTER:
        filter();
        return;
    case LOOP:
        call_loop();
        return;
    case SEM:
        char *test_args[] = {"3", "1"}; // Test with 10 iterations and semaphores enabled
        //call_create_process("test_sync", 0x0000000000010000, 0x0000000000010000, (void *)test_sync, test_args);
        test_sync(test_args);
        // Now run the test_sync function without semaphores
        // char *test_args_no_sem[] = {"3", "2"}; // Test with 10 iterations and semaphores disabled
        // test_sync(test_args_no_sem);
        return;
    case WRITE:
        write();
        return;
    default:
        call_sys_write("ERROR - Comando no reconocido",30,2);
        putC('\n');
        return;
    }
}

//imprime la lista de comandos disponibles
void help(){
    call_sys_write("Lista de commandos disponibles:\n", 34,1);
    for (int i = 0; i < COMMAND_LEN; i++){
        call_sys_write(command_list[i],40,1);
        call_sys_write("\n",1,1);
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


void setbgEnum(int i){
    switch (i){
        case GREEN_:; call_paintScreen(GREEN);
            return;
        case BLUE_:; call_paintScreen(BLUE);
            return;
        case BLACK_: ;call_paintScreen(BLACK);
            return;
        case YELLOW_: ;call_paintScreen(YELLOW);
            return;
        case ORANGE_:; call_paintScreen(ORANGE);
            return;
        default:
            return;
    }
}
static char hexArr[COLOR_LEN][10] = {"GREEN","BLUE","BLACK","YELLOW","ORANGE"};
void findColor(char * color){
    
   
    for (int i = 0; i < COMMAND_LEN; i++){
        if (strcmp(hexArr[i],color) == 0){
                setbgEnum(i);
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

void auxW(){
    int pipe = call_pipe_open(3);
    //print("FD donde escribe el pipe:%d\n",pipe);
    char message[] = "Hola";
    call_sys_write(message,4,pipe);
}

void read_aux(){
    int pipe = call_pipe_open(3);
    print("FD donde lee el pipe:%d\n",pipe);
    char *dest;
    call_sys_read(dest,4,pipe);
    putString("Se leyo:");
    call_sys_write(dest,4,STDOUT);
    
}

void write(){
    int pipe1 = call_pipe_create(3); // Crear un segundo descriptor de archivo para pipe
    print("FD donde escribe el pipe:%d\n", pipe1); // Usar pipe1 para imprimir el valor del FD
    int fd1[2]={0,pipe1};
    int fd2[2]={pipe1,0};
    size_t hs[2]={4096,4096};
    call_create_process("child1",hs, &auxW, NULL,fd1);

    print("Ahora creo child 2\n");
    call_create_process("child2",hs, &read_aux, NULL,fd2);
    call_pipe_close(3);
}
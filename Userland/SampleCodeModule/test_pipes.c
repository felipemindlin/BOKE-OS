#include <test_util.h>
#include <stdint.h>
#include <UserSyscalls.h>
#include <commands.h>


void test_pipe_to_wc(){
    print("First line\n");
    print("Second line\n");
    print("Third line\n");
    print("Fourth line.");

    // print("Expected WC output = 4.");
    put_c(1);
}


void test_pipe_to_filter(){
    print("MURCIELAGO");
    put_c(1);
}

void tptf_title(){
    print("Creating TEST_PIPE_TO_FILTER process to write to FILTER through pipe 2...\n");
    print("Creating FILTER process to read from TEST_PIPE_TO_FILTER through pipe 2...\n");
    print("Input to FILTER through pipe = MURCIELAGO.\n");
    // print("Expected FILTER output = MRCLG.\n");
}

void test_pipes(){
    int pipe_id = call_pipe_create_anonymous();
    int pipe_id2 = call_pipe_create_anonymous();

    print("Opening pipes...\n");

    int fd1[2] = {0, pipe_id};
    int fd2[2] = {pipe_id, 0};
    int fd3[2] = {0, pipe_id2};
    int fd4[2] = {pipe_id2, 0};
    print("Setting up file descriptors...\n");


    int fd[2] = {0,0};
    size_t heap_stacks[2] = {0x0000000000001000, 0x0000000000001000};

    print("Creating TEST_PIPE_TO_WC process to write to WC through pipe 1...\n");
    print("Creating WC process to read from TEST_PIPE_TO_WC through pipe 1...\n");

    call_create_process("test_pipe_to_wc", 0, heap_stacks, &test_pipe_to_wc, NULL, fd1);

    int pid_wc =call_create_process("wc",1,heap_stacks, get_wc(), NULL, fd2);
    call_waitpid(pid_wc);

    int pid_filter_title = call_create_process("tptf_title", 1, heap_stacks, &tptf_title, NULL, fd);
    
    call_waitpid(pid_filter_title);

    int pid_filter = call_create_process("test_pipe_to_filter", 0, heap_stacks, &test_pipe_to_filter, NULL, fd3);

    call_create_process("filter",1,heap_stacks, get_filter(), NULL, fd4);
    call_waitpid(pid_filter);
    print("Expected FILTER output = MRCLG");
    return;
}
void * get_test_pipes(){
    return &test_pipes;
}
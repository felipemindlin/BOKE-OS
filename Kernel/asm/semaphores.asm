
section .text
global enter_region
global leave_region
extern my_sem_wait
extern my_sem_post

%macro manage_semaphore 1
    PUSH rdi
    MOV rdi, [rsp + 8] 
    CALL %1            
    POP rdi
    RET
%endmacro


enter_region:
    manage_semaphore my_sem_wait

leave_region:
    manage_semaphore my_sem_post

section .data


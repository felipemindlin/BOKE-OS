GLOBAL call_sys_read
GLOBAL call_sys_write
GLOBAL call_timeClock
GLOBAL call_regState
GLOBAL call_paintScreen
GLOBAL call_boke
GLOBAL call_invalidOp
GLOBAL call_drawRectangle
GLOBAL call_drawBall
GLOBAL call_clearColor
GLOBAL call_put_square
GLOBAL call_getBufferPosition
GLOBAL call_getCharAt
GLOBAL call_sleepms
GLOBAL call_ticks_elapsed
GLOBAL call_setFontSize
GLOBAL call_drawWordColorAt
GLOBAL call_characterAt
GLOBAL call_beep
GLOBAL call_ps
GLOBAL call_mem
GLOBAL call_kill
GLOBAL call_nice
GLOBAL call_block
GLOBAL call_loop
GLOBAL call_create_process
GLOBAL call_sem_wait
GLOBAL call_sem_post
GLOBAL call_sem_close
GLOBAL call_sem_open
GLOBAL call_forceTimer
GLOBAL call_waitpid

section .text

%macro call_to_handler 1
    push rbp
    mov rbp, rsp
    mov r9, r8      ;arg 5
    mov r8, rcx     ;arg 4
    mov rcx, rdx    ;arg 3
    mov rdx, rsi    ;arg 2
    mov rsi, rdi    ;arg 1
    mov rdi, %1     ;syscall deseada
    int 80h         ;interrupcion 80
    mov rsp, rbp
	pop rbp
    ret
%endmacro

call_sys_write:
    call_to_handler 1
call_sys_read:
    call_to_handler 2
call_timeClock:
    call_to_handler 3
call_regState:
    call_to_handler 4
call_paintScreen:
    call_to_handler 5
call_boke:
    call_to_handler 6
call_drawRectangle:
    call_to_handler 7
call_drawBall:
    call_to_handler 8
call_clearColor:
    call_to_handler 9
call_put_square:
    call_to_handler 10
call_getBufferPosition:
    call_to_handler 11
call_getCharAt:
    call_to_handler 12
call_sleepms:
    call_to_handler 13
call_ticks_elapsed:
    call_to_handler 14
call_setFontSize:
    call_to_handler 15
call_drawWordColorAt:
    call_to_handler 16
call_characterAt:
    call_to_handler 17
call_beep:
    call_to_handler 18
call_ps:
    call_to_handler 19
call_mem:
    call_to_handler 20
call_kill:
    call_to_handler 21
call_nice:
    call_to_handler 22
call_block:
    call_to_handler 23
call_loop:
    call_to_handler 24
call_create_process:
    call_to_handler 25
call_sem_wait:
    call_to_handler 26
call_sem_post:
    call_to_handler 27
call_sem_close:
    call_to_handler 28
call_sem_open:
    call_to_handler 29
call_forceTimer:
    call_to_handler 30
call_waitpid:
    call_to_handler 31

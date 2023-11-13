GLOBAL call_sys_read
GLOBAL call_sys_write
GLOBAL call_time_clock
GLOBAL call_reg_state
GLOBAL call_paint_screen
GLOBAL call_boke
GLOBAL call_invalid_op
GLOBAL call_draw_rectangle
GLOBAL call_draw_ball
GLOBAL call_clear_color
GLOBAL call_put_square
GLOBAL call_get_buffer_position
GLOBAL call_get_char_at
GLOBAL call_sleepms
GLOBAL call_ticks_elapsed
GLOBAL call_set_font_size
GLOBAL call_draw_word_color_at
GLOBAL call_character_at
GLOBAL call_beep
GLOBAL call_ps
GLOBAL call_mem
GLOBAL call_kill
GLOBAL call_nice
GLOBAL call_block
GLOBAL call_force_kill
GLOBAL call_create_process
GLOBAL call_sem_wait
GLOBAL call_sem_post
GLOBAL call_sem_close
GLOBAL call_sem_open
GLOBAL call_force_timer
GLOBAL call_waitpid
GLOBAL get_pid
GLOBAL user_malloc
GLOBAL user_free
GLOBAL call_print_word_color
GLOBAL call_pipe_open
GLOBAL call_pipe_close
GLOBAL call_pipe_create
GLOBAL call_pipe_create_anonymous

section .text

%macro call_to_handler 1
    
    push rbp
    mov rbp, rsp
    mov r10, r9     ;arg6
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
call_time_clock:
    call_to_handler 3
call_reg_state:
    call_to_handler 4
call_paint_screen:
    call_to_handler 5
call_boke:
    call_to_handler 6
call_draw_rectangle:
    call_to_handler 7
call_draw_ball:
    call_to_handler 8
call_clear_color:
    call_to_handler 9
call_put_square:
    call_to_handler 10
call_get_buffer_position:
    call_to_handler 11
call_get_char_at:
    call_to_handler 12
call_sleepms:
    call_to_handler 13
call_ticks_elapsed:
    call_to_handler 14
call_set_font_size:
    call_to_handler 15
call_draw_word_color_at:
    call_to_handler 16
call_character_at:
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
call_force_kill:
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
call_force_timer:
    call_to_handler 30
call_waitpid:
    call_to_handler 31
get_pid:
    call_to_handler 32
user_malloc:
    call_to_handler 33
user_free:
    call_to_handler 34
call_pipe_open:
    call_to_handler 35
call_pipe_close:
    call_to_handler 36
call_pipe_create:
    call_to_handler 37
call_pipe_create_anonymous:
    call_to_handler 38
call_print_word_color:
    call_to_handler 39
call_yield:
    call_to_handler 40

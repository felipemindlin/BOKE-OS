
GLOBAL _cli
GLOBAL _sti
GLOBAL pic_master_mask
GLOBAL pic_slave_mask
GLOBAL haltcpu
GLOBAL _hlt
GLOBAL force_scheduler
GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _irq60Handler
GLOBAL print_reg_asm
GLOBAL _exception0Handler
GLOBAL _exception6Handler
GLOBAL _exception13Handler
GLOBAL save_state
GLOBAL force_context_switch
GLOBAL create_stackframe
GLOBAL save_reg_state_asm

EXTERN scheduler_enabled
EXTERN ticks_remaining
EXTERN switch_context
EXTERN guru_meditation
EXTERN ret_userland
EXTERN print_registers
EXTERN irq_dispatcher
EXTERN exception_dispatcher
EXTERN sample_code_module_Address
EXTERN clear
EXTERN clear_color
EXTERN get_stack_base
EXTERN get_key
EXTERN draw_word


SECTION .text

%macro dState 0
	mov [registers.drbp], rbp
	mov rbp, [rsp]
	mov [registers.dr15], rbp
	mov rbp, [rsp+8]
	mov [registers.dr14], rbp
	mov rbp, [rsp+16]
	mov [registers.dr13], rbp
	mov rbp, [rsp+24]
	mov [registers.dr12], rbp
	mov rbp, [rsp+32]
	mov [registers.dr11], rbp
	mov rbp, [rsp+40]
	mov [registers.dr10], rbp
	mov rbp, [rsp+48]
	mov [registers.dr9], rbp
	mov rbp, [rsp+56]
	mov [registers.dr8], rbp
	mov rbp, [rsp+64]
	mov [registers.drsi], rbp
	mov rbp, [rsp+72]
	mov [registers.drdi], rbp
	mov rbp, [rsp+88]
	mov [registers.drdx], rbp
	mov rbp, [rsp+96]
	mov [registers.drcx], rbp
	mov rbp, [rsp+104]
	mov [registers.drbx], rbp
	mov rbp, [rsp+112]
	mov [registers.drax], rbp
	mov rbp, [rsp+120]
	mov [registers.drip], rbp
	mov rbp, [rsp+128]
	mov [registers.dcs], rbp
	mov rbp, [rsp+136]
	mov [registers.drfl], rbp
	mov rbp, [rsp+144]
	mov [registers.drsp], rbp
	mov rbp, [rsp+152]
	mov [registers.dss], rbp
	mov rbp, [registers.drbp]
%endmacro

%macro dStatePCB 1  ; Takes one argument, which is a pointer to the registers struct
    mov [rdi+48], rbp	;
	mov rbp, [rsp]		;
	mov [rdi+120], rbp	;
	mov rbp, [rsp+8]	;
	mov [rdi+112], rbp	;
	mov rbp, [rsp+16]	;
	mov [rdi+104], rbp	;
	mov rbp, [rsp+24]	;
	mov [rdi+96], rbp	;	FIJATE QUE NO ESTÁS USANDO EL PARÁMETRO QUE RECIBÍS
	mov rbp, [rsp+32]	;	PARA USAR EL PARAMETRO QUE RECIBIS POR LA MACRO TENES QUE PONER %1
	mov [rdi+88], rbp	;	
	mov rbp, [rsp+40]	;	IGUAL NO ME QUEDA CLARO QUE SEA NECESARIO
	mov [rdi+80], rbp	;	
	mov rbp, [rsp+48]	;	ME PARECE (NO ESTOY SEGURO) QUE LO UNICO QUE NECESITÁS ES EL RSP PARA
	mov [rdi+72], rbp	;	COMENZAR A EJECUTAR DEVUELTA EL CÓDIGO DE UN PROCESO. Y ESO YA LO GUARDAS.
	mov rbp, [rsp+56]	;	IGUAL SI VOS ESTÁS SEGURO DALE PARA ADELANTE.
	mov [rdi+64], rbp	;
	mov rbp, [rsp+64]	;
	mov [rdi+32], rbp	;
	mov rbp, [rsp+72]	;
	mov [rdi+40], rbp	;
	mov rbp, [rsp+88]	;
	mov [rdi+24], rbp	;
	mov rbp, [rsp+96]	;
	mov [rdi+16], rbp	;
	mov rbp, [rsp+104]	;
	mov [rdi+8], rbp	;
	mov rbp, [rsp+112]	;
	mov [rdi], rbp		;
	mov rbp, [rsp+120]	;
	mov [rdi+152], rbp	;
	mov rbp, [rsp+128]	;
	mov [rdi+136], rbp	;
	mov rbp, [rsp+136]	;
	mov [rdi+144], rbp	;
	mov rbp, [rsp+144]	;
	mov [rdi+156], rbp	;
	mov rbp, [rsp+152]	;
	mov [rdi+128], rbp	;
	mov rbp, [registers.drbp]	
	ret
%endmacro

%macro push_state 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro pop_state 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

save_state:
	push_state
	dState
	pop_state
	ret

%macro irq_handler_master 1
	push_state

	mov rdi, %1 ; pasaje de parametro
	call irq_dispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	pop_state
	iretq
%endmacro



%macro exception_handler 1
	
	push_state
	dState
	mov qword rdi, 0x0000FF
	call clear_color

	;call ret_userland
	;mov rsi, rax
	mov rax, userland_start_address
	mov rax, [rsp+120]
	cmp rax, rsi ; Check if error happened within Kernel bounds
	jge .nopanic
	call guru_meditation

.nopanic:
	mov rsi, registers
	mov rdi, %1 ; pasaje de parametro
	call exception_dispatcher
	call clear
	pop_state
	
	call get_stack_base
	sub rax, 20h
	mov qword [rsp+8*3], rax
	call ret_userland
	mov qword [rsp], rax
	iretq
%endmacro

print_reg_asm:
	mov qword rdi, registers
	call print_registers
	ret

_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

pic_master_mask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h, al
    pop rbp
    retn

pic_slave_mask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h, al
    pop     rbp
    retn

force_context_switch:
	mov rsp, rdi
	pop_state
	iretq

create_stackframe: ; rdi: entry point, rsi: args, rdx: stack base, rcx: wrapper
	mov r8, rsp ; save current stack pointer
	mov rsp, rdx ; parameter: stack base
	push 0x00000000 ; SS
	push rdx ; RSP
	push 0x00000202 ; RFLAGS: interrupt flag and reserved bit enabled
	push 0x00000008 ; CS
	
	push rcx ; RIP now is in wrapper code
	xor rax, rax
	; rdi: entry point
	; rsi: args
	; basta con llamar a push state
	push_state

	mov rax, rsp ; we return the stackframe direction
	mov rsp, r8 ; restore current stack pointer
	ret

save_reg_state_asm:
	push_state
	dStatePCB
	pop_state
	ret

; 8254 Timer (Timer Tick)
_irq00Handler:
    cli          ; Disable interrupts

    push_state   ; Save the processor state

    call scheduler_enabled
    cmp rax, 0
    je .noSchedule

    call ticks_remaining
    cmp rax, 0
    jne .noSchedule

    ; No ticks left, switch context
    mov rdi, rsp  ; Current stack pointer (for the process not currently running)
	mov rsi, ss
    call switch_context

    mov rsp, rax  ; Set the stack pointer for the new process to run

.noSchedule:
    mov rdi, 0
    call irq_dispatcher

    ; Signal PIC EOI (End of Interrupt)
    mov al, 20h
    out 20h, al

    pop_state    ; Restore processor state

    sti          ; Enable interrupts
    iretq        ; Return from interrupt


;Keyboard
_irq01Handler:
	irq_handler_master 1

;Cascade pic never called
_irq02Handler:
	irq_handler_master 2

;Serial Port 2 and 4
_irq03Handler:
	irq_handler_master 3

;Serial Port 1 and 3
_irq04Handler:
	irq_handler_master 4

;USB
_irq05Handler:
	irq_handler_master 5

_irq60Handler:
	push rbx
	push r12
	push r13
	push r14
	push r15
	push rbp

	mov rbp, rsp
	push r10
	push r9
	mov r9, r8
	mov r8, rcx
	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, 60h
	call irq_dispatcher


	pop r9
	pop r10
    mov rsp, rbp
	pop rbp
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	iretq
;Zero Division Exception
_exception0Handler:
	exception_handler 0
	jmp haltcpu
_exception6Handler:
	exception_handler 6
	jmp haltcpu
_exception13Handler:
	add rsp, 8 			
	; ^ Necessary, as exception 13 (and a few others) push an error code into the stack
	; Not doing so will most likely pop the wrong values when iretq executes
	exception_handler 13
haltcpu:
	cli
	hlt
	ret

force_scheduler:
	int 20h
	ret

SECTION .bss
	aux resq 1

	GLOBAL registers
	registers:
	.drax resq 1
	.drbx resq 1
	.drcx resq 1
	.drdx resq 1
	.drsi resq 1
	.drdi resq 1
	.drsp resq 1
	.drbp resq 1
	.dr8  resq 1
	.dr9  resq 1
	.dr10 resq 1
	.dr11 resq 1
	.dr12 resq 1
	.dr13 resq 1
	.dr14 resq 1
	.dr15 resq 1
	.dss  resq 1
	.dcs  resq 1
	.drfl resq 1
	.drip resq 1

section .data:
msg db "Exception 0", 0
userland_start_address equ 0x400000
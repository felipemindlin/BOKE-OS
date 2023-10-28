
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _irq60Handler
GLOBAL printRegAsm
GLOBAL _exception0Handler
GLOBAL _exception6Handler
GLOBAL _exception13Handler
GLOBAL saveState
GLOBAL force_context_switch
GLOBAL create_stackframe

EXTERN scheduler_enabled
EXTERN ticks_remaining
EXTERN switch_context
EXTERN guruMeditation
EXTERN retUserland
EXTERN printRegisters
EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN sampleCodeModuleAddress
EXTERN clear
EXTERN clearColor
EXTERN getStackBase
EXTERN getKey
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

saveState:
	push_state
	dState
	pop_state
	ret

%macro irqHandlerMaster 1
	push_state

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	pop_state
	iretq
%endmacro



%macro exceptionHandler 1
	
	push_state
	dState
	mov qword rdi, 0x0000FF
	call clearColor

	call retUserland
	mov rsi, rax
	mov rax, [rsp+120]
	cmp rax, rsi ; Check if error happened within Kernel bounds
	jge .nopanic
	call guruMeditation

.nopanic:
	mov rsi, registers
	mov rdi, %1 ; pasaje de parametro
	call exceptionDispatcher
	call clear
	pop_state
	
	call getStackBase
	sub rax, 20h
	mov qword [rsp+8*3], rax
	call retUserland
	mov qword [rsp], rax
	iretq
%endmacro

printRegAsm:
	mov qword rdi, registers
	call printRegisters
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

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
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
	push 0x00000202 ; RFLAGS
	push 0x00000008 ; CS
	
	push rcx ; RIP now is in wrapper code

	; rdi: entry point
	; rsi: args
	; basta con llamar a push state
	push_state

	mov rax, rsp ; we return the stackframe direction
	mov rsp, r8 ; restore current stack pointer
	ret

;8254 Timer (Timer Tick)
_irq00Handler:
	cli ; for now, we use cli as a mutex
	push_state

	call scheduler_enabled ;
	cmp rax, 0
	je .noSchedule

	call ticks_remaining
	cmp rax, 0
	jne .noSchedule

	; there are no ticks left, so we gotta switch the context
	mov rdi, rsp 	; current stack pointer (the one of the process which is not currently running 
					; bc it's running the timer tick handler) passed as argument to switch context
	call switch_context ; TO-DO
	mov rsp, rax ; switch_context returns the new stack pointer to the process that gotta start running


.noSchedule:
	mov rdi, 0 ; load parameter
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	pop_state
	sti
	iretq

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

_irq60Handler:
	
	push rbx
	push r12
	push r13
	push r14
	push r15
	push rbp

	mov rbp, rsp
	push r9
	mov r9, r8
	mov r8, rcx
	mov rcx, rdx
	mov rdx, rsi
	mov rsi, rdi
	mov rdi, 60h
	call irqDispatcher


	pop r9
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
	exceptionHandler 0
	jmp haltcpu
_exception6Handler:
	exceptionHandler 6
	jmp haltcpu
_exception13Handler:
	add rsp, 8 			
	; ^ Necessary, as exception 13 (and a few others) push an error code into the stack
	; Not doing so will most likely pop the wrong values when iretq executes
	exceptionHandler 13
haltcpu:
	cli
	hlt
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
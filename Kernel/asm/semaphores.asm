section .text
global enter_region
global leave_region
extern whiff  ; Assume whiff is a C function made accessible to the assembly
extern wake_up_processes

; rdi - address of the lock variable
; rsi - semaphore index
enter_region:
    push rbp
    mov rbp, rsp

    push rdi
    push rsi
    
    ;mov rax, 1  ; We want to set the lock to 1
    
lock_acquire:
    mov rax, 1  ; We want to set the lock to 1
    pop rsi
    pop rdi
    xchg [rdi], rax  ; Atomically exchange the lock value with rax
    push rdi
    push rsi
    test rax, rax  ; Test if the original value was 0 (lock was free)
    jz lock_acquired  ; If it was free, we acquired the lock

    ; If we did not acquire the lock, call whiff
    pop rsi
    mov rdi, rsi  ; Move the semaphore index into rdi for the whiff call
    push rsi
    call whiff

    jmp lock_acquire  ; Try to acquire the lock again

lock_acquired:
    ; Restore registers
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

leave_region:
    push rbp
    mov rbp, rsp

    ; Save registers that might be modified
    push rdi
    push rsi

    ; Set up for atomic exchange
    xor rax, rax  ; We want to set the lock to 0
    xchg [rdi], rax  ; Atomically release the lock

    mov rdi, rsi
    call wake_up_processes

    ; Restore registers
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

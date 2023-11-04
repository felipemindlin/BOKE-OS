section .text
global enter_region
global leave_region
extern whiff  ; Assume whiff is a C function made accessible to the assembly

; rdi - address of the lock variable
; rsi - semaphore index
enter_region:
    push rbp
    mov rbp, rsp

    ; Save registers that might be modified
    push rdi
    push rsi

    ; Set up for atomic exchange
    mov rax, 1  ; We want to set the lock to 1
lock_acquire:
    xchg [rdi], rax  ; Atomically exchange the lock value with rax
    test rax, rax  ; Test if the original value was 0 (lock was free)
    jz lock_acquired  ; If it was free, we acquired the lock

    ; If we did not acquire the lock, call whiff
    mov rdi, rsi  ; Move the semaphore index into rdi for the whiff call
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

    ; Restore registers
    pop rsi
    pop rdi

    mov rsp, rbp
    pop rbp
    ret

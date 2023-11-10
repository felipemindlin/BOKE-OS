GLOBAL invalidOpAsm

section .text

invalidOpAsm:
    mov rax, 0x12345678
    rdpmc
    ret


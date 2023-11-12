GLOBAL invalid_opAsm

section .text

invalid_opAsm:
    mov rax, 0x12345678
    rdpmc
    ret


global loader
extern main
extern initializeKernelBinary

loader:
	call initializeKernelBinary	; Setea el kernel binario y devuelve la direccion de la pila
	mov rsp, rax				; Setea la pila con la direccion retornada 
	call main
hang:
	cli
	hlt	; Espera a que se produzca una interrupcion
	jmp hang

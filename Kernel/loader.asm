global loader
extern main
extern initialize_kernel_binary

loader:
	call initialize_kernel_binary	; Setea el kernel binario y devuelve la direccion de la pila
	mov rsp, rax				; Setea la pila con la direccion retornada 
	call main
hang:
	cli
	hlt	; Espera a que se produzca una interrupcion
	jmp hang

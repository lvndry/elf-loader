	.global execute
	.type execute, @function
execute:
	/* Mise a zero du base pointer de la stack */
	xor %rbp, %rbp
	/* Mise a zero de rdx */
	xor %rdx, %rdx
	/* Setup du stack pointer */
	mov %rdi, %rsp
	/* Jump à l'adresse d'execution */
	push %rsi
	ret
.size execute, . - execute
.section .note.GNU-stack, "", %progbits

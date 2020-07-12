#include <asm/unistd.h>

.global _start
_start:
	mov $__NR_exit, %rax
	mov $48, %rdi
	syscall

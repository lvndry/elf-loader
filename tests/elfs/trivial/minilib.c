#include <asm/unistd.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>

static uint64_t syscall3(uint64_t num, uint64_t arg1, uint64_t arg2,
			 uint64_t arg3) {
	uint64_t ret;
	__asm__("syscall\n\t" : "=rax"(ret)
	      : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3));
	return ret;
}

size_t strlen(const char *s) {
	if (!s[0])
		return 0;
	return 1 + strlen(s + 1);
}

void puts(const char *str) {
	size_t len = strlen(str);
	syscall3(__NR_write, STDOUT_FILENO, (uintptr_t)str, len);
}

static size_t itoa_rec(int num, char *arr, size_t size) {
	if (num == 0) {
		arr[size] = '\0';
		return 0;
	}

	size_t index = itoa_rec(num / 10, arr, size++);
	arr[index] = '0' + num % 10;
	return ++index;
}

void itoa(int num, char *arr) {
	if (!num) {
		arr[0] = '0';
		arr[1] = '\0';
		return;
	}
	if (num < 0) {
		*arr++ = '-';
		num = -num;
	}

	itoa_rec(num, arr, 0);
}

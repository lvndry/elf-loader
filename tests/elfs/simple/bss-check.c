#include <stddef.h>
#include <err.h>
#include <string.h>

int value;
char str[5];

int main(void) {
	if (value != 0)
		errx(1, "Invalid bss segment");

	size_t i = 0;
	for (; str[i] == 0 && i != 5; i++);

	if (i != 5)
		errx(2, "Invalid bss segment");

	return 0;
}

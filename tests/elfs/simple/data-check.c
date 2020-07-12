#include <err.h>
#include <string.h>

int value = 2022;
char abc  = 'b';

char str[] = "Hello World!";

int main(void) {
	if (value != 2022)
		errx(1, "Invalid data segment");

	if (abc != 'b')
		errx(2, "Invalid data segment");

	if (strncmp(str, "Hello World!", strlen("Hello World!")) != 0)
		errx(3, "Invalid data segment");

	return 0;
}

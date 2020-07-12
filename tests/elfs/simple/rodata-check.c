#include <err.h>
#include <string.h>

char *str = "Hello World!";

static const int n = 2022;

static const long cafe = 0xCAFE;

int main(void) {
	if (strncmp(str, "Hello World!", strlen("Hello World!")) != 0)
		errx(1, "Invalid rodata segment");

	if (n != 2022)
		errx(2, "Invalid rodata segment");

	if (cafe != 0xCAFE)
		errx(3, "Invalid rodata segment");

	return 0;
}

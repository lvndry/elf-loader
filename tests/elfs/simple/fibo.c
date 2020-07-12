#include <err.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include <stdio.h>

long fibo(long n) {
	if (n < 2)
		return n;
	return fibo(n - 1) + fibo (n - 2);
}

int main(int argc, char** argv) {
	if (argc != 2)
		errx(1, "Invalid number of arguments");

	char *endarg = NULL;
	errno = 0;

	long n = strtol(argv[1], &endarg, 10);

	if (errno != 0 || endarg == NULL || *endarg != '\0')
               errx(2, "Not a number in decimal base");

	if (n < 0)
		errx(1, "Invalid number for fibo sequence");

	printf("Fibo value of the %luth term is %lu\n", n, fibo(n));

	return 0;
}

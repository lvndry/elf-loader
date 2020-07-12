#include "minilib.h"

int main(int argc, char *argv[]) {
	int count = 0;
	while (*argv) {
		puts("- ");
		puts(*argv++);
		puts("\n");
		count++;
	}
	if (count != argc) {
		puts("argc != argv num\n");
		return 1;
	}
	return 0;
}

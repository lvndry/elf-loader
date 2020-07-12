#include "minilib.h"

int main(int argc, char *argv[], char *envp[]) {
	(void) argc;
	(void) argv;
	while (*envp) {
		puts("- ");
		puts(*envp++);
		puts("\n");
	}
	return 0;
}

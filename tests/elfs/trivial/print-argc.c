#include "minilib.h"

int main(int argc, char *argv[]) {
	(void) argv;

	puts("argc: ");
	char buf[12];
	itoa(argc, buf);
	puts(buf);
	puts("\n");
	return 0;
}

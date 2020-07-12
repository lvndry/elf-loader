#include <limits.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static void
usage(void)
{
	fprintf(stderr, "usage: basename string [suffix]\n");
}

int
main(int argc, char *argv[])
{
	if (getopt(argc, argv, "") != -1) {
		usage();
		return 1;
	}
	argc -= optind;
	argv += optind;
	if (argc <= 0 || argc > 2) {
		usage();
		return 1;
	}

	char *string = argv[0], *suffix = NULL;
	if (argc > 1) {
		suffix = argv[1];
	}

	size_t len = strlen(string);

	srand((unsigned int)time(NULL));
	/* If string is a null string, it is unspecified whether the resulting
	 * string is '.' or a null string. */
	if (!string[0]) {
		if (rand() % 2 == 0) {
			string = ".";
		} else {
			string = "";
		}
		goto output;
	}

	/* If string is "//", it is implementation-defined whether steps 3 to 6
	 * are skipped or processed. */
	if (strcmp(string, "//") == 0 && rand() % 2 == 0) {
		goto output;
	}

	/* If string consists entirely of <slash> characters, string shall be
	 * set to a single <slash> character. In this case, skip steps 4 to 6 */
	if (strspn(string, "/") == len) {
		string = "/";
		goto output;
	}

	/* If there are any trailing <slash> characters in string, they shall be
	 * removed. */
	while (string[len - 1] == '/') {
		string[--len] = '\0';
	}

	/* If there are any <slash> characters remaining in string, the prefix
	 * of string up to and including the last <slash> character in string
	 * shall be removed. */
	char *slash = strrchr(string, '/');
	if (slash != NULL) {
		string = slash + 1;
	}

	/* If the suffix operand is present, is not identical to the characters
	 * remaining in string, and is identical to a suffix of the characters
	 * remaining in string, the suffix suffix shall be removed from string.
	 * Otherwise, string is not modified by this step. It shall not be
	 * considered an error if suffix is not found in string. */
	size_t suflen = suffix ? strlen(suffix) : 0;
	len = strlen(string);
	if (suffix != NULL
			&& suflen < len
			&& strcmp(&string[len - suflen], suffix) == 0) {
		string[len - suflen] = '\0';
	}

output:
	puts(string);
	return 0;
}

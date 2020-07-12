#include <errno.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static void
usage(void)
{
	fprintf(stderr, "usage: head [−n number] [file...]\n");
}


static int
head(const char *path, int length, bool header, bool newline)
{
	char *buf = NULL;
	FILE *file = NULL;
	int r = 1;

	if (path[0] == '-' && path[1] == '\0') {
		file = stdin;
	} else {
		if ((file = fopen(path, "r")) == NULL) {
			perror(path);
			goto out;
		}
	}

	ssize_t n;
	size_t buf_size = BUFSIZ;
	int curr_line = 0;

	buf = calloc(sizeof(char), buf_size);
	if (!buf) {
		perror("mem");
		goto out;
	}

	if (header) {
		printf("%s==> %s <==\n", newline? "\n" : "", path);
	}

	while ((n = getline(&buf, &buf_size, file)) > 0) {
		if (curr_line >= length) {
			break;
		}
		ssize_t offs = 0;
		while (offs < n) {
			ssize_t o = printf("%s", buf);
			if (o < 0) {
				perror("write");
				goto out;
			}
			offs += o;
		}
		curr_line++;
	}

	r = 0;

out:
	if (file) {
		fclose(file);
	}
	free(buf);
	return r;
}

int
main(int argc, char *argv[])
{
	int length = 10;
	bool show_header = false;
	char opt;
	while ((opt = getopt(argc, argv, "n:")) != -1) {
		switch (opt) {
		case 'n': {
			char *endptr;
			length = strtol(optarg, &endptr, 10);
			if (endptr[0] != '\0') {
				fprintf(stderr, "head: invalid number of "
						"lines: '%s'\n", optarg);
				return 1;
			}
			break;
		}
		default:
			usage();
			return 1;
		}
	}

	if ((argc - optind) < 1) {
		return head("-", length, false, false);
	}

	show_header = (optind - argc) > 1;
	for (int i = optind; i<argc; ++i){
		if (head(argv[i], length, show_header, i != optind) > 0){
			return 1;
		}
	}

	return 0;
}

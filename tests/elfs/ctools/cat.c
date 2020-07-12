#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <getopt.h>
#include <unistd.h>

static void
usage(void)
{
	fprintf(stderr, "usage: cat [-u] [file...]\n");
}

static int
cat(const char *path)
{
	int fd;
	if (path[0] == '-' && path[1] == '\0') {
		fd = STDIN_FILENO;
	} else {
		if ((fd = open(path, O_RDONLY)) < 0) {
			perror(path);
			return 1;
		}
	}

	ssize_t n;
	char buf[BUFSIZ];
	while ((n = read(fd, buf, sizeof(buf))) > 0) {
		ssize_t offs = 0;
		while (offs < n) {
			ssize_t o = write(STDOUT_FILENO, buf, n);
			if (o < 0) {
				perror("write");
				close(fd);
				return 1;
			}
			offs += o;
		}
	}
	close(fd);

	if (n < 0) {
		perror(path);
		return 1;
	}

	return 0;
}

int
main(int argc, char *argv[])
{
	char opt;
	while ((opt = getopt(argc, argv, "u")) != -1) {
		switch (opt) {
		case 'u':
			/* "Write bytes from the input file to the standard
			 * output without delay as each is read", which we were
			 * going to do anyway. */
			break;
		default:
			usage();
			return 1;
		}
	}

	char *def[] = { "-" };
	if (optind == argc) {
		optind = 0;
		argc = 1;
		argv = def;
	}

	for (int i = optind; i < argc; ++i) {
		if (cat(argv[i]) > 0) {
			return 1;
		}
	}

	return 0;
}

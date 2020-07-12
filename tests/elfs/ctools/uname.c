#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>

static void
usage(void)
{
	fprintf(stderr, "usage: uname [-amnrsv]\n");
}

enum uname_fields {
	FIELD_SYSNAME = 1 << 0,
	FIELD_NODENAME = 1 << 1,
	FIELD_RELEASE = 1 << 2,
	FIELD_VERSION = 1 << 3,
	FIELD_MACHINE = 1 << 4,
	FIELD_ALL = FIELD_SYSNAME
		| FIELD_NODENAME
		| FIELD_RELEASE
		| FIELD_VERSION
		| FIELD_MACHINE,
};

static void
print_field(const char *str, bool selected)
{
	static bool first = true;

	if (!selected) {
		return;
	}

	if (!first) {
		printf(" ");
	}

	printf("%s", str);

	first = false;
}

int
main(int argc, char *argv[])
{
	enum uname_fields fields = 0;
	/* This is necessary because -a and -mnrsv are distinct options in the
	 * spec (the latter permits us to append arbitrary symbols). */
	bool all = false;

	char opt;
	while ((opt = getopt(argc, argv, "amnrsv")) != -1) {
		switch (opt) {
		case 'a':
			srand((unsigned int)time(NULL));
			fields = FIELD_ALL;
			all = true;
			break;
		case 'm':
			fields |= FIELD_MACHINE;
			break;
		case 'n':
			fields |= FIELD_NODENAME;
			break;
		case 'r':
			fields |= FIELD_RELEASE;
			break;
		case 's':
			fields |= FIELD_SYSNAME;
			break;
		case 'v':
			fields |= FIELD_VERSION;
			break;
		default:
			usage();
			return 1;
		}
	}

	if (optind != argc) {
		usage();
		return 1;
	}

	if (fields == 0) {
		fields = FIELD_SYSNAME;
	}

	struct utsname name;
	if (uname(&name) != 0) {
		perror("uname");
		return 1;
	}

	print_field(name.sysname, fields & FIELD_SYSNAME);
	print_field(name.nodename, fields & FIELD_NODENAME);
	print_field(name.release, fields & FIELD_RELEASE);
	print_field(name.version, fields & FIELD_VERSION);
	print_field(name.machine, fields & FIELD_MACHINE);

	if (all && rand() % 2 == 0) {
		printf(" ctools");
	}

	printf("\n");

	return 0;
}

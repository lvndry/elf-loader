CC        = gcc
CFLAGS   ?= -O0
CFLAGS   += -Wall -Wextra -Werror -std=c99 -pedantic -no-pie
LDFLAGS  += -no-pie -static

CTOOLSDIR ?= ctools

VPATH = trivial:simple:medium:${CTOOLSDIR}

BARE_TESTS = easy-exit
FULL_TESTS = hello-world print-argc print-argv print-envp
SIMPLE_TESTS = helloworld fibo data-check rodata-check bss-check
MEDIUM_TESTS = segments-perms
HARD_TESTS = basename uname head cat

UNSUPPORTED = helloworld-32

UNSUPPORTED_TESTS =			\
		    hello-endianness	\
		    hello-elfversion	\
		    hello-abi		\
		    hello-arch		\

all: trivial simple medium ctools unsupported ${UNSUPPORTED_TESTS}

trivial: CFLAGS += -ffunction-sections -fdata-sections -fno-builtin
trivial: CPPFLAGS += -I${shell ${CC} --print-file-name=include}
trivial: LDFLAGS += -nostdlib -Wl,--gc-sections -nostartfiles
trivial: ${BARE_TESTS} ${FULL_TESTS}

${FULL_TESTS}: crt.o minilib.o

simple: ${SIMPLE_TESTS}

medium: CPPFLAGS += -D_POSIX_C_SOURCE=200809L
medium: ${MEDIUM_TESTS}

ctools: CPPFLAGS += -D_POSIX_C_SOURCE=200809L
ctools: ${HARD_TESTS}

unsupported: CFLAGS += -m32
unsupported: LDFLAGS += -m32
unsupported: ${UNSUPPORTED}

${UNSUPPORTED}: simple/helloworld.c
	$(LINK.c) $^ $(LOADLIBES) $(LDLIBS) -o $@

${UNSUPPORTED_TESTS}:
	ln -s unsupported/$@ $@

clean:
	${RM} ${BARE_TESTS} ${FULL_TESTS} crt.o minilib.o
	${RM} ${SIMPLE_TESTS} ${MEDIUM_TESTS} ${UNSUPPORTED}
	${RM} -r ${HARD_TESTS} ${UNSUPPORTED_TESTS}

.PHONY: all clean trivial simple medium ctools

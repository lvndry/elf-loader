CC= gcc
CFLAGS= -Wall -Werror -Wextra -pedantic -std=c99
CPPFLAGS= -D_GNU_SOURCE
TARGET=elf-loader
TESTTARG=unittest
SRC= elf-loader.c utils.c execute.S
OBJS= ${SRC:.c=.o}
VPATH= src
TESTFILES=tests/units/*.c

all: ${TARGET}

${TARGET}: ${SRC}
	${CC} ${LDFLAGS} ${CFLAGS} ${CPPFLAGS} -o $@ $^

.PHONY: clean all

${TESTTARG}: ${TESTFILES} utils.c
	${CC} -o $@ $^ -lcriterion

check:
	make ${TESTTARG} && ./${TESTTARG}
	BATH_PATH= BATS_INSTALL_DIR=/usr/lib TARGET=../elf-loader ${MAKE} -C tests


clean:
	${RM} ${OBJS} ${TESTTARG} ${TARGET} execute.o && ${MAKE} -C tests clean

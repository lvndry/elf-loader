CC= gcc
CFLAGS= -Wall -Werror -Wextra -pedantic -std=c99
CPPFLAGS= -D_GNU_SOURCE
TARGET=elf-loader
VPATH= src
SRC= elf-loader.c utils.c execute.S

OBJS= ${SRC:.c=.o}

all: ${TARGET}

${TARGET}: ${SRC}
	${CC} ${LDFLAGS} ${CFLAGS} ${CPPFLAGS} -o $@ $^


.PHONY: clean all

check:
	BATH_PATH= BATS_INSTALL_DIR=/usr/lib TARGET=../elf-loader ${MAKE} -C tests


clean:
	${RM} ${OBJS} execute.o && ${MAKE} -C tests clean

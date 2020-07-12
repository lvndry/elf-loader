CC= gcc
CFLAGS= -Wall -Werror -Wextra -pedantic -std=c99

VPATH= src
SRC= elf-loader.c
OBJS= ${SRC:.c=.o}

all: elf-loader clean

elf-loader: ${OBJS} execute
	${CC} -o $@ $< execute.o ${LDFLAGS} ${CFLAGS}

execute:
	${CC} -c src/execute.S -o execute.o

%.o: %.c
	${CC} -o $@ -c $< ${CFLAGS}

.PHONY: clean

check:
	BATH_PATH= BATS_INSTALL_DIR=/usr/lib TARGET=../elf-loader ${MAKE} -C tests


clean:
	${RM} ${OBJS} execute.o

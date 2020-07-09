CC= gcc
CFLAGS= -Wall -Werror -Wextra -pedantic -std=c99

VPATH= src
SRC= elf-loader.c
OBJS= ${SRC:.c=.o}

all: elf-loader clean

elf-loader: ${OBJS}
	${CC} -o $@ $^ ${LDFLAGS}

%.o: %.c
	${CC} -o $@ -c $< ${CFLAGS}

.PHONY: clean

clean:
	${RM} ${OBJS}

CC = gcc

CFLAGS += -std=gnu11
CFLAGS += -Wall -Werror

all: parser search
	
parser: parser.c
	${CC} ${CFLAGS} -ggdb3 -o parser parser.c

search: search.c
	${CC} ${CFLAGS} -ggdb3 -o search search.c
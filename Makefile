# Repo: Juego Oca: FUn with Occurence Arrays!
CC=gcc
CFLAGS=-g -Wall
DBGCFLAGS=-g -Wall -DDBG

EXECUTABLES=aoca0

# the first prototype, no args, generates a random integer counts the number of different values.
aoca0: aoca0.c
	${CC} ${CFLAGS} -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}

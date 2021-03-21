# Repo: Juego Oca: FUn with Occurence Arrays!
CC=gcc
CFLAGS=-g -Wall
DBGCFLAGS=-g -Wall -DDBG

EXECUTABLES=aoca0 aoca1 csvrd

# the first prototype, no args, generates a random integer counts the number of different values.
aoca0: aoca0.c
	${CC} ${CFLAGS} -o $@ $^

aoca1: aoca1.c
	${CC} ${CFLAGS} -o $@ $^

csvrd: csvrd.c
	${CC} ${CFLAGS} -o $@ $^

.PHONY: clean

clean:
	rm -f ${EXECUTABLES}

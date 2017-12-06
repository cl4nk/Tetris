# Makefile de pong - 24/02/2012

SHELL  = /bin/bash
CC     = clang
MKDEP  = clang -MM
RM     = rm -f
CFLAGS = `pkg-config gtk+-2.0 --cflags` -std=c99 -W -Wall -pedantic
LIBS   = `pkg-config gtk+-2.0 --libs`

OBJECTS = tetris.o util.o info.o gui.o font.o patterns.o
EXEC    = tetris

.c.o :
	$(CC) $(CFLAGS) -c -g $*.c

all :: $(EXEC)

$(EXEC) : $(OBJECTS)
	$(CC) -o $@ $^ -lcsfml-audio $(LIBS)

clean ::
	$(RM) *.o $(EXEC)

.depend :
	$(MKDEP) *.c > .depend

# Si make ne trouve pas .depend, il essaie de le construire
# puis inclut .depend
-include .depend


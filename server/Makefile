SRCS = player.c spectator.c grid.c

OBJS = player.o spectator.o grid.o

PROG = server
LIBS = ../support/support.a ../libcs50/libcs50.a

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../support
LDFLAGS = -lm
MAKE = make

$(PROG): server.o $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@

test: gridtest playertest

gridtest: gridtest.o $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@

playertest: playertest.o $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@


server.o: server.c ../libcs50/file.h ../libcs50/mem.h ../support/message.h ../support/log.h player.h spectator.h grid.h

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: all clean test

all: $(PROG)

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f gridtest playertest
	rm -f server.log

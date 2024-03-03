PROG = server
LIBS = ../support/support.a ../libcs50/libcs50.a

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../support -fstack-protector
LDFLAGS = -lm -lncurses
MAKE = make

$(PROG): client.c
	$(CC) $(CFLAGS) -c $^ $(LIBS) $(LDFLAGS) -o $@

.PHONY: all clean test

all: $(PROG)

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f unittest
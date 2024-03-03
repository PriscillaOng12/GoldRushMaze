PROG = client
LIBS = ../support/support.a ../libcs50/libcs50.a

CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../libcs50 -I../support -fstack-protector
LDFLAGS = -lm -lncurses
MAKE = make

$(PROG): $(PROG).c
	$(CC) $(CFLAGS) $^ $(LIBS) $(LDFLAGS) -o $@

.PHONY: all clean test

all: $(PROG)

test:
	bash -v testing.sh

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	rm -f *.log

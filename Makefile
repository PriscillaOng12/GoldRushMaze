# Makefile for game
#
# ctrl-zzz, Winter 2024
# 
MAKE = make

.PHONY: all

all: 
	make -C client
	make -C server

clean:
	rm -f *~ *.o *.dSYM
	rm -f $(PROG)
	(cd ./client && make clean)
	(cd ./server && make clean)

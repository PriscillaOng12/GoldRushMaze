# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -Wextra -std=c11 -I../support

LIBS = ../support/log.c ../support/message.c
# Source files
SOURCES = file.c grid.c player.c mem.c
GRIDTESTSRC = file.c grid.c player.c gridtest.c mem.c
PLAYERTESTSRC = file.c grid.c player.c playertest.c mem.c
# Object files
OBJECTS = $(SOURCES:.c=.o)
# Executables
EXECUTABLES = gridtest playertest

# Main target
all: $(EXECUTABLES)

# Compile object files
$(OBJECTS): $(SOURCES)

gridtest.o: $(GRIDTESTSRC)

playertest.o: $(PLAYERTESTSRC)
# Compile executables
gridtest: $(OBJECTS) $(LIBS) gridtest.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ -lm

playertest: $(OBJECTS) playertest.o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ -lm

server: server.o $(OBJECTS) $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ -lm

# Clean target
clean:
	rm -f $(EXECUTABLES) $(OBJECTS)

# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -Wextra -std=c11 -I../support

LIBS = ../support/support.a
# Source files
SOURCES = file.c grid.c player.c
GRIDTESTSRC = file.c grid.c player.c gridtest.c
PLAYERTESTSRC = file.c grid.c player.c playertest.c
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
gridtest: $(OBJECTS) $(LIBFILES)
	$(CC) $(CFLAGS) $^ gridtest.o $(LIBS) -o $@ -lm

playertest: $(OBJECTS)
	$(CC) $(CFLAGS) $^ playertest.o $(LIBS) -o $@ -lm

# Clean target
clean:
	rm -f $(EXECUTABLES) $(OBJECTS)

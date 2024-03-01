# # Compiler
# CC = gcc
# # Compiler flags
# CFLAGS = -Wall -Wextra -std=c11 -I../support

# LIBS = ../support/log.c ../support/message.c
# # Source files
# SOURCES = file.c grid.c player.c mem.c spectator.c
# GRIDTESTSRC = file.c grid.c player.c spectator.c gridtest.c mem.c
# PLAYERTESTSRC = file.c grid.c player.c spectator.c playertest.c mem.c
# # Object files
# OBJECTS = $(SOURCES:.c=.o)
# # Executables
# EXECUTABLES = gridtest playertest

# # Main target
# all: $(EXECUTABLES)

# # Compile object files
# $(OBJECTS): $(SOURCES)

# gridtest.o: $(GRIDTESTSRC)

# playertest.o: $(PLAYERTESTSRC)
# # Compile executables
# gridtest: $(OBJECTS) $(LIBS) gridtest.o
# 	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ -lm

# playertest: $(OBJECTS) playertest.o
# 	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ -lm

# server: server.o $(SOURCES) $(LIBS)
# 	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ -lm

# # Clean target
# clean:
# 	rm -f $(EXECUTABLES) $(OBJECTS)


CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I../support -ggdb

# Libraries are compiled into object files, not included directly in the final link command
LIBS_O = ../support/log.o ../support/message.o

# Source files
SOURCES = file.c grid.c player.c mem.c spectator.c
# For executables, don't repeat common source files in the test source variable definitions
GRIDTESTSRC = gridtest.c
PLAYERTESTSRC = playertest.c

# Object files for the main sources and for the test sources
OBJECTS = $(SOURCES:.c=.o)
GRIDTESTOBJ = $(GRIDTESTSRC:.c=.o)
PLAYERTESTOBJ = $(PLAYERTESTSRC:.c=.o)

# Executables
EXECUTABLES = gridtest playertest

# Main target
all: $(EXECUTABLES)

# Compile support library object files
../support/%.o: ../support/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile object files for the main sources
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile executables
gridtest: $(OBJECTS) $(GRIDTESTOBJ) $(LIBS_O)
	$(CC) $(CFLAGS) $^ -o $@ -lm

playertest: $(OBJECTS) $(PLAYERTESTOBJ) $(LIBS_O)
	$(CC) $(CFLAGS) $^ -o $@ -lm

server: server.o $(OBJECTS) $(LIBS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@ -lm

# Clean target
clean:
	rm -f $(EXECUTABLES) $(OBJECTS) ../support/*.o *.o

# CS50 Nuggets Project
## CS50 Winter 2023

### Client

### User interface

The user interfaces first through the command line where the input is in the form:
``` 
./server map.txt [seed]
```
Assuming success, the server module will print out its port number. A client can join the game with the following syntax in a separate terminal window:
```
./client hostname port [playername]
```
`hostname` is the server IP

`port` is the port number that the server expects messages from

`playername` is optional, but if provided, the client will be in player mode. Otherwise, the client joins the game in spectator mode.

In player mode, the user interfaces with the program through keystrokes (specifics provided in the Requirements specifications).

In spectator mode, the client does not interface with the program, but rather watches as a player, or players, move around the map.'


### Assumptions

No assumptions beyond those that are clear from the specifications are made in the Nuggets game.


### Files

More information on each component in the program is available in the `.h` files, as well as in the `DESIGN.md` and `IMPLEMENTATION.md` files.
* `Makefile` - compilation procedure
* `client.c` - the implementation of the player/spectator logic
* `client.h` - the interface of client


### Compilation


To compile, simply type `make` in command line.


### Unit Testing

`testing.sh` covers unit tests of  `client` with shared server at `cs50-dev/shared/nuggets/linux/server` and `main.txt` map.

To run unit tests with `testing.sh`:
1. Run `cs50-dev/shared/nuggets/linux/server cs50-dev/shared/nuggets/linux/maps/main.txt`. The shared server module will print a port number.
2. Change the `SERVER_PORT` number in in testing.sh to the port number printed
2. In a separate terminal, run `./testing.sh/`

### Testing


To test, `make test` in the `server` directory which will print out the port number that the client shall connect to. Run the `client` module with that port number and the appropriate hostname that the server is being run on. Pass a player name to join the game in play mode. Otherwise, join the game as a spectator. Collect all the gold. 
 
To test with valgrind, simply `make valgrind` in the server directory and play as detailed above.



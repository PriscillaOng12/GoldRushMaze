# CS50 Nuggets
## Implementation Spec
### ctrl-zzz, Winter 2024

> This **template** includes some gray text meant to explain how to use the template; delete all of them in your document!

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes grid, player, and spectator modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

- Edward will handle the server setup, focusing on communication. He'll cover testing and documentation for this.
- Edward will write the grid module. She'll cover testing and documentation for this.
- Priscilla will handle the client, and working with ncurses. She'll cover testing and documentation for this.
- Edward will write the player and spectator modules. She'll cover testing and documentation for this.

## Client

> Teams of 3 students should delete this section.

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	initialize message module
	print assigned port number
	decide whether spectator or player

---

## Server

### Data structures

> For each new data structure, describe it briefly and provide a code block listing the `struct` definition(s).
> No need to provide `struct` for existing CS50 data structures like `hashtable`.

### Definition of function prototypes

> For function, provide a brief description and then a code block with its function prototype.
> For example:

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```
### Detailed pseudo code

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> For example:

#### `parseArgs`:

	validate commandline
	verify map file can be opened for reading
	if seed provided
		verify it is a valid seed number
		seed the random-number generator with that seed
	else
		seed the random-number generator with getpid()

---

## Grid module

### Data structures
Grid:
- Implemented as a 2-D array data structure
- Each cell holds information about the type of spot (boundary, hallway, nugget) and what is occupying the cell (either player or nugget pile).

Player:
- List with player's letter corresponding to index
- Hold pointers to Player structs (defined later), which contains information such as player's letter symbol, name location and nuggets in purse

### Definition of function prototypes
`grid_t* grid_load(char* filename)`: Loads the map file and initializes the game grid.

`int grid_init_gold(grid_t* grid)`: Drops gold piles on the grid at the start of the game.

`void grid_spawn_player(grid_t* grid)`: Randomly places a player onto a spot on the grid.

`void grid_spawn_spectator(grid_t* grid)`: Places a spectator.

`void grid_send_state(grid_t* grid, player_t* player)`: Sends the current game state to a player client.

`void grid_send_state(grid_t* grid, spectator_t* spectator)`: Sends the current game state to a spectator client.

`void grid_game_over(grid_t* grid)`: Handles the end of the game by preparing a summary and cleaning up.

`void grid_delete(grid_t* grid)`: Cleans up and deallocates memory associated with grid.

### Detailed pseudo code
#### `grid_t* grid_load(char* filename)`:
	Read the map file line by line
	Parse each character in the file to form the grid

#### `int grid_init_gold(grid_t* grid)`:
	Calculate the number of gold piles to be dropped within a certain area
	Iterate through the room spots to randomly 
Drop gold nugget piles with at least 1 nugget per pile
Update the grid to reflect the gold piles 

#### `void grid_spawn_player(grid_t* grid)`:
	Calculate random spot on the grid
	Place new player with new symbol
If player is already in spot
		Calculate another random spot

#### `void grid_spawn_spectator(grid_t* grid)`:
	Calculate random spot on the grid
	Place new spectator 
	If spectator is already present
		Kick them off and replace

#### `void grid_send_state(grid_t* grid, player_t* player)`:
	Create a message about current game state
	Send the message to the specified player client.
	
#### `void grid_send_state(grid_t* grid, spectator_t* spectator)`:
	Create a message about current game state
	Send the message to the specified spectator client.

#### `void grid_game_over(grid_t* grid)`:
	Iterate over each player 
	Calculate the final score 
	Create summary containing purse contents, score and name
	Send game summary to all clients
	Terminate game
	
#### `void grid_delete(grid_t* grid)`:
	Deallocate memory associated with the game grid 

---

## Testing plan

### unit testing

> How will you test each unit (module) before integrating them with a main program (client or server)?

### integration testing

> How will you test the complete main programs: the server, and for teams of 4, the client?

### system testing

> For teams of 4: How will you test your client and server together?

---

## Limitations

> Bulleted list of any limitations of your implementation.
> This section may not be relevant when you first write your Implementation Plan, but could be relevant after completing the implementation.

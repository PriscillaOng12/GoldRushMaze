# CS50 Nuggets
## Design Spec
### ctrl-zzz, Winter 2024

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes Grid, Player, and Spectator modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Client

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

See the requirements spec for both the command-line and interactive UI.

### Inputs and outputs

Inputs: The player will have eight choices of keys for the eight directions they can move (cardinal directions + diagonals), and capital letters to move as far as possible in a direction, and one more to quit the program, making a total of 8+8+1=17 valid keystrokes. See the requirements spec for specific keys.

Output: Graphical display of visible map, and current status of game (e.g. how many players, how many nuggets)

We log all communication messages originating from the player (along with destination) and debug messages to a log file log.txt.

### Functional decomposition into modules

We use no other modules, aside from ncurses to display the server output.

### Pseudo code for logic/algorithmic flow

> For each function write pseudocode indented by a tab, which in Markdown will cause it to be rendered in literal form (like a code block).
> Much easier than writing as a bulleted list!
> See the Server section for an example.

> Then briefly describe each of the major functions, perhaps with level-4 #### headers.

### Major data structures

> A language-independent description of the major data structure(s) in this program.

> Mention, but do not describe, any libcs50 data structures you plan to use.

---

## Server
### User interface

See the requirements spec for the command-line interface.
There is no interaction with the user.

### Inputs and outputs

> Briefly describe the inputs (map file) and outputs (to terminal).
Inputs: Map file
Outputs: Message to terminal 

We log all communication messages originating from the server (along with destination) and debug messages to a log file log.txt.

### Functional decomposition into modules

#### Grid 
The grid module helps to manage the grid environment by loading the map, initializing the position of gold piles and player positions, updating game states, detecting gold nugget collection, writing player gameplay summaries and deallocating resources at the conclusion of the game.

#### Player 
The player module stores information for each player, such as position, visible cells, connection info, player name, nugget "purse", etc. It supports movement and nugget collection.

#### Spectator
The spectator mode is a simpler, but largely parallel, module to the Player, which stores connection info.


### Pseudo code for logic/algorithmic flow

The server will run as follows:
	validate arguments
	call grid_load(), loading map file and initializing grid
	initialize host
	keep waiting and processing messages until game over

#### process_message(): processes messages from the clients and sends back messages when necessary


### Major data structures
We utilize a grid struct, which uses a 2-D array to store player position information and nugget information as well. We also use a player struct which stores information about the player like the player's letter. We also [FOR EXTRA CREDIT] store a hashmap which caches visibility.

## Grid module

### Functional decomposition

grid_t* grid_load(): Loads the map file and initializes the game grid.
int grid_init_gold(): Drops gold piles on the grid at the start of the game.
void grid_spawn_player(): Randomly places a player onto a spot on the grid.
void grid_spawn_spectator(): Places a spectator on the grid.
void grid_send_state(player_t* player): Sends the current game state to a player client.
void grid_send_state(spectator_t* spectator): Sends the current game state to a spectator client.
void grid_game_over():Handles the end of the game by preparing a summary and cleaning up.
void grid_delete(): Cleans up and deallocates memory associated with grid.

### Pseudo code for logic/algorithmic flow

#### grid_t* grid_load():
	Read the map file line by line
	Parse each character in the file to form the grid

#### int grid_init_gold():
	Calculate the number of gold piles to be dropped within a certain area
	Iterate through the room spots to randomly 
Drop gold nugget piles with at least 1 nugget per pile
Update the grid to reflect the gold piles 

#### void grid_spawn_player():
	Calculate random spot on the grid
	Place new player with new symbol
If player is already in spot
		Calculate another random spot

#### void grid_spawn_spectator():
	Calculate random spot on the grid
	Place new spectator 
	If spectator is already present
		Kick them off and replace

#### void grid_send_state(player_t* player):
	Create a message about current game state
	Send the message to the specified player client.
	
#### void grid_send_state(spectator_t* spectator):
	Create a message about current game state
	Send the message to the specified spectator client.

#### void grid_game_over():
	Iterate over each player 
	Calculate the final score 
	Create summary containing purse contents, score and name
	Send game summary to all clients
	Terminate game
	
#### void grid_delete():
	Deallocate memory associated with the game grid 
	
### Major data structures
Grid:
- Implemented as a 2-D array data structure
- Each cell holds information about the type of spot (boundary, hallway, nugget) and what is occupying the cell (either player or nugget pile).

Player:
- List with player's letter corresponding to index
- Hold pointers to Player structs (defined later), which contains information such as player's letter symbol, name location and nuggets in purse

## Player module

A module that handles the player's data: their name, position, purse, and the cells they can see. It handles player movement, gold collection, visibility updates, and player quit/dropoff.

### Functional decomposition

#### `player_t* player_new(char* name, char* conn_info, int x, int y)`
Initializes a new player with specified name, connection info and position. Returns pointer to `player` struct.

#### `void player_move(player_t* player) `
Updates the player's position and the location of conflicting players, calling `update_visibility` on all affected players, and sending a `DISPLAY` message to all clients with updated locations.

#### `void player_collect_gold(player_t* player, int gold_x, int gold_y)`
Updates the player's purse and the location of the gold piles, and sends a `GOLD` message to all clients with updated locations.

#### `void player_update_visibility(player_t* player)`
Updates the player's visibility layer. Called by `player_move`.

#### `void player_quit(player_t* player, grid_t* grid)`
Called by grid module when a player quits. Gracefully handles quit, then calls `player_delete`.

#### `player_delete(player_t* player)`
Deallocates memory associated with the player.

### Pseudo code for logic/algorithmic flow

#### `player_t* player_new(char* name, char* conn_info, int x, int y)`
Trivial constructor for a new player.

#### `void player_move(player_t* player, int dx, int dy, grid_t* grid) `
```
    if (player->x + dx, player->y + dy) is a valid move:
        if there is a player in the new position:
            call player_move on the other player with the opposite dx and dy
            call update_visibility on the other player
        update player's position
        call update_visibility on the player
        if there is a gold pile in the new position:
            call player_collect_gold
        send a DISPLAY message to all clients with updated locations
    else:
        do nothing
```

#### `void player_collect_gold(player_t* player, int gold_x, int gold_y, grid_t* grid)`
```
    update player's purse
    update gold pile's value in grid
    decrement number of remaining gold piles in grid
    send a GOLD message to all clients with updated locations
```
#### `void player_update_visibility(player_t* player, grid_t* grid, hashmap_t* cache)`
This function is cached **[FOR EXTRA CREDIT]** in a hashmap: if **any** previous player has been at the spot, we store all cells that are visible from that spot. If not, we compute the visibility layer from scratch and cache it.
```
    x, y = player's position
    if (num_rows * x + y) is already cached:
        player->visibility = player_visibility OR cached // element-wise OR
    vis_cache = 2d array size of grid, initialized to 0s
    for each cell (i, j) in the grid
        compute functions get_y(x), get_x(y) for the line (point-slope)
        visible = True
        for p in [x, i]:
            if (p, floor(get_y(p))) and (p, ceil(get_y(p))) are both walls:
                visible = False
        for q in [y, j]:
            if (floor(get_x(q)), q) and (ceil(get_x(q)), q) are both walls:
                visible = False
        if visible:
            player->visibility[i][j] = 1
            cached[i][j] = 1
    cached[num_rows * x + y] = vis_cache
    
```

#### `void player_quit(player_t* player, grid_t* grid)`
Trivial function that removes player from grid, then calls `player_delete`.
#### `player_delete(player_t* player)`
Trivial function to delete.

### Major data structures

The only extra data structure is the visibility layer, which is a 2D Boolean array.

## Spectator module

A module that handles the spectator data: their connection info. It handles sending map data to spectator, and spectator quit/dropoff.

### Functional decomposition

#### `spectator_t* spectator_new(char* conn_info)`
Initializes a new spectator with specified connection info. Returns pointer to `spectator` struct.

#### `void spectator_get_map(spectator_t* spectator, grid_t* grid)`
Constructs and sends display message to spectator.

#### `void spectator_quit(spectator_t* player)`
Called by grid module when a player quits. Sends a `QUIT` message to server, and calls `spectator_delete`.

#### `spectator_delete(spectator_t* spectator)`
Deallocates memory associated with the spectator.

### Pseudo code for logic/algorithmic flow

#### `spectator_t* spectator_new(char* conn_info)`
Initializes a new spectator with specified connection info. Returns pointer to `spectator` struct.

#### `void spectator_get_map(spectator_t* spectator, grid_t* grid)`
Constructs and sends display message to spectator.

#### `void spectator_quit(spectator_t* spectator)`
Trivial function, which gracefully handles

#### `spectator_delete(spectator_t* spectator)`
Trivial function to deallocate memory associated with the spectator.

### Major data structures

No specific data structures are used.

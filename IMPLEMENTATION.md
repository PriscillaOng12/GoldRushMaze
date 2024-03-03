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
- Eliana will write the grid module. She'll cover testing and documentation for this.
- Priscilla will handle the client, and working with ncurses. She'll cover testing and documentation for this.
- Karun will write the player and spectator modules. She'll cover testing and documentation for this.

## Client

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

A struct to store nugget positional information and a list of current players.

```c
typedef struct grid {
  int[][] grid_array;
  player_t* current_players[26];
  spectator_t* current_spectator;
  int gold_remaining;
} grid_t;
```

A struct to store information about the player such as name and position.

```c
typedef struct player {
  char letter;
  char* name;
  int position_x;
  int position_y;
  int current_nuggets;
  char* player_address;
  int[][] player_visibility;
} player_t;
```

### Definition of function prototypes

A function to parse the command-line arguments, initialize the game struct, initialize the message module, and (BEYOND SPEC) initialize analytics module.

```c
static int parseArgs(const int argc, char* argv[]);
```

A function to handle messages from the client
```c
static bool handleMessage(void* arg, const addr_t from, const char* message);
```

### Detailed pseudo code

#### `main`:

  validate arguments with parseArgs()
  initialize message module and validate
  set message address and validate
  call message_loop() with server, 0, NULL, NULL, handleMessage


#### `parseArgs`:

	validate commandline
	verify map file can be opened for reading
	if seed provided
		verify it is a valid seed number
		seed the random-number generator with that seed
	else
		seed the random-number generator with getpid()

---

#### `handleMessage`:

  if first word of message is PLAY:
    set `real_name` variable to rest of message
    if the size of players list in grid is already MaxPlayers:
      send message saying game is full
      return false
    if `real_name` is empty:
      send message saying name must be provided
      return false
    truncate `real_name` to MaxNameLength
    replace any character in `real_name` that is both isgraph() and isblank() to `_`
    call grid_spawn_player() with player information
    send `GRID nrows ncols` message using grid information
    send initial `GOLD n p r` message using grid information
    call grid_send_state() with player as argument which sends DISPLAY message
    send message OK followed by new player's assigned letter
    return false
  else if first word of message is KEY:
    find correct player message is coming from by calling message_eqAddr() on each player
    switch (keystroke after KEY):
      case h:
        call player_move(-1, 0)
      case l:
        call player_move(1, 0)
      case j:
        call player_move(0, -1)
      case k:
        call player_move(0, 1)
      case y:
        call player_move(-1, 1)
      case u:
        call player_move(1, 1)
      case b:
        call player_move(-1, -1)
      case n:
        call player_move(1, -1)
      case Q:
        if address not in list of players:
          send message `QUIT Thanks for watching!`
          return false
        send `QUIT Thanks for playing`
        return false
    if no gold remaining in grid:
      call grid_game_over()
      return true
  else if first word is SPECTATE:
    call grid_spawn_spectator()
    send `GRID nrows ncols` message using grid information
    send initial `GOLD 0 0 r` message using grid information
    call grid_send_state() with spectator as argument which sends DISPLAY message
    return false

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

`void grid_spawn_player(grid_t* grid, addr_t* connection_info, char* real_name)`: Randomly places a player onto a spot on the grid.

`void grid_spawn_spectator(grid_t* grid, spectator_t* spectator)`: Places a spectator and ensures only one spectator is present on the grid at one time.

`void grid_send_state(grid_t* grid, player_t* player)`: Sends the current game state to a player client.

`void grid_send_state_spectator(grid_t* grid, spectator_t* spectator)`: Sends the current game state to a spectator client.

`void grid_game_over(grid_t* grid)`: Handles the end of the game by preparing a summary and cleaning up.

** Getter functions included as helper functions to faciliatate easy integration. 

### Detailed pseudo code
#### `grid_t* grid_load(char* filename)`:
	Allocate memory for a new grid structure
    Allocate and initialize memory for storing the number of rows and columns in the grid
    Initialize variables for counting rows, columns, and tracking the maximum number of columns
    Read through the file character by character:
        If a newline character is encountered:
            Increment the row count
            Update the maximum number of columns if the current row's column count is greater
            Reset the column count for the next row
        Otherwise, increment the column count for the current row
    If the file does not end with a newline:
        Treat the last line as a row, increment the row count, and update the maximum column count if necessary
    Assign the calculated number of rows and maximum column count to the grid
    Allocate memory for the grid's cell and nugget arrays:
        For each row, allocate memory for its cells and initialize them
        For each row, allocate memory for nugget counts and initialize them to 0
    Rewind the file to the beginning
    Read the file again to fill in the grid's cells:
        For each character in the file that is not a newline, assign it to the appropriate cell
        Fill in any remaining cells in a row with a default character if the row is shorter than the maximum column count
    Allocate and initialize additional properties of the grid (player count, nugget count, spectator count, arrays for players and spectators)
    Return the initialized grid

#### `int grid_init_gold(grid_t* grid)`:
	  Initizlize gold total, min and max number of piles
    Initialize ndots to 0
    For each cell in the grid:
        If cell is empty ('.'):
            Increment ndots
    If ndots is less than GoldMinNumPiles:
        Print error message "Error: too few spots to place gold"
        Exit the program
    Calculate numPiles, ensuring it's between GoldMinNumPiles and the lesser of GoldMaxNumPiles or ndots
    Initialize an array piles[numPiles] to keep track of gold nuggets in each pile
    Distribute GoldTotal nuggets across the piles randomly
    For each pile in piles:
        Repeat until a pile is placed:
            Randomly select a grid cell (x, y)
            If the cell is empty and has no nuggets:
                Assign the number of nuggets in the current pile to this cell
                Break the loop
    Set the total nugget count in the grid to numPiles

#### `void grid_spawn_player(grid_t* grid, addr_t* connection_info, char* real_name)`:
	Randomly select a cell (until cell words)
		If the cell is empty ('.'), place a new player there.
		Update the player's visibility on the grid.
		Increment the grid's player count.

#### `void grid_spawn_spectator(grid_t* grid, spectator_t* spectator)`:
	Place new spectator 
	If spectator is already present
		Kick them off and replace

#### `void grid_send_state(grid_t* grid, player_t* player)`:
	Allocate memory for a message string.
	Prepare a visibility map for the player.
	Iterate over the grid, adding visible elements to the message string based on the player's visibility and the grid's state.
	Free memory used for the visibility map.
	Return the message string.
	
#### `void grid_send_state_spectator(grid_t* grid, spectator_t* spectator)`:
	If there is a spectator, prepare a complete view of the grid for them.
	Allocate memory for a message string and populate it with the grid state.
	If the spectator has a valid address, send them the message; otherwise, print the message.
	Free memory used for the message preparation.
	Return the message string.

#### `void grid_game_over(grid_t* grid)`:
	Allocate memory for a message string to summarize the game outcome.
	Iterate over each player, compiling their score and other details into the message.
	Send the summary message to each active player or print it.
	Delete each player and then the grid itself.
	Free memory used for the message preparation.
	
#### `void grid_delete(grid_t* grid)`:
	Free memory allocated for grid cells and nuggets.
	Free memory for other grid properties like player count, rows, columns, etc.
	Finally, free the grid structure itself.

## Player module

### Data structures

The only extra data structure is the visibility layer, which is a 2D Boolean array.

### Definition of function prototypes

`player_t* player_new(addr_t* connection_info, char* real_name, int x, int y, int nrows, int ncols)`: Initializes a new player with specified name, connection info and position. Returns pointer to `player` struct.

`bool player_move(player_t* player, grid_t* grid, int dx, int dy) `: Updates the player's position and the location of conflicting players, calling `update_visibility` on all affected players, and sending a `DISPLAY` message to all clients with updated locations.

`void player_collect_gold(player_t* player, grid_t* grid, int gold_x, int gold_y)`: Updates the player's purse and the location of the gold piles, and sends a `GOLD` message to all clients with updated locations.

`void player_update_visibility(player_t* player, grid_t* grid)`: Updates the player's visibility layer. Called by `player_move`.

`void player_quit(player_t* player)`: Called by grid module when a player quits. Gracefully handles quit, then calls `player_delete`.

`player_delete(player_t* player, grid_t* grid)`: Deallocates memory associated with the player.

** Getter and setter functions included as helper functions to faciliatate easy integration. 

### Detailed pseudo code

#### `player_t* player_new(addr_t* connection_info, char* real_name, int x, int y, int nrows, int ncols)`
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

#### `void player_collect_gold(player_t* player, grid_t* grid, int gold_x, int gold_y, grid_t* grid)`
```
    update player's purse
    update gold pile's value in grid
    decrement number of remaining gold piles in grid
    send a GOLD message to all clients with updated locations
```
#### `void player_update_visibility(player_t* player, grid_t* grid)`
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
[Note: this algorithm runs in O(mn(m+n)) time, where the grid has size (m,n). We could optimize to O(mn) at the cost of simplicity, using trigonometry.]

#### `void player_quit(player_t* player)`
Trivial function that removes player from grid, then calls `player_delete`.
#### `player_delete(player_t* player),grid_t* grid`
Trivial function to delete.

## Spectator module

### Data structures
No specific data structures are used.

### Definition of function prototypes
`spectator_t* spectator_new(addr_t* connection_info)`: Initializes a new spectator with specified connection info. Returns pointer to `spectator` struct.

`addr_t* spectator_get_addr(spectator_t* spectator)`: Get the address of a spectator.

`void spectator_quit(spectator_t* player, grid_t* grid)`: Called by grid module when a player quits. Sends a `QUIT` message to server, and calls `spectator_delete`.

`spectator_delete(spectator_t* spectator)`: Deallocates memory associated with the spectator.

### Detailed pseudo code

#### `spectator_t* spectator_new(addr_t* connection_info)`
Initializes a new spectator with specified connection info. Returns pointer to `spectator` struct.

#### `addr_t* spectator_get_addr(spectator_t* spectator)`
Constructs and sends display message to spectator.

#### `void spectator_quit(spectator_t* player, grid_t* grid)`
Get the address of a spectator.

#### `spectator_delete(spectator_t* spectator)`
Trivial function to deallocate memory associated with the spectator.

---

## Testing plan

### Unit testing

`Grid Module`: Test loading from various map files, including edge cases like empty or exceptionally large maps. Check for proper initialization of gold and validation of grid dimensions. Test each grid manipulation function (e.g., grid_spawn_player, grid_init_gold) in isolation to ensure they behave as expected under different grid states.

`Player Module`: Create mock player objects and test functions such as player_move, player_collect_gold, and player_update_visibility. Use predefined grid states to ensure player movements and interactions are correctly handled.

`Spectator Module`: Test spectator creation and ensure that the spectator can correctly receive game state updates. Test the behavior when switching spectators.


### Integration testing

`Server`: Test the server's ability to handle multiple connections, manage game state, and correctly process and respond to messages from clients. This includes testing game initialization, player movements, gold collection, and spectator updates.

`Client`: Test the client's ability to parse server messages, update the local display correctly, and send appropriate responses back to the server. Verify the client's handling of different game states and transitions.

### system testing

`Client and Server Together`: Test the complete system by running the server and connecting multiple clients, including both players and spectators. Evaluate the system's performance in real-time gameplay, ensuring synchronization between server and clients, and verifying the correct game logic execution. Test scenarios include starting a game, player movements and interactions, gold collection, spectator viewing, and game termination.

---

## Limitations
`Spectator Limit`: Only one spectator is allowed at a time, limiting the number of people who can view the game state without participating

`Client Interface`: The client interface, particularly for handling user inputs and displaying the game state, may not be optimized for usability or accessibility.

`Scalability`: The system may not efficiently scale to support a very large number of players or spectators simultaneously due to limitations in server processing and network bandwidth.

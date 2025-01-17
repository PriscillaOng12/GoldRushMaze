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

The user will see a CURSES display, the top line of which shall provide game status, and the rest of the rows will display the game state from the client’s perspective.

The user interfaces with the game by sending keystrokes input through stdin.

In player mode, the user interfaces with the program through keystrokes (specifics provided in the Requirements specifications).

In spectator mode, the client does not interface with the program, but rather watches as a player, or players, move around the map.'

For more details, see the requirements spec for both the command-line and interactive UI.

### Inputs and outputs

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

**Spectator Inputs (Keystrokes)**

All keystrokes are sent immediately to the server.
The only valid keystroke for the spectator is

* `Q` quit the game.

**Player Inputs (keystrokes)**

The player will have eight choices of keys for the eight directions they can move (cardinal directions + diagonals), and capital letters to move as far as possible in a direction, and one more to quit the program, making a total of 8+8+1=17 valid keystrokes. See the requirements spec for specific keys.

**Outputs**

The output display will depend on the client mode. In **spectator mode**, the entire grid is displayed, showing all the gold to be collected and all the players. In **player mode**, only parts of the map and the contained information (i.e. nuggets and players) are displayed, according to visibility. It updates as the player(s) gathers the nuggets and as the player moves around the map, changing the visibility of the player.

**Logging**

We log all communication messages originating from the player (along with destination) and debug messages to a log file log.txt.

### Function prototypes
```c
static bool handleMessage(void* arg, const addr_t from, const char* message);
static bool handleInput(void* arg);
static void cursesInit(); 
static void setupWindow();
static void displayMap(char* display);
static void displayTempMessage(const char* temp);
static void clearTempMessage();
static bool gameOk(const char* message);
static bool gameGrid(const char* message);
static bool gameGold(const char* message);
static void gameDisplay(const char* message);
static localclient_t* data_new();
```
 
### Function definitions and Pseudo code for logic/algorithmic flow

#### Overview
The client will run as follows:
```
	Validate that either 3 or 4 arguments were passed in the command line
	Verify hostname (or IP address) and port number and determine player or spectator mode
	Initialize display
	Initialize network and join game with a `PLAY` or `SPECTATE` message to server, accordingly
	Receive `GRID`, `GOLD`, and `DISPLAY` messages from server
	Calls handleMessage() to appropriately react to each of these messages
	If player mode
			Wait for input
	Call handleInput which sends keystrokes to the server
	If spectator mode
		Display entire grid
		Wait for `GOLD` and `DISPLAY` messages and continuously update the display as players move and collect gold. 
	Calls handleMessage() to parse message and update display accordingly
```


#### `int main(int argc, char* argv[])`:
The `main` function calls `parseArgs` and other functions.
```
	Start
	Check if the number of command-line arguments is either 3 or 4
		If not, print error message and exit with code 1
	For each argument starting from the first (ignoring the program name itself):
		If any argument is NULL, print an error message and exit with code 2
	Initialize the data structure
		If initialization fails, print an error, free resources, and exit with code 3
	Prepare the server address using the provided hostname and port
		If address preparation fails, print an error, free resources, and exit with code 4
	If a player name is provided (indicating 4 arguments were passed):
		Send "PLAY <playername>" message to the server to connect as a player
		If sending fails, send a "KEY Q" message to quit, clean up, and exit with code 6
	Else (only hostname and port were provided):
		Send "SPECTATE" message to the server to connect as a spectator
		If connection fails, follow the same quit procedure as in step 6.2
	Enter a loop to handle input from the user and messages from the server
		If the loop exits indicating failure, proceed to cleanup
	Clean up resources:
		Free the data structure
		Close the curses window
		Shut down the message module
	Exit the program with 0 on success or 1 on failure from the loop
```


#### `static bool setupWindow(void)`:
`setupWindow` sets up a game window using the ncurse library with a given row and column size.
```
	initialize ncurses window 
	set window to nRows nColumns
	call cbreak
```

#### `static bool handleMessages(void* arg, const addr_t from, const char* message)`:
`handleMessage` parses the incoming message to interpret the type of message and displays information onto the game window Everytime a message is received from the server, this function is called to handle the message. The details of each received message (`QUIT`, `OK`, `GRID`, `GOLD`, `DISPLAY`, and `ERROR`) are explained further in the requirements specifications.
```
	check message for NULL
	parses the message
	if first word of message == OK:
		initialize player struct with playerID
	if first word of message == GRID:
		initialize size of grid
		call setupWindow
	if first word of message == GOLD:
		if client is player:
			update players gold value
			update display message
		if client is spectator:
			update how much gold is left
	if first word of message == DISPLAY:
		call printDisplay to load display and update from message
	if first word of message == QUIT:
		end loop
	if first word of message == ERROR:
		print error message
```


#### `static bool handleInput(void* arg)`;
`handleInput` waits for keyboard input from the user and sends a corresponding message to the server.
```
    Check if serverPointer is null or invalid
        If true, print error and exit function with true

    While character input is not 'q':
        Create and send "KEY " message with input character to server

    Exit and return true if 'q' is pressed, otherwise keep looping
```



### Major data structures
Create a `localclient_t data` structure in order to hold information for each of the players that join the client.

This localclient data structure keeps track of the player’s characteristics such as playerID, purse, and the amount of gold remaining. It also keeps track of the size of the window, whether the player is a spectator, and the port number.

---

## Server
### User interface

See the requirements spec for the command-line interface.
There is no interaction with the user.

### Inputs and outputs

Inputs: Map file
Outputs: Message to terminal, and messages to clients.

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

`void process_message(char* message)`: processes messages from the clients and sends back messages when necessary


### Major data structures
We utilize a grid struct, which uses a 2-D array to store player position information and nugget information as well. We also use a player struct which stores information about the player like the player's letter. We also [FOR EXTRA CREDIT] store a hashmap which caches visibility.

## Grid module

### Functional decomposition

`grid_t* grid_load(char* filename)`: Loads the map file and initializes the game grid.

`int grid_init_gold(grid_t* grid)`: Drops gold piles on the grid at the start of the game.

`void grid_spawn_player(grid_t* grid, addr_t connection_info, char* real_name)`: Randomly places a player onto a spot on the grid.

`void grid_spawn_spectator(grid_t* grid, spectator_t* spectator)`: Places a spectator and ensures only one spectator is present on the grid at one time.

`void grid_send_state(grid_t* grid, player_t* player)`: Sends the current game state to a player client.

`void grid_send_state_spectator(grid_t* grid, spectator_t* spectator)`: Sends the current game state to a spectator client.

`void grid_delete(grid_t* grid)`: Frees the grid and allocated memory/resources required for it. 

`void grid_game_over(grid_t* grid)`: Handles the end of the game by preparing a summary and cleaning up.

** Getter functions included as helper functions to faciliatate easy integration. 

### Pseudo code for logic/algorithmic flow

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
    Initialize an array piles to keep track of gold nuggets in each pile
    Distribute GoldTotal nuggets across the piles randomly
    For each pile in piles:
        Repeat until a pile is placed:
            Randomly select a grid cell (x, y)
            If the cell is empty and has no nuggets:
                Assign the number of nuggets in the current pile to this cell
                Break the loop
    Set the total nugget count in the grid to numPiles
	

#### `void grid_spawn_player(grid_t* grid, addr_t connection_info, char* real_name)`:
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

`player_t* player_new(addr_t connection_info, char* real_name, int x, int y, int nrows, int ncols)`: Initializes a new player with specified name, connection info and position. Returns pointer to `player` struct.

`bool player_move(player_t* player, grid_t* grid, int dx, int dy) `: Updates the player's position and the location of conflicting players, calling `update_visibility` on all affected players, and sending a `DISPLAY` message to all clients with updated locations.

`void player_collect_gold(player_t* player, grid_t* grid, int gold_x, int gold_y)`: Updates the player's purse and the location of the gold piles, and sends a `GOLD` message to all clients with updated locations.

`void player_update_visibility(player_t* player, grid_t* grid)`: Updates the player's visibility layer. Called by `player_move`.

`void player_quit(player_t* player)`: Called by grid module when a player quits. Gracefully handles quit, then calls `player_delete`.

`player_delete(player_t* player, grid_t* grid)`: Deallocates memory associated with the player.

** Getter and setter functions included as helper functions to faciliatate easy integration. 

### Pseudo code for logic/algorithmic flow

#### `player_t* player_new(addr_t connection_info, char* real_name, int x, int y, int nrows, int ncols)`
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

### Major data structures

The only extra data structure is the visibility layer, which is a 2D Boolean array.

## Spectator module

A module that handles the spectator data: their connection info. It handles sending map data to spectator, and spectator quit/dropoff.

### Functional decomposition

`spectator_t* spectator_new(addr_t connection_info)`: Initializes a new spectator with specified connection info. Returns pointer to `spectator` struct.

`addr_t* spectator_get_addr(spectator_t* spectator)`: Get the address of a spectator.

`void spectator_quit(spectator_t* player, grid_t* grid)`: Called by grid module when a player quits. Sends a `QUIT` message to server, and calls `spectator_delete`.

`spectator_delete(spectator_t* spectator)`: Deallocates memory associated with the spectator.

### Pseudo code for logic/algorithmic flow

#### `spectator_t* spectator_new(addr_t connection_info)`
Initializes a new spectator with specified connection info. Returns pointer to `spectator` struct.

#### `addr_t* spectator_get_addr(spectator_t* spectator)`
Constructs and sends display message to spectator.

#### `void spectator_quit(spectator_t* player, grid_t* grid)`
Get the address of a spectator.

#### `spectator_delete(spectator_t* spectator)`
Trivial function to deallocate memory associated with the spectator.

### Major data structures

No specific data structures are used.

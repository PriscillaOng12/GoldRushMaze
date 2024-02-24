# CS50 Nuggets
## Implementation Spec
### Team name, term, year

> This **template** includes some gray text meant to explain how to use the template; delete all of them in your document!

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes x, y, z modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

> Update your plan for distributing the project work among your 3(4) team members.
> Who writes the client program, the server program, each module?
> Who is responsible for various aspects of testing, for documentation, etc?

## Player

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

## XYZ module

> For each module, repeat the same framework above.

### Data structures

### Definition of function prototypes

### Detailed pseudo code

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

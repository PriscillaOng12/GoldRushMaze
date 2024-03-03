# CS50 Nuggets
## Client
### ctrl-zzz, Winter 2024

### Client

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


#### Function Prototypes
```c
int main(int argc, char* argv[]);
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


`main` Initializes the client application, processes command-line arguments for server connection details, and manages the game's main loop. This function sets up the client's environment, including initializing the data structure and the messaging system, and then continuously handles user input and server messages until the game concludes.
```c
int main(int argc, char* argv[]);
```


`handleMessage` Processes incoming messages from the server and dispatches them to appropriate handler functions based on the message type. This function is responsible for interpreting server commands such as game state updates, error messages, and game conclusion signals, ensuring that the client responds appropriately to each.
```c
handleMessage(void* arg, const addr_t from, const char* message);
static bool handleInput(void* arg);
```


`handleInput` Captures user input from the keyboard and sends corresponding actions to the server. This function is crucial for enabling player interaction with the game, allowing users to move around the map, collect gold, and communicate with other players or spectators.
```c
static bool handleInput(void* arg);
```


`cursesInit` Sets up the ncurses library for rendering the game interface. This function configures the terminal to support real-time input handling and graphical output, providing the foundation for the game's visual presentation.
```c
static void cursesInit(); 
```


`setupWindow` Prepares the initial game window, clearing any pre-existing content and setting it up for game display. This function is called after initializing curses to ensure that the game's graphical interface starts from a clean slate.
```c
static void setupWindow();
```


`displayMap` Renders the game map on the screen based on the latest layout received from the server. This function translates the server-provided map data into visual elements, updating the game window to reflect the current state of the game world.
```c
static void displayMap(char* display);
```


`displayTempMessage` Shows temporary messages, such as errors or notifications, at the top of the game window. This function ensures that important information is communicated to the player promptly without disrupting the game display.
```c
static void displayTempMessage(const char* temp);
```


`clearTempMessage` Removes any temporary messages currently displayed to the user. This function is typically called before showing a new message or updating the game window to keep the display uncluttered.
```c
static void clearTempMessage();
```


`gameOk` Handles the "OK" message from the server, which confirms successful communication or actions. This function might update the client state or UI to reflect the successful operation as indicated by the server.
```c
static bool gameOk(const char* message);
```


`gameGrid` Processes the "GRID" message containing the dimensions of the game map. This function updates the client's internal representation of the game world size, adjusting the display parameters accordingly.
```c
static bool gameGrid(const char* message);
```


`gameGold` Interprets the "GOLD" message from the server, which provides updates on gold collected by the player or remaining in the game. This function ensures that the player is kept informed about their progress and the state of the game.
```c
static bool gameGold(const char* message);
```


`gameDisplay` Updates the game window with the current map layout received from a "DISPLAY" message. This function ensures that the game map is accurately rendered on the user's screen, providing a visual representation of the game state.
```c
static void gameDisplay(const char* message);
```


`data_new` Allocates and initializes a new instance of the `localclient_t` data structure. This function sets up the foundational data model for the client, preparing it to handle and display the game state.
```c
static localclient_t* data_new();
```


### Assumptions


No assumptions beyond those that are clear from the specifications are made in the Nuggets game.


### Files


More information on each component in the program is available in the `.h` files, as well as in the `DESIGN.md` and `IMPLEMENTATION.md` files.
* `Makefile` - compilation procedure
* `client.c` - the implementation of the player/spectator logic
* `client.h` - the interface of client


### Compilation


To compile, simply type `make` in command line.


### Testing


To test, `make test` in the `server` directory which will print out the port number that the client shall connect to. Run the `client` module with that port number and the appropriate hostname that the server is being run on. Pass a player name to join the game in play mode. Otherwise, join the game as a spectator. Collect all the gold. 
 
To test with valgrind, simply `make valgrind` in the server directory and play as detailed above.

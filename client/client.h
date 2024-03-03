#ifndef CLIENT_H
#define CLIENT_H

#include "message.h" 

/**************** global types ****************/
typedef struct localclient {
  // size of board, initialized to initial window size
  int NROWS;
  int NCOLS;
  char player; 
} localclient_t;

/**************** local function  ****************/
// Functions for game communication handling
static bool handleMessage(void* arg, const addr_t from, const char* message);
static bool handleInput(void* arg);
static void initialize_curses(); 
static void setupWindow();
static void display_map(char* display);
static void display_temp_message(const char* temp);
static void clear_temp_message();

// Functions for handling specific types of messages
static bool gameOk(const char* message);
static bool gameGrid(const char* message);
static bool gameGold(const char* message);
static void gameDisplay(const char* message);

// game helper function
static localclient_t* data_new();


/**************** handleMessage ****************/
/**
 * @brief Handles incoming datagrams from the server and processes various game messages.
 * 
 * This function interprets and acts upon messages received from the game server, dispatching
 * each message to the appropriate handler function based on its type. It supports messages, each triggering specific actions within the client:
 * 
 * - "OK": Initializes the player character and sets up the game window.
 * - "GRID": Updates the game grid dimensions and verifies the window size can accommodate it.
 * - "GOLD": Displays the current gold status, including gold collected, purse, and remaining gold.
 * - "DISPLAY": Updates the ncurses window with the current game map or state.
 * - "QUIT": Handles game termination by closing the ncurses window and printing the quit message.
 * - "ERROR": Logs and displays error messages to the player.
 * - Malformed messages are logged as errors and ignored.
 * 
 * @param arg A void pointer, intended for future use or to carry additional data, but ignored in this implementation.
 * @param from The address of the server sending the message.
 * @param message The actual message received from the server.
 * @return Returns true if a "QUIT" message is received or a fatal error occurs, indicating the message loop should terminate. Otherwise, returns false, indicating normal operation and that the message loop should continue.
 *
 * The function uses defensive programming to ensure that neither the `arg` nor the `message`
 * parameters are NULL. On receiving specific messages, it may terminate the program by
 * calling `exit()` with an appropriate exit code if a critical failure occurs (e.g., failing to
 * process an "OK" or "GRID" message correctly).
 */

static bool handleMessage(void* arg, const addr_t from, const char* message);


/**************** handleInput ****************/
/**
 * @brief Reads a character from stdin and sends it as a "KEY" message to the server.
 * 
 * This function is designed to be called when stdin has input ready. It reads a single
 * character, packages it as a "KEY" message, and sends it to the server. It also clears
 * any temporary message displayed on the screen.
 * 
 * @param arg A void pointer to an addr_t structure representing the server's address.
 * @return Returns true to exit the message loop on EOF or fatal error, otherwise false.
 */
static bool handleInput(void* arg);


/**************** initialize_curses ****************/
/**
 * @brief Initializes the ncurses library for displaying the game interface.
 * 
 * This function sets up the ncurses environment for the game. It configures the screen
 * to not echo characters typed by the user, makes the cursor invisible, and sets the
 * default color pair to yellow text on a black background.
 */
static void initialize_curses(void);


/**************** setupWindow ****************/
/**
 * @brief Prepares the game window with initial settings.
 * 
 * Clears the screen and sets up the initial game window. This includes clearing any
 * temporary messages and filling the screen with blank spaces to prepare for game display.
 */
static void setupWindow(void);


/**************** display_map ****************/
/**
 * @brief Displays the game map in the ncurses window.
 * 
 * Takes the string representation of the game map and displays it in the ncurses window,
 * starting from the second row to leave space for status messages at the top.
 * 
 * @param display A string containing the map to be displayed.
 */
static void display_map(char* display);


/**************** display_temp_message ****************/
/**
 * @brief Displays a temporary status message at the top of the game window.
 * 
 * This function is used to display temporary status messages, such as error messages
 * from the server, at the top of the game window. It ensures that the message fits
 * within the window width.
 * 
 * @param temp The temporary message to be displayed.
 */
static void display_temp_message(const char* temp);


/**************** clear_temp_message ****************/
/**
 * @brief Clears any temporary message displayed at the top of the game window.
 * 
 * This function removes temporary status messages from the top of the game window,
 * restoring the display to its state before the message was shown.
 */
static void clear_temp_message(void);


/**************** gameOk ****************/
/**
 * @brief Processes the "OK" message from the server, initializing game state.
 * 
 * This function is called upon receiving an "OK" message from the server. It sets
 * the player character in the `data` structure and initializes the ncurses library
 * and game window. If no player character is specified in the message, the function
 * defaults the player character to 0.
 * 
 * @param message The message received from the server, expected to start with "OK".
 * @return Returns true if the "OK" message is processed successfully, otherwise false.
 */
static bool gameOk(const char* message);


/**************** gameGrid ****************/
/**
 * @brief Processes the "GRID" message from the server to set up the game grid.
 * 
 * This function extracts the grid dimensions from the "GRID" message and verifies
 * that the current window size can accommodate the grid. If the window is too small,
 * the function prompts the user to resize the window. The grid dimensions are then
 * stored in the `data` structure.
 * 
 * @param message The "GRID" message received from the server, containing grid dimensions.
 * @return Returns true if the window is successfully verified or resized, otherwise false.
 */
static bool gameGrid(const char* message);


/**************** gameGold ****************/
/**
 * @brief Processes the "GOLD" message from the server and displays gold status.
 * 
 * Extracts gold information from the "GOLD" message and updates the top line of the
 * ncurses window with the gold status. If the player collects gold, this status is
 * also updated. If the display line exceeds the window width, it is cropped to fit.
 * 
 * @param message The "GOLD" message received from the server, containing gold information.
 * @return Returns true if the gold status is successfully displayed, otherwise false.
 */
static bool gameGold(const char* message);


/**************** gameDisplay ****************/
/**
 * @brief Processes the "DISPLAY" message from the server and updates the game display.
 * 
 * Extracts the game map from the "DISPLAY" message and displays it in the ncurses
 * window. The map display starts from the second row to leave space for the gold status
 * at the top. This function is responsible for rendering the game's graphical interface.
 * 
 * @param message The "DISPLAY" message received from the server, containing the game map.
 */
static void gameDisplay(const char* message);


/**************** data_new ****************/
/**
 * @brief Allocates and initializes a new localclient_t structure.
 * 
 * This function allocates memory for a new localclient_t structure, initializes its
 * fields to default values, and returns a pointer to the structure. It exits the
 * program if memory allocation fails.
 * 
 * @return A pointer to the newly allocated localclient_t structure.
 */
static localclient_t* data_new(void);

#endif // CLIENT_H

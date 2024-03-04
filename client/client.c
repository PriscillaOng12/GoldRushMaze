/**
 * Nuggets Game Client, Winter 2024
 * ====================
 * 
 * This file implements the client-side logic for the Nuggets game, a multiplayer treasure-hunt
 * game. It utilizes the ncurses library for rendering the game's graphical user interface in the
 * terminal and handles communication with the game server using a custom messaging protocol.
 * 
 * The main function initializes the client application, establishes a connection to the server,
 * and enters a loop to handle user input and server messages until the game concludes or an error occurs.
 * 
 * Usage:
 * The executable expects 2 or 3 command-line arguments: hostname, port, and optionally a player name.
 * If a player name is provided, the client connects as a player; otherwise, it connects as a spectator.
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include "message.h"

/**************** global types ****************/
typedef struct localclient {
  int NROWS;  // Number of rows in the game board, based on window size
  int NCOLS;  // Number of columns in the game board, based on window size
  char player; // Represents the player's character in the game
} localclient_t;


/**************** file-local functions ****************/
// handling communications
static bool handleMessage(void* arg, const addr_t from, const char* message);
static bool handleInput(void* arg);
static void cursesInit(); 
static void setupWindow();
static void displayMap(char* display);
static void displayTempMessage(const char* temp);
static void clearTempMessage();

// handling specific communication
static bool gameOk(const char* message);
static bool gameGrid(const char* message);
static bool gameGold(const char* message);
static void gameDisplay(const char* message);

// game helper function
static localclient_t* data_new();

static localclient_t* data;


/************** main **************/
int main(int argc, char *argv[]) 
{
  // Validate command line arguments
  const char* program = argv[0];
  if (argc != 3 && argc != 4) {
      fprintf(stderr, "ERROR: Expected either 2 or 3 arguments but recieved %d\n", argc-1);
      fprintf(stderr, "USAGE: %s hostname port [playername]\n", program);
      exit(1); // Exit on incorrect argument count
  }
  // Check for null arguments
  for (int i = 1; i < argc; i++) {
      if (argv[i] == NULL) {
          fprintf(stderr, "ERROR: argmument %d is NULL\n", i);
          exit(2);   // Exit on null argument
      }
  }

  // Initialize data structure
  data = data_new();

  // Initialize message module without logging
  if (message_init(stderr) == 0) {
    free(data);
    exit(3); // Exit on message module initialization failure
  }

  // assign command-line arguments to variables 
  const char* serverHost = argv[1];
  const char* serverPort = argv[2];
  addr_t server; // address of the server
  if (!message_setAddr(serverHost, serverPort, &server)) {
      fprintf(stderr, "ERROR: Failure forming address from %s %s\n", serverHost, serverPort);
      free(data);
      exit(4); // Exit on address formation failure
  }

  // connect to server as player or spectator
  const char* playername = argv[3];
  if (playername != NULL) {     // send playername to server
    char line[message_MaxBytes];
    strcpy(line, "PLAY ");
    strncat(line, playername, message_MaxBytes-strlen("PLAY "));
    message_send(server, line);       // connect as player
  } else {
    message_send(server, "SPECTATE");   // Connect as spectator
    if(!gameOk(NULL)) {  // send quit message
      message_send(server, "KEY Q");
      message_done();
      free(data);
      exit(6);  // Exit on player initialization failure
    }
  }

  // Loop to handle input and messages
  bool ok = message_loop(&server, 0, NULL, handleInput, handleMessage);


  // clear data
  free(data);
  endwin();  // Close curses window
  message_done();  // Shutdown message module

  return ok ? 0 : 1;  // Return success or failure status
}


/**************** handleMessage ****************/
static bool handleMessage(void* arg, const addr_t from, const char* message)
{
  if (arg == NULL || message == NULL) {
    return false; // Ignore null arguments
  }

  // Process various types of messages from the server
  if (strncmp(message, "OK ", strlen("OK ")) == 0){
    if(!gameOk(message)) {
      message_send(from, "KEY Q");
      message_done();
      free(data);
      exit(6); // Exit on "OK" message handling failure
    }
  } else if (strncmp(message, "GRID ", strlen("GRID ")) == 0) {
    if(!gameGrid(message)) {
      message_send(from, "KEY Q");
      message_done();
      free(data);
      exit(5); // Exit on "GRID" message handling failure
    }
  } else if (strncmp(message, "GOLD ", strlen("GOLD ")) == 0) {
    gameGold(message); // Process "GOLD" message
  } else if (strncmp(message, "DISPLAY\n", strlen("DISPLAY\n")) == 0) {
    gameDisplay(message); // Process "DISPLAY" message
  } else if (strncmp(message, "QUIT ", strlen("QUIT ")) == 0) {
    endwin();
    fprintf(stdout, "\n%s\n", message);
    return true; // Exit on "QUIT" message
  } else if (strncmp(message, "ERROR ", strlen("ERROR ")) == 0) {
    fprintf(stderr, "%s\n", message); // Log error message
    const char* temp = strchr(message, ' ') + 1;
    displayTempMessage(temp); // Display temporary message
  } else {
    fprintf(stderr, "ERROR: Malformed message '%s'\n", message); // Log malformed message
  }
  
  return false; // Continue message loop
}


/**************** handleInput ****************/
static bool handleInput(void* arg)
{
  addr_t* serverp = arg; // Cast arg to expected type for server address.
  if (serverp == NULL) {
    fprintf(stderr, "handleInput called with arg=NULL");
    return true; // Return true to signal an error condition.
  }
  if (!message_isAddr(*serverp)) {
    fprintf(stderr, "handleInput called without a correspondent.");
    return true; // Similar error handling as above.
  }

  char c = getch(); // Read a character from ncurses input.
  char message[strlen("KEY ") + 2]; // Prepare message buffer.
  strcpy(message, "KEY "); // Start message with "KEY ".
  strcat(message, &c); // Append the read character to the message.
  message[strlen("KEY ") + 1] = '\0'; // Ensure null-termination.

  clearTempMessage(); // Clear any existing temporary messages.

  message_send(*serverp, message); // Send the constructed message to the server.

  return false; // Continue looping for more input.
}



/* ************ cursesInit *********************** */
static void cursesInit()
{
  initscr(); // Initialize screen
  getmaxyx(stdscr, data->NROWS, data->NCOLS); // Cache window size
  cbreak(); // Input modes
  noecho(); // Don't echo input characters
  curs_set(0); // Invisible cursor
  start_color(); // Start color functionality
  init_pair(1, COLOR_CYAN, COLOR_BLACK); // Set color pair
  attron(COLOR_PAIR(1)); // Apply color pair
}


/* ************ setupWindow *********************** */
static void setupWindow()
{
  int max_nrows = 0; 
  int max_ncols = 0;
  getmaxyx(stdscr, max_nrows, max_ncols); // Get maximum dimensions of the window.
  for (int row = 1; row < max_nrows; row++) {
    for (int col = 0; col < max_ncols; col++) {
      move(row, col); // Move cursor to position.
      addch(' '); // Add a blank space at the cursor position.
    }
  }
  refresh(); // Refresh the screen to show changes.
}


/* ************ displayMap *********************** */
static void displayMap(char* display)
{
  for (int row = 0; row < data->NROWS; row++) {
    for (int col = 0; col < data->NCOLS; col++) {
      move(row + 1, col); // Move to position, accounting for the top status line.
      int idx = row * (data->NCOLS + 1) + col; // Calculate index in the display string.
      if (idx < strlen(display)) {
        addch(display[idx]); // Add character from the display string to the screen.
      } else {
        addch(' '); // Fill with blank if beyond the display string.
      }
    }
  }
  refresh(); // Refresh the screen to show changes.
}


/* ************ displayTempMessage ************* */
static void displayTempMessage(const char* temp)
{
  int max_nrows = 0;
  int max_ncols = 0;
  int loc = 0;
  getmaxyx(stdscr, max_nrows, max_ncols); // Get screen dimensions.
  (void)max_nrows;
  for (loc = 0; loc < max_ncols; loc++) {
    char c = mvinch(0, loc) & A_CHARTEXT; // Get character at position.
    if (c == '.') {
      loc++;
      break;
    }
  }

// Display the temporary message if provided.
if (temp != NULL) {
    mvprintw(0, loc + 1, "%.*s", max_ncols - (loc + 2), temp); // Print the temporary message.
}
refresh(); // Refresh the screen to show the temporary message.
}



/* ************ clearTempMessage ************* */
static void clearTempMessage()
{
  int max_nrows = 0; 
  int max_ncols = 0;
  int loc = 0;
  getmaxyx(stdscr, max_nrows, max_ncols);  // Get screen dimensions to determine area to clear.
  (void)max_nrows; // Unused variable, explicitly ignored to avoid compiler warnings.
  // Iterate through the top row to find and clear temporary messages.
  for (loc = 0; loc < max_ncols; loc++) {
    char c = mvinch(0, loc) & A_CHARTEXT; // Get character at current location.
    if (c == '.') { // '.' used as an indicator for the start of temporary messages.
      loc++; // Move past the indicator to start clearing.
      break; // Exit loop once indicator is found.
    }
  }
  if (loc != 0) { // Ensure that we found a message to clear.
    while(loc < max_ncols) { // Clear from the indicator to the end of the line.
      move(0, loc++); // Move to each character position.
      addch(' '); // Replace with a blank space.
    }
  }
  refresh(); // Refresh the screen to display the updated content.
}




/**************** gameOk ****************/
static bool gameOk(const char* message)
{
  data->player = 0; // Reset player character to 0, indicating no player character received yet.
  if (message != NULL) { // Ensure the message is not NULL.
    if (sscanf(message, "OK %c", &data->player) != 1) { // Attempt to parse player character from the message.
      return false; // Return false if parsing fails.
    }
  }
  cursesInit(); // Initialize the curses library for UI display.
  setupWindow(); // Set up the initial game window with a blank map.
  return true; // Return true, indicating successful processing of the "OK" message.
}


/**************** gameGrid ****************/
static bool gameGrid(const char* message)
{
  int nrows;
  int ncols;
  if (sscanf(message, "GRID %d %d", &nrows, &ncols) != 2) { // Parse nrows and ncols from the message.
    fprintf(stderr, "ERROR: Malformed GRID message '%s'", message); // Log error on malformed message.
    return false; // Return false due to parsing failure.
  }
  // Verify that the screen size is sufficient for the grid dimensions.
  while (nrows+1 > data->NROWS || ncols > data->NCOLS) { // Check if window resize is needed.
    endwin(); // Temporarily end curses session to allow for window resizing.
    fprintf(stderr, "ERROR: incompatible screen of size [%d, %d] for [%d, %d]\n", data->NROWS, data->NCOLS, nrows+1, ncols);
    mvprintw(0,0, "ERROR: incompatible screen of size [%d, %d] for [%d, %d]\nRESIZE and press ENTER to continue", data->NROWS, data->NCOLS, nrows+1, ncols);
    char ch = getch(); // Wait for user input after resizing.
    if (ch == '\n') {
      endwin(); // End curses session again if necessary.
      cursesInit(); // Reinitialize curses with the new window size.
    }
  }
  data->NROWS = nrows+1; // Update stored grid dimensions.
  data->NCOLS = ncols;
  return true; // Return true, indicating successful processing of the "GRID" message.
}


/**************** gameGold ****************/
static bool gameGold(const char* message)
{
  // Declare variables to hold gold information
  int gold_collected;
  int gold_purse;
  int gold_left;
  // Parse the gold information from the message
  if (sscanf(message, "GOLD %d %d %d", &gold_collected, &gold_purse, &gold_left) != 3) {
    // Log error if message format is incorrect
    fprintf(stderr, "ERROR: Malformed GOLD message '%s'", message);
    return false; // Return false due to parsing failure
  } else {
    // Dummy variable for function call and window columns
    int temp = 0; 
    int ncols = 0;
    // Get the current number of columns in the window
    getmaxyx(stdscr, temp, ncols); 
    // Dummy variable usage to satisfy the compiler
    (void)temp; 
    // Prepare the top line for displaying gold information
    for (int col = 0; col < ncols; col++) {
      move(0, col); // Move cursor to column position
      addch(' '); // Clear the position
    }
    // Buffer for formatted gold information message
    char buffer[ncols]; 
    // Conditional formatting based on player status
    if (data->player == 0) {
      // Format message for spectator
      snprintf(buffer, ncols, "Spectator: %d nuggets unclaimed.", gold_left);
    } else {
      // Format message for player
      int len_msg = snprintf(buffer, ncols, "Player %c has %d nuggets (%d nuggets unclaimed).", data->player, gold_purse, gold_left);
      char gold_left_buffer[ncols]; // Additional buffer for extended message
      int len_gold_left_msg = 0;
      // Append message about collected gold if applicable
      if (gold_collected > 0) {
        if (ncols - len_msg > 0) {
          len_gold_left_msg = snprintf(gold_left_buffer, ncols - len_msg, "  GOLD received: %d", gold_collected);
        }
      }
      // Combine the main message with the extended message
      for (int i = len_msg; i < len_msg + len_gold_left_msg; i++) {
        buffer[i] = gold_left_buffer[i-len_msg];
      }
      buffer[len_msg+len_gold_left_msg] = '\0'; // Ensure null-termination
    }
    // Display the composed message on the top line of the window
    mvprintw(0,0, "%.*s", ncols, buffer);
    refresh(); // Refresh the window to update the display
    return true; // Return true on successful execution
  }
}

/**************** gameDisplay ****************/
static void gameDisplay(const char* message)
{
  // Calculate the length of the display part of the message
  size_t headerLength = strlen("DISPLAY\n");
  size_t displayLength = strlen(message) - headerLength + 1;
  // Prepare buffer for the display content
  char display[displayLength]; 
  // Copy the display content from the message to the buffer
  strncpy(display, message + headerLength, displayLength);
  display[displayLength] = '\0'; // Ensure null-termination
  
  // Call the function to display the map on the screen
  displayMap(display);
}



/* ************ data_new *********** */
static localclient_t* data_new()
{
  // Allocate memory for a new localclient_t structure
  localclient_t* data = mem_assert(malloc(sizeof(localclient_t)), "Failed to allocate memory for localclient_t.");
  if (data == NULL) {
    exit(3); // Exit if memory allocation fails
  }
  // Initialize the data structure with default values
  data->NROWS = -1;
  data->NCOLS = -1;
  data->player = 0;

  return data; // Return the pointer to the newly allocated structure
}

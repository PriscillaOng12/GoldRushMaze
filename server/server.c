/**
 * Server blah blah
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "../support/log.h"
#include "../support/message.h"
#include "file.h"
#include "grid.h"
#include "player.h"
#include "spectator.h"

static bool parseArgs(const int argc, const char** argv);
static bool handleMessage(void* arg, const addr_t from, const char* message);
// grid_t* gameGrid;

int main(const int argc, const char** argv) {
  //validate args
  if (!parseArgs(argc, argv)) {
    return 1;
  }

  //initialize message module
  int serverPort;
  FILE* logFP = fopen("server.log", "w");
  if ((serverPort = message_init(logFP)) == 0) {
    return 1;
  }

  printf("Server port is:%d", serverPort);

  char* mapPath = malloc(strlen(argv[1]) + 9);
  strcpy(mapPath, "../maps/");
  strcat(mapPath, argv[1]);
  FILE* fp = fopen(mapPath, "r");
  grid_t* gameGrid = grid_load(fp);
  fclose(fp);
  // free(mapPath);

  message_loop(gameGrid, 0, NULL, NULL, handleMessage);

  message_done();

  // grid_game_over(gameGrid);

  return 0;
}

static bool parseArgs(const int argc, const char** argv) { //CHANGE IMPLEMENTATION

  //validate there are 2 or 3 args
  if (argc > 3 || argc < 2) {
    fprintf(stderr, "usage : %s map.txt [seed], your command must have two arguments", argv[0]);
    return false;
  }
  
  //validate file can be opened
  char* mapPath = malloc(strlen(argv[1]) + 9);
  strcpy(mapPath, "../maps/");
  strcat(mapPath, argv[1]);
  FILE* fp = fopen(mapPath, "r");
  if (fp == NULL) {
    fprintf(stderr, "map file could not be opened");
    free(mapPath);
    return false;
  }
  free(mapPath);
  fclose(fp);

  //assumes seed will be integer
  if (argc == 3) {
    //convert to int type
    int seed = atoi(argv[2]);
    if (seed < 0) {
      fprintf(stderr, "seed must be positive integer");
      return false;
    }
    srand(seed);
  }
  else {
    srand(getpid());
  }

  return true;

}

static bool handleMessage(void* arg, const addr_t from, const char* message) {
  //set max name length to 50 chars
  int MaxNameLength = 50;
  int MaxPlayers = 26;
  grid_t* gameGrid = (grid_t*) arg;
  player_t** playerList = grid_getplayers(gameGrid); //THIS METHOD MUST BE MADE
  int playerCount = grid_getplayercount(gameGrid); //THIS METHOD MUST BE MADE
  //get first word from message
  char* firstWord;
  int firstSpace = 0;
  while (!isspace(message[firstSpace])) {
    firstSpace++;
  }
  //amount of bytes needed for first word of message + 1 for null terminating
  firstWord = malloc(firstSpace + 1);
  strncpy(firstWord, message, firstSpace);
  firstWord[firstSpace] = '\0';

  //if first word is PLAY
  if (strcmp(firstWord, "PLAY") == 0) {
    if (playerCount == MaxPlayers) {
      message_send(from, "QUIT Game is full: no more players can join.");
    }
    //get real_name, i.e. rest of message
    char* real_name = mem_assert(malloc(128), "Error allocating space"); //prob
    strcpy(real_name, message+5); //problem
    //if name is empty, send error message
    if (strcmp(real_name, "") == 0) {
      message_send(from, "QUIT Sorry - you must provide player's name.");
      return false;
    }
    //truncate to MaxNameLength and replace characters that are both isgraph() and isblank()
    char* real_name_truncated = malloc(MaxNameLength + 1);
    strncpy(real_name_truncated, real_name, MaxNameLength);
    real_name_truncated[50] = '\0';
    for (int i = 0; i < 51; i++) {
      if (isgraph(real_name_truncated[i] && isblank(real_name_truncated[i]))) {
        real_name_truncated[i] = '_';
      }
    }
    
    //spawns players
    grid_spawn_player(gameGrid, &from, real_name_truncated);
    //get updated player count and player list
    int playerCount = grid_getplayercount(gameGrid);
    player_t** playerList = grid_getplayers(gameGrid);
    //send OK message with player symbol
    char* messageToSend = malloc(128);
    char playerCharacter = (char) (64 + playerCount);
    sprintf(messageToSend, "OK %c", playerCharacter);
    message_send(from, messageToSend);
    free(messageToSend);

    //send initial GRID message with grid information
    char* nrows = malloc(sizeof(int));
    sprintf(nrows, "%d", grid_getnrows(gameGrid));//problem
    char* ncols = malloc(sizeof(int));
    sprintf(ncols, "%d", grid_getncols(gameGrid));
    messageToSend = malloc(128);
    strcpy(messageToSend, "GRID ");
    strcat(messageToSend, nrows);
    strcat(messageToSend, " ");
    strcat(messageToSend, ncols);
    messageToSend[5 + strlen(nrows) + 1 + strlen(ncols)] = '\0';
    message_send(from, messageToSend);
    free(messageToSend);

    //initialize gold in grid
    grid_init_gold(gameGrid);

    //send initial GOLD message
    messageToSend = malloc(128);
    sprintf(messageToSend, "GOLD 0 0 %d", grid_getnuggetcount(gameGrid));
    // messageToSend[9 + strlen(remainingGold)] = '\0';
    message_send(from, messageToSend);
    free(messageToSend);

    //send DISPLAY message to all players and spectator
    for (int i = 0; i < playerCount; i++) {
      if (player_get_isactive(playerList[i])) {
        //if player is active, send them updated DISPLAY
        messageToSend = malloc(128);
        messageToSend = grid_send_state(gameGrid, playerList[i]);
        message_send(*player_get_addr(playerList[i]), messageToSend);
        free(messageToSend);
      }
    }
    //if there is spectator, send them updated DISPLAY
    if (grid_getspectatorCount(gameGrid) == 1) 
    {
      messageToSend = malloc(128);
      messageToSend = grid_send_state_spectator(gameGrid);
      message_send(*spectator_get_addr(grid_getspectator(gameGrid)), messageToSend);
      free(messageToSend);
    }
    return false;
  }
  else if (strcmp(firstWord, "KEY") == 0) {
    //find matching player in list of players to find out which player to move
    player_t* matchingPlayer = NULL;
    for (int i = 0; i < playerCount; i++) {
      const addr_t playerAddr = *player_get_addr(playerList[i]); //WILL THIS WORK?????
      if (message_eqAddr(from, playerAddr)) {
        matchingPlayer = playerList[i];
        break;
      }
    }
    
    char* keyStroke = malloc(128);
    strcpy(keyStroke, message + 4); //DOES THIS WORK
    if (strcmp(keyStroke, "h") == 0) { //CAPITAL CHARACTER FIX, CHECK IF SPECTATOR
      player_moveto(matchingPlayer, -1, 0);
    }
    else if (strcmp(keyStroke, "l") == 0) {
      player_moveto(matchingPlayer, 1, 0);
    }
    else if (strcmp(keyStroke, "j") == 0) {
      printf("hello");
      player_moveto(matchingPlayer, 0, -1);
    }
    else if (strcmp(keyStroke, "k") == 0) {
      player_moveto(matchingPlayer, 0, 1);
    }
    else if (strcmp(keyStroke, "y") == 0) {
      player_moveto(matchingPlayer, -1, 1);
    }
    else if (strcmp(keyStroke, "u") == 0) {
      player_moveto(matchingPlayer, 1, 1);
    }
    else if (strcmp(keyStroke, "b") == 0) {
      player_moveto(matchingPlayer, -1, -1);
    }
    else if (strcmp(keyStroke, "n") == 0) {
      player_moveto(matchingPlayer, 1, -1);
    }
    else if (strcmp(keyStroke, "Q") == 0) {
      free(keyStroke);
      if (matchingPlayer == NULL) {
        message_send(from, "QUIT Thanks for watching!");
        return false;
      }
      message_send(from, "QUIT Thanks for playing!");
      return false;
    }
    else {
      message_send(from, "ERROR unknown keystroke");
      return false;
    }
    free(keyStroke);
    //if no more nuggets remaining, game over
    if (grid_getnuggetcount(gameGrid) == 0) {
      grid_game_over(gameGrid);
      return true;
    }
    //send DISPLAY message to all players and spectator
    for (int i = 0; i < playerCount; i++) {
      if (player_get_isactive(playerList[i])) {
        //if player is active, send them updated DISPLAY
        char* messageToSend = malloc(128);
        messageToSend = grid_send_state(gameGrid, playerList[i]);
        message_send(*player_get_addr(playerList[i]), messageToSend);
        free(messageToSend);
      }
    }
    //if there is spectator, send them updated DISPLAY
    if (grid_getspectatorCount(gameGrid) == 1) 
    {
      char* messageToSend = malloc(128);
      messageToSend = grid_send_state_spectator(gameGrid);
      message_send(*spectator_get_addr(grid_getspectator), messageToSend);
      free(messageToSend);
    }
    return false;
  }
  else if (strcmp(firstWord, "SPECTATE") == 0) {
    //spawns spectator
    grid_spawn_spectator(gameGrid, spectator_new(&from)); //MAKE THIS

    //send initial GRID message with grid information
    char* nrows = malloc(sizeof(int));
    sprintf(nrows, "%d", grid_getnrows(gameGrid));//problem
    char* ncols = malloc(sizeof(int));
    sprintf(ncols, "%d", grid_getncols(gameGrid));
    char* messageToSend = malloc(128);
    strcpy(messageToSend, "GRID ");
    strcat(messageToSend, nrows);
    strcat(messageToSend, " ");
    strcat(messageToSend, ncols);
    messageToSend[5 + strlen(nrows) + 1 + strlen(ncols)] = '\0';
    message_send(from, messageToSend);
    free(messageToSend);

    grid_init_gold(gameGrid);

    //send initial GOLD message
    messageToSend = malloc(128);
    sprintf(messageToSend, "GOLD 0 0 %d", grid_getnuggetcount(gameGrid));
    message_send(from, messageToSend);
    free(messageToSend);

    //send DISPLAY message
    messageToSend = malloc(128);
    messageToSend = grid_send_state_spectator(gameGrid);
    message_send(from, messageToSend);
    free(messageToSend);

    return false;
  }
  else {
    message_send(from, "invalid message");
    return true;
  }
}
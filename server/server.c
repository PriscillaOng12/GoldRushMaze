/**
 * Server blah blah
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "../support/log.h"
#include "../support/message.h"
#include "file.h"
#include "grid.h"
#include "player.h"

static bool parseArgs(const int argc, const char** argv);
static bool handleMessage(void* arg, const addr_t from, const char* message);

int main(const int argc, const char** argv) {

  //validate args
  if (parseArgs(argc, argv) == NULL) {
    return 1;
  }

  //initialize message module
  if (message_init(NULL) == 0) {
    return 1;
  }

  //FIGURE OUT HOW TO GET ADDRESS INFORMATION
  char* serverHost = "127.0.0.1";
  char* serverPort = "8080";
  addr_t* server;
  if (!message_setAddr(serverHost, serverPort, &server)) {
    fprintf(stderr, "can't form address from %s %s\n", serverHost, serverPort);
    return 4; // bad hostname/port
  }

  char* mapPath = malloc(strlen(argv[2]) + 9);
  strcpy(mapPath, "../maps/");
  strcat(mapPath, argv[1]);
  FILE* fp = fopen(mapPath, "r");
  grid_t* gameGrid = grid_load(fp);

  message_loop(&gameGrid, 0, NULL, NULL, handleMessage);

  message_done();
  
  return 0;
}

static bool parseArgs(const int argc, const char** argv) { //CHANGE IMPLEMENTATION

  //validate there are 2 or 3 args
  if (argc > 3 || argc < 2) {
    fprintf(stderr, "usage : %s map.txt [seed], your command must have two arguments", argv[0]);
    return NULL;
  }
  
  //validate file can be opened
  char* mapPath = malloc(strlen(argv[2]) + 9);
  strcpy(mapPath, "../maps/");
  strcat(mapPath, argv[1]);
  FILE* fp = fopen(mapPath, "r");
  if (fp == NULL) {
    fprintf(stderr, "map file could not be opened");
    free(mapPath);
    free(fp);
    return NULL;
  }
  free(mapPath);
  free(fp);

  //assumes seed will be integer
  if (argc == 3) {
    if (argv[2] < 0) {
      fprintf(stderr, "seed must be positive integer");
      return NULL;
    }
    srand(argv[2]);
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
  grid_t* gameGrid = arg;
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
    char* real_name = malloc(strlen(message) - firstSpace - 1);
    strcopy(real_name, message[firstSpace + 1]);
    //if name is empty, send error message
    if (strcmp(real_name, "") == 0) {
      message_send(from, "QUIT Sorry - you must provide player's name.");
      return false;
    }
    //truncate to MaxNameLength and replace characters that are both isgraph() and isblank()
    char* real_name_truncated = malloc(MaxNameLength + 1);
    strncpy(real_name_truncated, real_name, MaxNameLength);
    real_name_truncated[50] = '\0';
    for (int i = 0; i < strlen(real_name_truncated); i++) {
      if (isgraph(real_name_truncated[i] && isblank(real_name_truncated[i]))) {
        real_name_truncated[i] = '_';
      }
    }
    
    //spawns players
    grid_spawn_player(gameGrid, &from, real_name_truncated);
    //send OK message with player symbol
    char playerCharacter = (char)(playerCount + 1);
    char* messageToSend = malloc(4);
    strcpy(messageToSend, "OK ");
    strcat(messageToSend, playerCharacter);
    message_send(from, messageToSend);
    free(messageToSend);

    //send initial GRID message with grid information
    char* nrows;
    sprintf(nrows, "%d", grid_getnrows(gameGrid));
    char* ncols;
    sprintf(ncols, "%d", grid_getncols(gameGrid));
    char* messageToSend = malloc(5 + strlen(nrows) + 1 + strlen(ncols) + 1);
    strcpy(messageToSend, "GRID ");
    strcat(messageToSend, nrows);
    strcat(messageToSend, " ");
    strcat(messageToSend, ncols);
    strcat(messageToSend, '\0');
    message_send(from, messageToSend);
    free(messageToSend);

    //send initial GOLD message
    char* remainingGold;
    sprintf(remainingGold, "%d", grid_getnuggetcount(gameGrid));
    char* messageToSend = malloc(9 + strlen(remainingGold) + 1);
    strcpy(messageToSend, "GOLD 0 0 ");
    strcat(messageToSend, remainingGold);
    strcat(messageToSend, '\0'); //DO I NEED TO DO THIS>>??????
    message_send(from, messageToSend);
    free(messageToSend);

    //send DISPLAY message
    grid_send_state(playerList[playerCount - 1]); //WILL THIS WORK???
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
    
    char* keyStroke;
    strcpy(keyStroke, message[4]);
    if (strcmp(keyStroke, "h") == 0) {
      player_moveto(matchingPlayer, -1, 0);
    }
    else if (strcmp(keyStroke, "l") == 0) {
      player_moveto(matchingPlayer, 1, 0);
    }
    else if (strcmp(keyStroke, "j") == 0) {
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
      if (matchingPlayer == NULL) {
        message_send(from, "QUIT Thanks for watching!");
        return false;
      }
      message_send(from, "QUIT Thanks for playing!");
      return false;
    }
    //if no more nuggets remaining, game over
    if (grid_getnuggetcount(gameGrid) == 0) {
      grid_game_over(gameGrid);
      return true;
    }
    else if (strcmp(firstWord, "SPECTATE") == 0) {
      //spawns spectator
      grid_spawn_spectator(gameGrid, &from); //MAKE THIS

      //send initial GRID message with grid information
      char* nrows;
      sprintf(nrows, "%d", grid_getnrows(gameGrid));
      char* ncols;
      sprintf(ncols, "%d", grid_getncols(gameGrid));
      char* messageToSend = malloc(5 + strlen(nrows) + 1 + strlen(ncols) + 1);
      strcpy(messageToSend, "GRID ");
      strcat(messageToSend, nrows);
      strcat(messageToSend, " ");
      strcat(messageToSend, ncols);
      strcat(messageToSend, '\0');
      message_send(from, messageToSend);
      free(messageToSend);

      //send initial GOLD message
      char* remainingGold;
      sprintf(remainingGold, "%d", **grid_getnuggets(gameGrid));
      char* messageToSend = malloc(9 + strlen(remainingGold) + 1);
      strcpy(messageToSend, "GOLD 0 0 ");
      strcat(messageToSend, remainingGold);
      strcat(messageToSend, '\0'); //DO I NEED TO DO THIS>>??????
      message_send(from, messageToSend);
      free(messageToSend);

      //send DISPLAY message
      grid_send_state_spectator(grid_getspectator(gameGrid)); //THIS METHOD MUST BE MADE
      return false;
    }
  }
  return false;
}
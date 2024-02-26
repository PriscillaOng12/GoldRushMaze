/**
 * Server blah blah
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "./support/log.h"
#include "./support/message.h"
#include "./server/file.h"
#include "./server/grid.h"
#include "./server/player.h"

static bool parseArgs(const int argc, char* argv[]);
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
  char* serverHost;
  char* serverPort;
  addr_t* server;
  if (!message_setAddr(serverHost, serverPort, &server)) {
    fprintf(stderr, "can't form address from %s %s\n", serverHost, serverPort);
    return 4; // bad hostname/port
  }


  FILE* fp = fopen(argv[1], "r");
  grid_t* gameGrid = grid_load(fp);

  message_loop(&server, 0, NULL, NULL, handleMessage);

  message_done();
  
  return 0;
}

static bool parseArgs(const int argc, char* argv[]) {

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
    player_t** playerList = grid_getplayers(gameGrid); //ADD THIS METHOD
    //get real_name, i.e. rest of message
    char* real_name = malloc(strlen(message) - firstSpace - 1);
    strcopy(real_name, message[firstSpace + 1]);
    //if name is empty, send error message
    if (strcmp(real_name, "") == 0) {
      message_send(from, "QUIT Sorry - you must provide player's name.");
      return false;
    }
    //truncate to MaxNameLength and replace characters that are both isgraph() and isblank()
    char* real_name_truncated = malloc(51);
    strncpy(real_name_truncated, real_name, 50);
    real_name_truncated[50] = '\0';
    for (int i = 0; i < strlen(real_name_truncated); i++) {
      if (isgraph(real_name_truncated[i] && isblank(real_name_truncated[i]))) {
        real_name_truncated[i] = '_';
      }
    }
    grid_spawn_player(gameGrid, from, real_name_truncated);
    



  }


}
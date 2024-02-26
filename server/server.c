/**
 * Server blah blah
*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "./support/log.h"
#include "./support/message.h"

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
  FILE* fp = fopen(argv[1], "r");
  if (fp == NULL) {
    fprintf(stderr, "map file could not be opened");
    return NULL;
  }

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
  //get first word from message
  char* firstWord;
  int firstSpace = 0;
  while (!isspace(message[i])) {
    firstSpace++;
  }
  //amount of bytes needed for first word of message + 1 for null terminating
  firstWord = malloc(firstSpace + 1);
  strncpy(firstWord, message, firstSpace);
  firstWord[firstSpace] = '\0';

  //if first word is PLAY
  if (strcmp(firstWord, "PLAY") == 0) {
    //get real_name, i.e. rest of message
    char* real_name = malloc(strlen(message) - firstSpace - 1);
    strcopy(real_name, message[firstSpace + 1]);
    //if name is empty, send error message
    if (strcmp(real_name, "") == 0) {
      message_send(from, "name must be provided");
    }



  }


}
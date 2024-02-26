/**
 * Server blah blah
*/

#include <stdio.h>
#include <stdlib.h>
#include "./support/log.h"
#include "./support/message.h"

static bool parseArgs(const int argc, char* argv[]);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static bool sendResponse(void* arg);

int main(const int argc, const char** argv) {

  if (parseArgs(argc, argv) != NULL) {
    
  }

  


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

}

static bool sendResponse(void* arg) {

}
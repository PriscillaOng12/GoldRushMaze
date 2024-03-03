/* 
 * counter.c - CS50 'counter' module
 *
 * see counters.h for more information.
 *
 * David Kotz, April 2016, 2017, 2019, 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "counters.h"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** local types ****************/
typedef struct countersnode {
  int key;          
  int count;
  struct countersnode *next;       // link to next node
} countersnode_t;

/**************** global types ****************/
typedef struct counters {
  struct countersnode *head;       // head of the list of items in counters
} counters_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see counters.h for comments about exported functions */

/**************** local functions ****************/
/* not visible outside this file */
static countersnode_t* countersnode_new(const int key);

/**************** counters_new() ****************/
/* see counters.h for description */
counters_t*
counters_new(void)
{
  counters_t* counters = mem_malloc(sizeof(counters_t));

  if (counters == NULL) {
    return NULL;              // error allocating counters
  } else {
    // initialize contents of counters structure
    counters->head = NULL;
    return counters;
  }
}

/**************** counters_add() ****************/
/* see counters.h for description */
int 
counters_add(counters_t* ctrs, const int key)
{
  if (ctrs != NULL && key >= 0) {
    for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
      if (node->key == key) { //key found, increment and return count
        node->count = node->count + 1;
        return node->count;
      }
    }
    // allocate a new node to be added to the list
    countersnode_t* new = countersnode_new(key);
    if (new != NULL) {
      // add it to the head of the list
      new->next = ctrs->head;
      new->key = key;
      new->count = 1;
      ctrs->head = new;
      return new->count;
    }
  }

  return (0);

#ifdef MEMTEST
  mem_report(stdout, "After counters_insert");
#endif
}


/**************** countersnode_new ****************/
/* Allocate and initialize a countersnode */
static countersnode_t*  // not visible outside this file
countersnode_new(const int key)
{
  countersnode_t* node = mem_malloc(sizeof(countersnode_t));

  if (node == NULL) {
    // error allocating memory for node; return error
    return NULL;
  } else {
    node->key = key;
    node->next = NULL;
    return node;
  }
}

/**************** counters_get() ****************/
/* see counters.h for description */
int
counters_get(counters_t* ctrs, const int key)
{
  if (ctrs == NULL) {
    return (0);              // bad counter
  } else if (ctrs->head == NULL) {
    return (0);              // counter is empty
  } else if (key < 0) {
    return (0);
  } else {
    for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
      if (node->key == key) { //key found, return count
        return node->count;
      }
    }
    return (0);
  }
}

/**************** counters_set() ****************/
/* see counters.h for description */
bool 
counters_set(counters_t* ctrs, const int key, const int count) {
  if (ctrs == NULL) {
    return false;              // bad counter
  } else if (ctrs->head == NULL) {
    return false;              // counter is empty
  } else if (key < 0) {
    return false;
  } else if (count < 0) {
    return false;
  } else {
    for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
      if (node->key == key) { //key found, set and return count
        node->count = count;
        return true;
      }
    }
    return false;
  }
}

/**************** counters_print() ****************/
/* see counters.h for description */
void
counters_print(counters_t* ctrs, FILE* fp)
{
  if (fp != NULL) {
    if (ctrs!= NULL) {
      fputc('{', fp);
      for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
        // print this node
        fprintf(fp, "key: %d", node->key);
        fprintf(fp, "=");
        fprintf(fp, "count: %d, ", node->count);
      }
      fputc('}', fp);
    } else {
      fputs("(null)", fp);
    }
  }
}

/**************** counters_iterate() ****************/
/* see counters.h for description */
void
counters_iterate(counters_t* ctrs, void* arg, void (*itemfunc)(void* arg, const int key, const int count) )
{
  if (ctrs != NULL && itemfunc != NULL) {
    // call itemfunc with arg, on each item
    for (countersnode_t* node = ctrs->head; node != NULL; node = node->next) {
      (*itemfunc)(arg, node->key, node->count); 
    }
  }
}

/**************** counters_delete() ****************/
/* see counters.h for description */
void 
counters_delete(counters_t* ctrs)
{
  if (ctrs != NULL) {
    for (countersnode_t* node = ctrs->head; node != NULL; ) {
      countersnode_t* next = node->next;     // remember what comes next
      mem_free(node);                   // free the node
      node = next;                      // and move on to next
    }

    mem_free(ctrs);
  }

#ifdef MEMTEST
  mem_report(stdout, "End of bag_delete");
#endif
}

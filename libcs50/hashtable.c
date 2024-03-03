/* 
 * hashtable.c - CS50 'hashtable' module
 *
 * see hashtable.h for more information.
 *
 * Edward Kim, Winter 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"
#include "hashtable.h"
#include "set.h"
#include "set.c"
#include "mem.h"

/**************** file-local global variables ****************/
/* none */

/**************** global types ****************/
typedef struct hashtable {
  set_t** table;
  int num_slots;
} hashtable_t;

/**************** global functions ****************/
/* that is, visible outside this file */
/* see hashtable.h for comments about exported functions */

/**************** hashtable_new() ****************/
/* see hashtable.h for description */
hashtable_t*
hashtable_new(const int num_slots)
{
  hashtable_t* hashtable = mem_malloc(sizeof(hashtable_t));

  if (hashtable == NULL) {
    return NULL;              // error allocating hashtable
  } else {
    // initialize contents of hashtable structure
    //set_t* set[num_slots];
    //hashtable->table = set;
    hashtable->table = mem_calloc(num_slots, sizeof(set_t*));
    hashtable->num_slots = num_slots;
    return hashtable;
  }
}

/**************** hashtable_insert() ****************/
/* see hashtable.h for description */
bool 
hashtable_insert(hashtable_t* ht, const char* key, void* item)
{

  if (ht != NULL && key != NULL && item != NULL) { //check for null pointers
    int hash = hash_jenkins(key, ht->num_slots); //create hash
    if (ht->table[hash] != NULL) { //check if set exists in hash already
      if (set_find(ht->table[hash], key) != NULL) { //if key exists in hashed set, return false
        return false;
      }
      set_insert(ht->table[hash], key, item); //if key doesn't already exist, insert into hashed set
      return true;
    }
    ht->table[hash] = set_new(); //if set does not exist in hashed slot, create set and add (key, item) as head of new set
    set_insert(ht->table[hash], key, item);
    return true;
  }

  return false;

#ifdef MEMTEST
  mem_report(stdout, "After bag_insert");
#endif
}

/**************** hashtable_find() ****************/
/* see hashtable.h for description */
void* 
hashtable_find(hashtable_t* ht, const char* key) {
  if (ht != NULL && key != NULL) {
    int hash = hash_jenkins(key, ht->num_slots);
    if (ht->table[hash] != NULL) {
      if (set_find(ht->table[hash], key) != NULL) {
        return set_find(ht->table[hash], key);
      }
    }
  }

  return NULL;
}

/**************** hashtable_print() ****************/
/* see hashtable.h for description */
void
hashtable_print(hashtable_t* ht, FILE* fp, void (*itemprint)(FILE* fp, const char* key, void* item))
{
  if (fp != NULL) {
    if (ht != NULL) {
      fputc('{', fp);
      for (int i = 0; i < ht->num_slots; i++) {
        set_t* set = ht->table[i];
        if (set != NULL) {
          set_print(set, fp, itemprint);
          printf("\n");
        }
        else {
          printf("(null), (null)");
          printf("\n");
        }
      }
      fputc('}', fp);
    } else {
      fputs("(null)", fp);
    }
  }
}

/**************** hashtable_iterate() ****************/
/* see hashtable.h for description */
void
hashtable_iterate(hashtable_t* ht, void* arg, void (*itemfunc)(void* arg, const char* key, void* item) )
{
  if (ht != NULL && itemfunc != NULL) {
    // call itemfunc with arg, on each set using set_iterate
    for (int i = 0; i < ht->num_slots; i++) {
      set_t* set = ht->table[i];
      if (set != NULL) {
        set_iterate(set, arg, itemfunc);
      }
    }
  }
}

/**************** hashtable_delete() ****************/
/* see hashtable.h for description */
void 
hashtable_delete(hashtable_t* ht, void (*itemdelete)(void* item) )
{
  if (ht != NULL) {
    for (int i = 0; i < ht->num_slots; i++) {
      set_t* set = ht->table[i];
      if (set != NULL) {         // if possible...
        set_delete(set, itemdelete);      // delete node's item
      }
    }
    mem_free(ht->table);
    mem_free(ht);
  }

#ifdef MEMTEST
  mem_report(stdout, "End of hashtable_delete");
#endif
}
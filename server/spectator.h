#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "grid.h"
#include "message.h"

/**************** global types ****************/
typedef struct spectator spectator_t;

/**************** functions ****************/

/***************** spectator_new *****************/
/* Create a new spectator object.
 *
 * Caller provides:
 *   a valid address struct with connection information.
 * We guarantee:
 *   a new spectator object is returned if memory allocation is successful.
 *   returns NULL if memory allocation fails.
 * Notes:
 *   the caller is responsible for managing the memory of the addr_t* passed in.
 *   the caller must call spectator_delete to free the spectator object's memory.
 */
spectator_t* spectator_new(addr_t* connection_info);

/***************** spectator_delete *****************/
/* Delete a spectator object.
 *
 * Caller provides:
 *   a valid spectator object.
 * We guarantee:
 *   the spectator's memory is freed.
 * Notes:
 *   does not free the memory of the connection_info inside the spectator; 
 *   this is the caller's responsibility.
 */
void spectator_delete(spectator_t* spectator);

/***************** spectator_quit *****************/
/* Handle a spectator quitting.
 *
 * Caller provides:
 *   valid spectator object and grid object.
 * We guarantee:
 *   spectator count in the grid is updated.
 *   the spectator's memory is freed.
 * Notes:
 *   assumes the spectator is present in the grid.
 *   does not modify the grid beyond updating the spectator count.
 */
void spectator_quit(spectator_t* spectator, grid_t* grid);

/***************** spectator_get_addr *****************/
/* Get the address of a spectator.
 *
 * Caller provides:
 *   a valid spectator object.
 * We guarantee:
 *   returns the address (addr_t*) of the spectator.
 * Notes:
 *   does not transfer ownership of the address; 
 *   the caller must not free the returned address through this function.
 */
addr_t* spectator_get_addr(spectator_t* spectator);
/* 
 * spectator.c - 'spectator' module
 *
 * see spectator.h for more information.
 *
 * ctrl-zzz, Winter 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "grid.h"
#include "message.h"

typedef struct spectator {
    addr_t* connection_info;
} spectator_t;

spectator_t* spectator_new(addr_t* connection_info)
{
    spectator_t* spectator = (spectator_t*) malloc(sizeof(spectator_t));
    return spectator;
}

void spectator_delete(spectator_t* spectator)
{
    free(spectator);
}

void spectator_quit(spectator_t* spectator, grid_t* grid)
{
    // grid_getspectator(); // TODO: SET NULL
    spectator_delete(spectator);
}

addr_t* spectator_get_addr(spectator_t* spectator)
{
    return spectator->connection_info;
}
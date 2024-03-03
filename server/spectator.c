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
#include "mem.h"

typedef struct spectator
{
    addr_t *connection_info;
} spectator_t;

spectator_t *spectator_new(const addr_t connection_info)
{
    spectator_t *spectator = (spectator_t *)mem_assert(malloc(sizeof(spectator_t)), "Error allocating space for spectator");
    spectator->connection_info = (addr_t *)mem_assert(malloc(sizeof(addr_t)), "Error allocating space for x");
    *(spectator->connection_info) = connection_info;
    return spectator;
}

void spectator_delete(spectator_t *spectator)
{
    free(spectator->connection_info);
    free(spectator);
}

void spectator_quit(spectator_t *spectator, grid_t *grid) // assumes spectator is actually there.
{
    if (spectator->connection_info != NULL)
    {
        message_send(*spectator->connection_info, "QUIT Thanks for watching!\n");
    }
    grid_setspectatorCount(grid, 0);
    spectator_delete(spectator);
}

addr_t *spectator_get_addr(spectator_t *spectator)
{
    return spectator->connection_info;
}
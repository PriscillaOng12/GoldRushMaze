#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "grid.h"
#include "message.h"

typedef struct spectator spectator_t;

spectator_t* spectator_new(addr_t* connection_info);

void spectator_delete(spectator_t* spectator);

void spectator_quit(spectator_t* spectator, grid_t* grid);

addr_t* spectator_get_addr(spectator_t* spectator);
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "message.h"
// #include "player.h"
// #include "spectator.h"

typedef struct grid grid_t;
typedef struct player player_t;
typedef struct spectator spectator_t;

grid_t* grid_load(FILE* file);
int grid_init_gold(grid_t* grid);
void grid_delete(grid_t* grid);
void grid_spawn_player(grid_t* grid, addr_t* connection_info, char* real_name);
void grid_spawn_spectator(grid_t* grid, spectator_t* spectator);
void grid_send_state(grid_t* grid, player_t* player);
void grid_send_state_spectator(grid_t* grid, spectator_t* spectator);
void grid_game_over(grid_t* grid);
int grid_getnrows(grid_t* grid);
int grid_getncols(grid_t* grid);
char** grid_getcells(grid_t* grid);
int** grid_getnuggets(grid_t* grid);
int grid_getnuggetcount(grid_t* grid);
int grid_getplayercount(grid_t* grid);
player_t** grid_getplayers(grid_t* grid);
spectator_t* grid_getspectator(grid_t* grid);
void grid_setnuggets(grid_t* grid, int i, int j, int n);
void grid_setnuggetcount(grid_t* grid, int count);

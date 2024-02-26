#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

typedef struct grid grid_t;



grid_t* grid_load(FILE* map_file);
int grid_init_gold(grid_t* grid);
// void grid_spawn_player(grid_t* grid, player_t* player);
// void grid_spawn_spectator(grid_t* grid, spectator_t* spectator);
// void grid_send_state(player_t* player);
// void grid_send_state_spectator(spectator_t* spectator);
// void grid_game_over(grid_t* grid, player_t* players[], int num_players);
void grid_delete(grid_t* grid);
int grid_getnrows(grid_t* grid);
int grid_getncols(grid_t* grid);
int grid_getplayers(grid_t* grid);
char** grid_getcells(grid_t* grid);
int** grid_getnuggets(grid_t* grid);

#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "grid.h"
#include "mem.h"
#include "message.h"

typedef struct player player_t;

player_t* player_new(addr_t* connection_info, char* real_name, int x, int y, int nrows, int ncols);

void player_update_visibility(player_t* player, grid_t* grid);

void player_moveto(player_t* player, int x, int y);

void player_delete(player_t* player, grid_t* grid);

void player_collect_gold(player_t* player, grid_t* grid, int gold_x, int gold_y);

bool player_move(player_t* player, grid_t* grid, int dx, int dy);

void player_quit(player_t* player, grid_t* grid);

char* player_get_name(player_t* player);

addr_t* player_get_addr(player_t* player);

int player_get_x(player_t* player);

int player_get_y(player_t* player);

int player_get_purse(player_t* player);

int** player_get_visibility(player_t* player);

void player_set_visibility(player_t* player, int x, int y, int val);

#endif //__PLAYER_H

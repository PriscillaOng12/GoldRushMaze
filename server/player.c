/* 
 * player.c - 'player' module
 *
 * see player.h for more information.
 *
 * ctrl-zzz, Winter 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "player.h"
#include "grid.h"
#include "message.h"

int MaxNameLength = 50;   // max number of chars in playerName

static int min(int a, int b);
static int max(int a, int b);

typedef struct player {
    char* real_name;
    addr_t* connection_info;
    int* purse;
    int* x;
    int* y;
    int** visibility;
} player_t;

player_t* player_new(addr_t* connection_info, char* real_name, int x, int y, int nrows, int ncols)
{
    player_t* player = (player_t*) malloc(sizeof(player_t));
    char* copied_name = (char*) malloc(sizeof(char) * (MaxNameLength + 1)); // truncate is handled by message processing
    strcpy(copied_name, real_name); // grid is allowed to free "REAL NAME" once sent
    player->real_name = copied_name;
    player->visibility = (int**) calloc(nrows, sizeof(int*));
    for (int i=0; i<nrows; i++) {
        player->visibility[i] = (int*) calloc(ncols, sizeof(int));
    }
    player->connection_info = connection_info;
    player->x = malloc(sizeof(int));
    player->y = malloc(sizeof(int));
    player->purse = malloc(sizeof(int));
    *(player->x) = x;
    *(player->y) = y;
    *(player->purse) = 0;
    return player;
}

void player_update_visibility(player_t* player, grid_t* grid)
{
    int x = *(player->x);
    int y = *(player->y);
    char** map = grid_getcells(grid);
    double cx;
    double cy;
    int cx_floor;
    int cx_ceil;
    int cy_floor;
    int cy_ceil;
    bool flag = false;
    if (map[x][y] != '.') {
        for (int i = 0; i < grid_getnrows(grid); i++) {
            for (int j = 0; j < grid_getncols(grid); j++) {
                if (player->visibility[i][j] == 1) {
                    player_set_visibility(player, i, j, 2);
                }
            }
        }
        if (map[x][y] == "#") {
            int count = 0;
            for (int dx = -1; dx <= 1; dx = dx + 2) {
                for (int dy = -1; dy <= 1; dy = dy + 2) {
                    if (map[x+dx][y+dy] == "#") {
                        count += 1;
                    }
                }
            }
            if (count == 1) {
                flag = true;
            } else {
                // VISIBILITY IS JUST THE STUFF OUTSIDE.
            }
        }
    } else {
        flag = true; // TODO: THIS IS A DIFFERENT ALGO, NOT THE STANDARD ONE. FIX!
    }
    if (flag == true) {
        for (int i = 0; i < grid_getnrows(grid); i++) {
            for (int j = 0; j < grid_getncols(grid); j++) {
                if (player->visibility[i][j] == 1) {
                    player_set_visibility(player, i, j, 2);
                }
                bool visible = true;
                for (int dx = min(x, i) + 1; dx < max(x, i); dx++) {
                    cy = y + (y - j) / (x - i) * (dx - x);
                    cy_floor = floor(cy);
                    cy_ceil = ceil(cy);
                    if (map[dx][cy_floor] != '.' || map[dx][cy_ceil] != '.') {
                        visible = false;
                        break;
                    }
                }
                for (int dy = min(y, j) + 1; dy < max(y, j); dy++) {
                    cx = x + (x - i) / (y - j) * (dy - y);
                    cx_floor = floor(cx);
                    cx_ceil = ceil(cx);
                    if (map[cx_floor][dy] != '.' || map[cx_ceil][dy] != '.') {
                        visible = false;
                        break;
                    }
                }
                if (visible) {
                    player_set_visibility(player, i, j, 1);
                }
            }
        }
    }
}

void player_moveto(player_t* player, int x, int y)
{
    *(player->x) = x;
    *(player->y) = y;
}

void player_delete(player_t* player, grid_t* grid)
{
    free(player->real_name);
    free(player->x);
    free(player->y);
    free(player->purse);
    int nr = grid_getnrows(grid);
    for (int i=0; i<nr; i++) {
        free(player->visibility[i]);
    }
    free(player->visibility);
    free(player);
}

// getter functions
char* player_get_name(player_t* player)
{
    return player->real_name;
}

addr_t* player_get_addr(player_t* player)
{
    return player->connection_info;
}

int player_get_x(player_t* player)
{
    return *(player->x);
}

int player_get_y(player_t* player)
{
    return *(player->y);
}

int player_get_purse(player_t* player)
{
    return *(player->purse);
}

int** player_get_visibility(player_t* player)
{
    return player->visibility;
}

void player_set_visibility(player_t* player, int x, int y, int val) {
    player->visibility[x][y] = val;
}

static int min(int a, int b)
{
    return a > b ? b : a;
}

static int max(int a, int b)
{
    return a > b ? a : b;
}
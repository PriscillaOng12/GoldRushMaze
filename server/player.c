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
    player_t* player = (player_t*) malloc(sizeof(player));
    char* copied_name = (char*) malloc(strlen(real_name) * sizeof(char));
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
    for (int i = 0; i < grid_getnrows(grid); i++) {
        for (int j = 0; j < grid_getncols(grid); j++) {
            if (player->visibility[i][j] == 1) {
                player->visibility[i][j] == 2;
            }
        }
    }
    int x = *(player->x);
    int x = *(player->y);
    double cx;
    double cy;
    int cx_floor;
    int cx_ceil;
    int cy_floor;
    int cy_ceil;
    for (int i = 0; i < grid_getnrows(grid); i++) {
        for (int j = 0; j < grid_getncols(grid); j++) {
            bool visible = true;
            for (int dx = min(x, i) + 1; dx < max(x, i); dx++) {
                cy = y + (y - j) / (x - i) * (dx - x);
                cy_floor = floor(cy);
                cy_ceil = ceil(cy);
                if (grid_getmap[cx][cy_floor] != "." or grid_getmap[cx][cy_ceil] != ".") {
                    visible = false;
                    break;
                }
            }
            if (visible) {
                for (int dy = min(y, j) + 1; dy < max(y, j); dy++) {
                    cx = x + (x - i) / (y - j) * (dy - y);
                    cy_floor = floor(cy);
                    cy_ceil = ceil(cy);
                    if (grid_getmap[cx][cy_floor] != "." or grid_getmap[cx][cy_ceil] != ".") {
                        visible = false;
                        break;
                    }
                }
            }
            if visible {
                player->visibility[i][j] = 1
            } 
        }
    }
}
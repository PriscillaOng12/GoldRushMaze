/*
 * playertest.c - testing player module 
 *
 * ctrl-zzz, Winter 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "grid.h"
#include "player.h"
#include "spectator.h"
#include "file.h"
#include "sys/types.h"

static void print_curr_state(int** res, char** map, int nr, int nc, player_t* player, grid_t* grid);
static void print_map(char** map, int nr, int nc, int** nuggets);

int main(int argc, char* argv[]) {
    srand(getpid());
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    grid_t* grid = grid_load(file);
      if (grid == NULL) {
        fclose(file);
         printf("Failed to load grid.\n");
        return EXIT_FAILURE;
    }

    grid_init_gold(grid);
    int nr = grid_getnrows(grid);
    int nc = grid_getncols(grid);
    
    grid_spawn_player(grid, message_noAddr(), "tester");
    player_t* player = grid_getplayers(grid)[0];
    int** res =  player_get_visibility(player);
    char** map = grid_getcells(grid);
    print_map(map, nr, nc, grid_getnuggets(grid));
    int int1;
    int int2;
    printf("Enter move coords\n");
    while (scanf("%d %d", &int1, &int2) == 2) {
        player_move(player, grid, int1, int2);
        print_curr_state(res, map, nr, nc, player, grid);
        printf("Enter move coords\n");
    }
    grid_game_over(grid);
    fclose(file);
    return EXIT_SUCCESS;
}

static void print_curr_state(int** res, char** map, int nr, int nc, player_t* player, grid_t* grid) {
    int flag;
    player_t** players = grid_getplayers(grid);
    for (int i = 0; i < nr; i++) {
        for (int j = 0; j < nc; j++) {
            if (i == player_get_x(player) && j == player_get_y(player)) {
                printf("@");
            } else {
                if (res[i][j] == 0) {
                    printf(" ");
                } else if (res[i][j] == 1) {
                    flag = -1;
                    for (int k = 0; k < grid_getplayercount(grid); k++) {
                        if (player_get_isactive(players[k])) {
                            if (player_get_x(players[k]) == i  && player_get_y(players[k]) == j) {
                                flag = k;
                                break;
                            }
                        }
                    }
                    if (flag > -1) {
                        printf("\e[0;33m%c\e[0m", 65+flag);
                    } else if (grid_getnuggets(grid)[i][j] > 0) {
                        printf("\e[0;31m*\e[0m");
                    } else {
                        printf("\e[0;31m%c\e[0m", map[i][j]);
                    }
                } else {
                    printf("%c", map[i][j]);
                }
            }
        }
        printf("\n");
    }
    printf("-------------------------------------------------------------------\n");
}

static void print_map(char** map, int nr, int nc, int** nuggets) {
    for (int i = 0; i < nr; i++) {
        for (int j = 0; j < nc; j++) {
            if (nuggets[i][j] > 0) {
                printf("*");
            } else {
                printf("%c", map[i][j]);
            }
        }
        printf("\n");
    }
    printf("-------------------------------------------------------------------\n");
}
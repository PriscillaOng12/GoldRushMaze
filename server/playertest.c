#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grid.h"
#include "player.h"
#include "file.h"

static void print_curr_state(int** res, char** map, int nr, int nc, player_t* player, grid_t* grid);
static void print_map(char** map, int nr, int nc);

int main(int argc, char* argv[]) {
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

    grid_spawn_player(grid, NULL, "tester2");
    grid_spawn_player(grid, NULL, "tester3");
    player_t* player = grid_getplayers(grid)[0];
    player_t* player_2 = grid_getplayers(grid)[1];
    // player_t* player2 = grid_getplayers(grid)[1];
    int** res =  player_get_visibility(player);
    char** map = grid_getcells(grid);
    print_map(map, nr, nc);
    printf("New position: (%d, %d)\n", player_get_x(player), player_get_y(player));
    print_curr_state(res, map, nr, nc, player, grid);
    int int1;
    int int2;
    while (scanf("%d %d", &int1, &int2) == 2) {
        player_move(player, grid, int1, int2);
        player_update_visibility(player, grid);
        print_curr_state(res, map, nr, nc, player, grid);
    }
    player_quit(player_2, grid);
    rewind(stdin);
    while (scanf("%d %d", &int1, &int2) == 2) {
        player_move(player, grid, int1, int2);
        player_update_visibility(player, grid);
        print_curr_state(res, map, nr, nc, player, grid);
    }
    grid_delete(grid);
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
                        if (players[k] != NULL) {
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

static void print_map(char** map, int nr, int nc) {
    for (int i = 0; i < nr; i++) {
        for (int j = 0; j < nc; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }
    printf("-------------------------------------------------------------------\n");
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grid.h"
#include "player.h"
#include "file.h"

static void print_curr_state(int** res, char** map, int nr, int nc, player_t* player);
static void print_map(char** map, int nr, int nc);
static void print_res(int** map, int nr, int nc);

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

    player_t* player = player_new(NULL, "tester", 13, 13, nr, nc);
    player_update_visibility(player, grid);
    int** res =  player_get_visibility(player);
    char** map = grid_getcells(grid);
    print_map(map, nr, nc);
    print_curr_state(res, map, nr, nc, player);
    int int1;
    int int2;
    while (scanf("%d %d", &int1, &int2) == 2) {
        player_moveto(player, int1, int2);
        player_update_visibility(player, grid);
        print_curr_state(res, map, nr, nc, player);
    }
    player_delete(player, grid);
    grid_delete(grid);
    fclose(file);
    return EXIT_SUCCESS;
}

static void print_curr_state(int** res, char** map, int nr, int nc, player_t* player) {
    for (int i = 0; i < nr; i++) {
        for (int j = 0; j < nc; j++) {
            if (i == player_get_x(player) && j == player_get_y(player)) {
                printf("@");
            } else {
                if (res[i][j] == 0) {
                    printf(" ");
                } else if (res[i][j] == 1) {
                    printf("\e[0;31m%c\e[0m", map[i][j]);
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
    printf("Info: (%d, %d)\n", nr ,nc);
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grid.h"
#include "player.h"
#include "file.h"

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
    int nr = grid_getnrows(grid);
    int nc = grid_getncols(grid);

    player_t* player = player_new(NULL, "tester", 3, 4, nr, nc);

    player_update_visibility(player, grid);
    int** res =  player_get_visibility(player);
    char** map = grid_getcells(grid);
    for (int i = 0; i < nr; i++) {
        for (int j = 0; j < nc; j++) {
            printf("%c", (res[i][j] > 0 ? map[i][j] : ' '));
        }
        printf("\n");
    }
    printf("----------------------------------------\n");
    player_moveto(player, 13, 13);
    player_update_visibility(player, grid);
    for (int i = 0; i < nr; i++) {
        for (int j = 0; j < nc; j++) {
            printf("%c", (res[i][j] > 0 ? map[i][j] : ' '));
        }
        printf("\n");
    }
    player_delete(player, grid);
    grid_delete(grid);
    fclose(file);
    return EXIT_SUCCESS;
}
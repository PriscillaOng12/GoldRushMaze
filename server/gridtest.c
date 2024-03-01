#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "grid.h"
#include "player.h"

int main(int argc, char* argv[]) {
    // Check if a filename has been provided
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open the file
    FILE* file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Load the grid from the file
    grid_t* grid = grid_load(file);
      if (grid == NULL) {
        fclose(file);
         printf("Failed to load grid.\n");
        return EXIT_FAILURE;
    }


    printf("Initial grid state:\n");
    for (int i = 0; i < grid_getnrows(grid); i++) {
    for (int j = 0; j < grid_getncols(grid); j++) {
        putchar(grid_getcells(grid)[i][j]);
    }
    putchar('\n');
    }
  
    srand(time(NULL)); // Seed the random number generator once
    
    printf("Before gold placed: rows: %d | columns: %d \n", grid_getnrows(grid), grid_getncols(grid));
    int goldPlaced = grid_init_gold(grid); // Place gold in the grid
    printf("Gold placed: %d\n", goldPlaced); // Optional: print how many g

    printf("Grid state after placing gold:\n");
    for (int i = 0; i < grid_getnrows(grid); i++) {
        for (int j = 0; j < grid_getncols(grid); j++) {
            if (grid_getnuggets(grid)[i][j] > 0) {
                putchar('*');
            } else {
                putchar(grid_getcells(grid)[i][j]);
            }
        }
        putchar('\n');
    }

    printf("AT THE END: rows: %d | columns: %d \n", grid_getnrows(grid), grid_getncols(grid));

        // Test cases
    printf("\n--- Starting Test Cases ---\n");
    // Test spawning a player
    printf("Testing spawning a player...\n");
    
    addr_t* test_connection_info = {NULL};
    char* testName = "player one"  ; 
    spectator_t* test_spectator = spectator_new(test_connection_info);
    grid_spawn_player(grid, test_connection_info, testName);

    // Test spawning a spectator
    printf("\nTesting spawning a spectator...\n");
    grid_spawn_spectator(grid, test_spectator);

    // Optionally, you can add more scenarios here to test other functionalities

    // Test game quit scenario
    printf("\nTesting game quit scenario...\n");
    grid_game_over(grid);
    
     // Clean up and close the file
    fclose(file);
    printf("its over.\n");

    return EXIT_SUCCESS;
}
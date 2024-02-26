#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Assuming grid_t is defined in "grid.h"
#include "grid.h"
#include "file.h"



// Prototype for the grid_load function
grid_t* grid_load(FILE* file);

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
    for (int i = 0; i < *(grid->rows); i++) {
    for (int j = 0; j < *(grid->columns); j++) {
        putchar(grid->cells[i][j]);
    }
    putchar('\n');
    }

  
    srand(time(NULL)); // Seed the random number generator once
    
    printf("Before gold placed: rows: %d | columns: %d \n", *grid->rows, *grid->columns);
    int goldPlaced = grid_init_gold(grid); // Place gold in the grid
    printf("Gold placed: %d\n", goldPlaced); // Optional: print how many g

    printf("Grid state after placing gold:\n");
    for (int i = 0; i < *grid->rows; i++) {
        for (int j = 0; j < *grid->columns; j++) {
            putchar(grid->cells[i][j]);
        }
        putchar('\n');
    }

    printf("AT THE END: rows: %d | columns: %d \n", *grid->rows, *grid->columns);


    // for (int i = 0; i < grid->rows; i++) {
    //     printf("%d: %s\n", i, grid->cells[i]);
    // }

    print_nugget_array(grid);

    // Clean up and close the file
    
    grid_delete(grid);
    fclose(file);
    printf("its over.\n");

    return EXIT_SUCCESS;
}
/*
* grid.c - CS50 'grid' module
*
* see grid.h for more information.
*
* Eliana Stanford, Winter 2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "grid.h"
#include "player.h"
#include "spectator.h"
#include "mem.h"
#include "../support/message.h"


typedef struct grid {
   char** cells;
   int** nuggets;
   player_t** players;
   int* rows;
   int* columns;
   int* playerCount;
   int* nuggetCount;
} grid_t;


grid_t* grid_load(FILE* file) {
    grid_t* grid = (grid_t*)malloc(sizeof(grid_t));
    if (grid == NULL) {
        printf("Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    grid->rows = (int*)malloc(sizeof(int));
    grid->columns = (int*)malloc(sizeof(int));

    if (grid->rows == NULL || grid->columns == NULL) { // TODO: check after each; could leak if rows allocs but cols don't
        printf("Memory allocation for rows or columns failed.\n");
        free(grid);
        exit(EXIT_FAILURE);
    }

    grid->cells = NULL;
    grid->nuggets = NULL;

    int rows = 0;
    int cols = 0;
    int max_cols = 0;
    char c;

    // Iterate through the file to determine the number of rows and columns
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            rows++;                 // Increment row count when encountering a newline character
            if (cols > max_cols) {  // Update max_cols if necessary
                max_cols = cols;
            }
            cols = 0;               // Reset column count for the next row
        } else {
            cols++;                 // Increment column count for each character
        }
    }

    // Handle the case when the last line doesn't end with '\n'
    if (cols > 0) {
        rows++;                     // Increment row count for the last line
        if (cols > max_cols) {      // Update max_cols if necessary
            max_cols = cols;
        }
    }

    // Assign the determined rows and columns to the grid structure
    *(grid->rows) = rows;
    *(grid->columns) = max_cols;

    // Allocate memory for the grid cells
    grid->cells = (char**)calloc(*grid->rows, sizeof(char*));
    if (grid->cells == NULL) {
        printf("Memory allocation failed.\n");
        free(grid->rows);
        free(grid->columns);
        free(grid); // Cleanup
        exit(EXIT_FAILURE);
    }

    // Allocate memory for each row of grid cells
    for (int i = 0; i < *grid->rows; i++) {
        grid->cells[i] = (char*) calloc(*grid->columns, sizeof(char));
        if (grid->cells[i] == NULL) {
            printf("Memory allocation failed.\n");
             for (int j = 0; j < i; j++) { // Free any rows that were successfully allocated
                free(grid->cells[j]);
            }
            free(grid->cells);
            free(grid->rows);
            free(grid->columns);
            free(grid);
            exit(EXIT_FAILURE);
        }
    }

    // Allocate memory for the nugget counts
    grid->nuggets = (int**)calloc(*grid->rows, sizeof(int*));
    if (grid->nuggets == NULL) {
        printf("Memory allocation for nuggets failed.\n");
        for (int i = 0; i < *grid->rows; i++) {
            free(grid->cells[i]);
        }
        free(grid->cells);
        free(grid->rows);
        free(grid->columns);
        free(grid); // Cleanup
        return NULL;
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *grid->rows; i++) {
        grid->nuggets[i] = (int*)calloc(*grid->columns, sizeof(int));
        if (grid->nuggets[i] == NULL) {
            printf("Memory allocation for nuggets failed.\n");
            for (int j = 0; j < *grid->rows; j++) {
                free(grid->cells[j]);
            }
            free(grid->cells);
            // Cleanup previously allocated rows in nuggets
            for (int j = 0; j < i; j++) {
                free(grid->nuggets[j]);
            }
            free(grid->nuggets);
            free(grid->rows);
            free(grid->columns);
            free(grid); // Cleanup

            exit(EXIT_FAILURE);
        }
        // Initialize all counts to 0 without using memset
        for (int j = 0; j < *grid->columns; j++) {
            grid->nuggets[i][j] = 0;
        }
    }


    rewind(file);   // Reset file pointer to the beginning of the file

   // Read the map file line by line and parse each character to form the grid
    for (int i = 0; i < *grid->rows; i++) {
        int j = 0;
        while ((c = fgetc(file)) != EOF && c != '\n') {
            if (j < *grid->columns) {
                grid->cells[i][j] = c;
                j++;
            }
        }
        // Fill any remaining cells in the row with the default character if the row is shorter than max_cols
        for (; j < *grid->columns; j++) {
            grid->cells[i][j] = '.';
        }
    }
    grid->playerCount = (int*)malloc(sizeof(int)); // need to free this on error
    grid->nuggetCount = (int*)malloc(sizeof(int)); // need to free this on error
    grid->players = (player_t**) calloc(26, sizeof(player_t*)); // need to free this on error
    return grid;    // Return the grid structure
}

int grid_init_gold(grid_t* grid) {
    printf("Before placing gold: Rows = %d, Columns = %d\n", *grid->rows, *grid->columns);

   printf("Initializing gold placement...\n");

   int GoldTotal = 250;
   int GoldMinNumPiles = 10;
   int GoldMaxNumPiles = 30;
   srand(time(NULL));


   // Calculate the number of gold piles to be dropped within a certain area
   int num_gold = GoldMinNumPiles + rand() % (GoldMaxNumPiles - GoldMinNumPiles + 1);
   printf("Calculated number of gold piles: %d\n", num_gold);


   if (num_gold * 1 > GoldTotal) {
       num_gold = GoldTotal;
   }
  
   printf("Adjusted number of gold piles (if necessary): %d\n", num_gold);

   // Drop gold nugget piles with at least 1 nugget per pile
  
   int gold_placed = 0;
   while (gold_placed < num_gold) {
       int x = rand() % *grid->rows;
       int y = rand() % *grid->columns;
        printf("Attempting to place gold at (%d, %d)...\n", x, y);


       if (grid->cells[x][y] == '.') {
           grid->nuggets[x][y] += 1; // increment nugget count
           gold_placed++;
           printf("Gold placed at (%d, %d). Total placed: %d\n", x, y, gold_placed);
       }


       else {
           printf("Cell (%d, %d) not empty. Skipping...\n", x, y);
       }
   }
    
    printf("After placing gold: Rows = %d, Columns = %d\n", *grid->rows, *grid->columns);

   printf("Gold placement complete. Total gold piles placed: %d\n", gold_placed);
   for (int i = 0; i < *grid->rows; i++) {
       for (int j = 0; j < *grid->columns; j++) {
            (*grid->nuggetCount)++;
        }
   }
   return num_gold;
}


void grid_delete(grid_t* grid) {
   // Deallocate memory associated with the game grid
   for (int i = 0; i < *grid->rows; i++) {
       free(grid->cells[i]);
       grid->cells[i] = NULL;
   }
    free(grid->cells);


    for (int i = 0; i < *grid->rows; i++) {
       free(grid->nuggets[i]);
        grid->nuggets[i] = NULL;
   }
    free(grid->nuggets);


   free(grid->rows); // Free the allocated memory for rows
   free(grid->columns); // Free the allocated memory for columns
   free(grid->nuggetCount);
   free(grid->playerCount);
   free(grid->players);

   free(grid);
}


void grid_spawn_player(grid_t* grid, addr_t* connection_info, char* real_name) {
    // Calculate random spot on the grid
    srand(time(NULL));
    int x;
    int y;

   while (1) {
        x = rand() % *grid->rows;
        y = rand() % *grid->columns;
      
        if (grid->cells[x][y] == '.') { // if its gold
            // Place new player with new symbol
            player_t* new_player = player_new(connection_info, real_name, x, y, *grid->rows, *grid->columns);
            grid->players[*(grid->playerCount)++] = new_player; // add the player to the player arraay
            break; // Exit the loop once a valid spot is found
        }
    }
}

void grid_spawn_spectator(grid_t* grid, spectator_t* spectator) {
    static spectator_t* current_spectator = NULL;
   // if there is already a spectator kick them off
   if(current_spectator != NULL){
        spectator_quit(current_spectator, grid);
    }
    current_spectator = spectator;
}

void grid_send_state(grid_t* grid, player_t* player) {
    char* message = malloc(((*grid->rows + 1) * (*grid->columns) + 1) * sizeof(char*));
    char* moving_ptr = message; // index to iterate through message string
    int** visibility = player_get_visibility(player);
    char** message_vis = (char**) mem_assert(calloc(*grid->rows, sizeof(char*)), "Error allocating space for message grid");
    int k;
    for (k = 0; k < *grid->rows; k++) {
        message_vis[k] = mem_assert(calloc(*grid->columns, sizeof(char*)), "Error allocating space for player message");
    }
    int px, py;
    for (k = 0; k < *grid->playerCount; k++) {
        if (grid->players[k] != NULL) {
            px = player_get_x(grid->players[k]);
            py = player_get_y(grid->players[k]);
            message_vis[px][py] = (char) (65 + k);
        }
    }

    for (int i = 0; i < *grid->rows; i++) {
       for (int j = 0; j < *grid->columns; j++) {
            if (visibility[i][j] == 1 && (message_vis[px][py] < 65 || message_vis[px][py] > 90)) {
                if (grid->nuggets[i][j] > 0) {
                    message_vis[i][j] = '*';
                } else {
                    message_vis[i][j] = grid->cells[i][j];
                }
                *moving_ptr = message_vis[i][j];
            } else if (visibility[i][j] == 2) {
                *moving_ptr = grid->cells[i][j];
            } else {
                *moving_ptr = ' ';
            }
            moving_ptr++;
        }
        *moving_ptr = '\n';
        moving_ptr++;
    }

    *moving_ptr = '\0';

    addr_t* address = player_get_addr(player);
    message_send(*address, message);
    for (int k = 0; k < *grid->playerCount; k++) {
        free(message_vis[k]);
    }
    free(message_vis);
    free(message);
}
    
    
void grid_send_state_spectator(grid_t* grid, spectator_t* spectator) {
    char* message = malloc(((*grid->rows + 1) * (*grid->columns) + 1) * sizeof(char*));
    char* moving_ptr = message; // index to iterate through message string
    char** message_vis = (char**) mem_assert(calloc(*grid->rows, sizeof(char*)), "Error allocating space for message grid");
    int k;
    for (k = 0; k < *grid->rows; k++) {
        message_vis[k] = mem_assert(calloc(*grid->columns, sizeof(char*)), "Error allocating space for player message");
    }
    int px, py;
    for (k = 0; k < *grid->playerCount; k++) {
        if (grid->players[k] != NULL) {
            px = player_get_x(grid->players[k]);
            py = player_get_y(grid->players[k]);
            message_vis[px][py] = (char) (65 + k);
        }
    }

    for (int i = 0; i < *grid->rows; i++) {
       for (int j = 0; j < *grid->columns; j++) {
            if ((message_vis[px][py] < 65 || message_vis[px][py] > 90)) {
                if (grid->nuggets[i][j] > 0) {
                    message_vis[i][j] = '*';
                } else {
                    message_vis[i][j] = grid->cells[i][j];
                }
                *moving_ptr = message_vis[i][j];
            } else {
                *moving_ptr = grid->cells[i][j];
            }
            moving_ptr++;
        }
        *moving_ptr = '\n';
        moving_ptr++;
    }

    *moving_ptr = '\0';

    addr_t* address = spectator_get_addr(spectator);
    message_send(*address, message);
    for (int k = 0; k < *grid->playerCount; k++) {
        free(message_vis[k]);
    }
    free(message_vis);
    free(message);
}


void grid_game_over(grid_t* grid) {
    char* message = (char*)malloc(129 * sizeof(char));

    for (int i = 0; i < *grid->playerCount; i++) {
        int purse = player_get_purse(grid->players[i]);
        char* name = player_get_name(grid->players[i]);
        addr_t* playerAddress = player_get_addr(grid->players[i]);

        // Calculate the final score and create a summary containing purse contents, score, and name
        sprintf(message, "Game Over\nPlayer %s - Score: %d, Nuggets: %d\n", name, purse * 100, purse);
        message_send(*playerAddress, message);
    }
    grid_delete(grid);
}


int grid_getnrows(grid_t* grid) {
    return *grid->rows;
}

int grid_getncols(grid_t* grid) {
    return *grid->columns;
}

char** grid_getcells(grid_t* grid) {
    return grid->cells;
}

int** grid_getnuggets(grid_t* grid) {
    return grid->nuggets;
}

int grid_getnuggetcount(grid_t* grid) {
    return *grid->nuggetCount;
}

int grid_getplayercount(grid_t* grid) {
    return *grid->playerCount;
}

player_t** grid_getplayers(grid_t* grid) {
    return grid->players;
}

void grid_setnuggets(grid_t* grid, int i, int j, int n) {
    grid->nuggets[i][j] = n;
}

void grid_setnuggetcount(grid_t* grid, int count) {
    *grid->nuggetCount = count;
}


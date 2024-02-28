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
#include "message.h"


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


// void grid_spawn_player(grid_t* grid, addr_t* connection_info, char* real_name) {
//     // Calculate random spot on the grid
//     srand(time(NULL));
//     int x;
//     int y;

//    while (1) {
//         x = rand() % *grid->rows;
//         y = rand() % *grid->columns;
      
//         if (grid->cells[x][y] == '.') { // if its gold
//             // Place new player with new symbol
//             player_t* new_player = player_new(connection_info, real_name, x, y, *grid->rows, *grid->columns);
//             grid->players[*(grid->playerCount)++] = new_player; // add the player to the player arraay
//             break; // Exit the loop once a valid spot is found
//         }
//     }
// }

void grid_spawn_spectator(spectator_t* spectator) {
    static spectator_t* current_spectator = NULL;
   // if there is already a spectator kick them off
   if(current_spectator != NULL){
        spectator_quit( current_spectator);
    }
    current_spectator = new_spectator;
}

void grid_spawn_spectator(spectator_t* spectator) {
    static spectator_t* current_spectator = NULL;
   // if there is already a spectator kick them off
   if(current_spectator != NULL){
        spectator_quit( current_spectator);
    }
    current_spectator = new_spectator;
}

void grid_send_state(player_t* player) {
    char[100] message = (char*)malloc((*grid->row + 1)*(*grid->columns), sizeof(char*));
    int messageIndex = 0;// index to iterate through message string
    int** visibility = player_get_visibility(player);

     // every time you sstring copy over and then string copy a new line over 
    for (int i = 0; i < *grid->rows; i++) {
       for (int j = 0; j < *grid->columns; j++) {

        // iterate through every cell in and add to visible grid in message string
         if(visibility[i][j]= 1){
            char cellContents = grid->cells[i][j]; 
            
            // check if this is another player, if so put itsconvert askii to letter
            if(cellContents >64 && cellContents <=90){
                cellContents = (char)cellValue;
            }

       // Check for nuggets or obstacles if no player is found
            else if (grid->cells[i][j] == '#') {
                cellContents = '#'; // wall
            } else if (grid->cells[i][j] == '*') {
                cellContents = '*'; // nugget
            } else if (grid->cells[i][j] == '|') {
                cellContents = '|'; // border
            } else if (grid->cells[i][j] == '-') {
                cellContents = '-'; // border
            }
            else if (grid->cells[i][j] == '.') {
                cellContents = '.'; // border
            }

            // Add the cell content to the message
           message[messageIndex++] = cellContents;

            } 
            else {
                message[messageIndex++]= ' '; // Cell is not visible
            }

        }
        message[messageIndex++] = '\n'; // New line at the end of each row
    }
    message[messageIndex] = '\0'; // Null-terminate the string

    addr_t address = player_get_adress(player);
    message_send(address, message);

    free(message);
}
    
    
void grid_send_state_spectator(spectator_t* spectator) {
    char[500] message = (char*)malloc((*grid->row + 1)*(*grid->columns), sizeof(char*));
    int messageIndex = 0;// index to iterate through message string

     // every time you sstring copy over and then string copy a new line over 
    for (int i = 0; i < *grid->rows; i++) {
       for (int j = 0; j < *grid->columns; j++) {
        // iterate through every cell in and add to  grid in message string
            char cellContents = grid->cells[i][j]; 
            
            // check if this is another player, if so put itsconvert askii to letter
            if(cellContents >64 && cellContents <=90){
                cellContents = (char)cellValue;
            }

       // Check for nuggets or obstacles if no player is found
            else if (grid->cells[i][j] == '#') {
                cellContents = '#'; // wall
            } else if (grid->cells[i][j] == '*') {
                cellContents = '*'; // nugget
            } else if (grid->cells[i][j] == '|') {
                cellContents = '|'; // border
            } else if (grid->cells[i][j] == '-') {
                cellContents = '-'; // border
            }
            else if (grid->cells[i][j] == '.') {
                cellContents = '.'; // border
            }

            // Add the cell content to the message
           message[messageIndex++] = cellContents;
        }
        message[messageIndex++] = '\n'; // New line at the end of each row
    }
      message[messageIndex] = '\0'; // Null-terminate the string

    addr_t address = spectator_get_adress(spectator);
    message_send(address, message);
    free(message);
}




void grid_game_over(grid_t* grid) {
    char[500] message = (char*)malloc((*grid->row + 1)*(*grid->columns), sizeof(char*));

    printf("Game Over\n");
    for (int i = 0; i < *grid->playerCount; i++) {
        int purse = player_get_purse(grid->players[i]);
        char* name = player_get_name(grid->players[i]);
        addr_t playerAddress = player_get_address(grid->players[i]);

        // Calculate the final score and create a summary containing purse contents, score, and name
        snprintf(message, sizeof(message), "Game Over\nPlayer %s - Score: %d, Nuggets: %d\n", name, purse * 100, purse);
        message_send(playerAddress, message);
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


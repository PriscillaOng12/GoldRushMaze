/* 
 * grid.h - header file for 'grid' module
 * 
 * A "grid" is a data structure used to represent a two-dimensional game board for a multiplayer game with elements of exploration,
 * resource gathering (specifically gold nuggets) and  competition among players. 
 *
 * ctrl-zzz, Winter 2024
 */

#ifndef GRID_H
#define GRID_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "message.h"

/**************** global types ****************/
typedef struct grid grid_t;
typedef struct player player_t;
typedef struct spectator spectator_t;


/**************** functions ****************/
/***************** grid_load *****************/
/* Load the grid from a file.
 *
 * Caller provides:
 *   a file pointer to an open file containing the grid layout.
 * We guarantee:
 *   a new grid object is returned if file reading and memory allocation are successful.
 *   returns NULL if an error occurs during file reading or memory allocation.
 * Notes:
 *   the caller is responsible for opening and closing the file.
 *   the grid is initialized with cells, players, and nuggets based on the file content.
 */
grid_t* grid_load(FILE* file);

/***************** grid_init_gold *****************/
/* Initialize gold nuggets randomly within the grid.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   gold nuggets are placed randomly across the grid.
 * Notes:
 *   modifies the grid's nugget configuration.
 *   modifies the total nugget count of the grid
 */
void grid_init_gold(grid_t* grid);

/***************** grid_delete *****************/
/* Delete the grid and free associated resources.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   all memory allocated for the grid and its components is freed.
 * Notes:
 *   assumes all players and spectators are properly deleted or handled.
 */
void grid_delete(grid_t* grid);

/***************** grid_spawn_player *****************/
/* Spawn a new player in the grid.
 *
 * Caller provides:
 *   a valid grid object, connection information, and the player's real name.
 * We guarantee:
 *   a new player is added to the grid in a random empty position.
 * Notes:
 *   updates the player count and initializes player visibility.
 */
void grid_spawn_player(grid_t* grid, const addr_t connection_info, char* real_name);

/***************** grid_spawn_spectator *****************/
/* Add a spectator to the grid.
 *
 * Caller provides:
 *   a valid grid object and a spectator object.
 * We guarantee:
 *   replaces any existing spectator with the new one.
 * Notes:
 *   maintains at most one spectator at a time in the grid.
 */
void grid_spawn_spectator(grid_t* grid, spectator_t* spectator);

/***************** grid_send_state *****************/
/* Send the grid state to a specific player.
 *
 * Caller provides:
 *   a valid grid object and a player object.
 * We guarantee:
 *   returns a string representation of the grid from the player's perspective.
 * Notes:
 *   the caller is responsible for managing the memory of the returned string.
 */
char* grid_send_state(grid_t* grid, player_t* player);

/***************** grid_send_state_spectator *****************/
/* Send the grid state to the spectator.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   returns a string representation of the entire grid.
 * Notes:
 *   the caller is responsible for managing the memory of the returned string.
 *   returns an empty string if no spectator is present.
 */
char* grid_send_state_spectator(grid_t* grid);

/***************** grid_game_over *****************/
/* Handle the end of the game, sending final messages and cleaning up.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   sends a game over message to all players and deletes the grid.
 * Notes:
 *   assumes all players and the grid are properly deleted afterward.
 */
void grid_game_over(grid_t* grid);

/***************** grid_getnrows *****************/
/* Get the number of rows in the grid.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   returns the number of rows in the grid.
 * Notes:
 *   useful for functions needing grid dimensions.
 */
int grid_getnrows(grid_t* grid);

/***************** grid_getncols *****************/
/* Get the number of columns in the grid.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   returns the number of columns in the grid.
 * Notes:
 *   useful for functions needing grid dimensions.
 */
int grid_getncols(grid_t* grid);

/***************** grid_getcells *****************/
/* Get the grid's cell matrix.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   returns a pointer to the grid's cell matrix.
 * Notes:
 *   the caller must not modify or free the returned matrix.
 */
char** grid_getcells(grid_t* grid);

/***************** grid_getnuggets *****************/
/* Get the grid's nuggets matrix.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   returns a pointer to the grid's nuggets matrix.
 * Notes:
 *   the caller must not modify or free the returned matrix.
 */
int** grid_getnuggets(grid_t* grid);

/***************** grid_getnuggetcount *****************/
/* Get the total count of nuggets in the grid.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   returns the total count of nuggets.
 * Notes:
 *   reflects the current state of nuggets in the grid.
 */
int grid_getnuggetcount(grid_t* grid);

/***************** grid_getplayercount *****************/
/* Get the current number of players in the grid.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   returns the current player count.
 * Notes:
 *   reflects the current number of active players.
 */
int grid_getplayercount(grid_t* grid);

/***************** grid_getplayers *****************/
/* Get the array of players in the grid.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   returns a pointer to the array of player objects.
  * Notes:
 *   the caller must not modify or free the returned array.
 */
player_t** grid_getplayers(grid_t* grid);

/***************** grid_setnuggets *****************/
/* Set the number of nuggets at a specific location in the grid.
 *
 * Caller provides:
 *   a valid grid object, coordinates (i, j), and the number of nuggets (n) to set at that location.
 * We guarantee:
 *   updates the nugget count at the specified location in the grid.
 * Notes:
 *   can be used to add or remove nuggets from a location.
 */
void grid_setnuggets(grid_t* grid, int i, int j, int n);

/***************** grid_setnuggetcount *****************/
/* Set the total nugget count in the grid.
 *
 * Caller provides:
 *   a valid grid object and the total nugget count to set.
 * We guarantee:
 *   updates the grid's total nugget count.
 * Notes:
 *   should reflect the accurate count of all nuggets in the grid.
 */
void grid_setnuggetcount(grid_t* grid, int count);

/***************** grid_getspectator *****************/
/* Get the current spectator of the grid.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   returns the spectator object if present.
 * Notes:
 *   returns NULL if no spectator is currently associated with the grid.
 */
spectator_t* grid_getspectator(grid_t* grid);

/***************** grid_setspectator *****************/
/* Set the spectator for the grid.
 *
 * Caller provides:
 *   a valid grid object and a spectator object to set as the grid's spectator.
 * We guarantee:
 *   updates the grid's spectator to the provided spectator object.
 * Notes:
 *   replaces any existing spectator with the new one.
 */
void grid_setspectator(grid_t* grid, spectator_t* spectator);

/***************** grid_getspectatorCount *****************/
/* Get the count of spectators in the grid.
 *
 * Caller provides:
 *   a valid grid object.
 * We guarantee:
 *   returns the count of spectators, which will be 0 or 1.
 * Notes:
 *   reflects the current spectator status of the grid.
 */
int grid_getspectatorCount(grid_t* grid);

/***************** grid_setspectatorCount *****************/
/* Set the spectator count for the grid.
 *
 * Caller provides:
 *   a valid grid object and the spectator count to set.
 * We guarantee:
 *   updates the grid's spectator count.
 * Notes:
 *   typically used to add or remove a spectator from the grid's count.
 */
void grid_setspectatorCount(grid_t* grid, int count);


#endif //__GRID_H
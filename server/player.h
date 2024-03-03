#ifndef PLAYER_H
#define PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "grid.h"
#include "mem.h"
#include "message.h"
/**************** global types ****************/
typedef struct player player_t;

/**************** functions ****************/

/***************** player_new *****************/
/* Create a new player object with initial position and visibility setup.
 *
 * Caller provides:
 *   valid connection information, real name, initial coordinates (x, y), 
 *   and grid dimensions (nrows, ncols).
 * We guarantee:
 *   a new player object is returned if memory allocation is successful.
 *   returns NULL if any memory allocation fails.
 * Notes:
 *   the caller is responsible for managing the memory of the addr_t* passed in.
 *   the real_name is copied, and the copy is managed internally.
 *   visibility matrix is initialized based on grid dimensions.
 *   the caller must call player_delete to free the player object's memory.
 */
player_t* player_new(const addr_t connection_info, char* real_name, int x, int y, int nrows, int ncols);

/***************** player_update_visibility *****************/
/* Update the visibility matrix for a player based on their current position.
 *
 * Caller provides:
 *   valid player object and grid object.
 * We guarantee:
 *   player's visibility matrix is updated to reflect current visibility.
 * Notes:
 *   visibility calculation depends on player position and grid layout.
 *   assumes grid and player objects are valid and properly initialized.
 */
void player_update_visibility(player_t* player, grid_t* grid);

/***************** player_moveto *****************/
/* Move player to specified coordinates.
 *
 * Caller provides:
 *   valid player object and new coordinates (x, y).
 * We guarantee:
 *   player's position is updated to the new coordinates.
 * Notes:
 *   does not check if the move is valid within the game logic.
 *   assumes coordinates are within grid bounds.
 */
void player_moveto(player_t* player, int x, int y);

/***************** player_delete *****************/
/* Delete a player object and free associated memory.
 *
 * Caller provides:
 *   valid player object and grid object.
 * We guarantee:
 *   player's associated memory is freed, including visibility matrix.
 * Notes:
 *   does not free the memory of connection_info inside the player.
 *   grid object is used to determine dimensions for freeing visibility matrix.
 */
void player_delete(player_t* player, grid_t* grid);

/***************** player_collect_gold *****************/
/* Collect gold at specified coordinates and update the player's purse.
 *
 * Caller provides:
 *   valid player object, grid object, and coordinates of the gold (gold_x, gold_y).
 * We guarantee:
 *   player's purse is updated with gold amount if gold exists at the location.
 *   gold is removed from the grid.
 * Notes:
 *   notifies all active players of the gold collection.
 *   assumes gold_x and gold_y are within grid bounds.
 */
void player_collect_gold(player_t* player, grid_t* grid, int gold_x, int gold_y);

/***************** player_move *****************/
/* Attempt to move player by a delta x and y.
 *
 * Caller provides:
 *   valid player object, grid object, and delta movements (dx, dy).
 * We guarantee:
 *   returns true and moves player if move is valid.
 *   returns false if move is invalid (e.g., blocked by wall).
 * Notes:
 *   checks for collisions with other players and grid boundaries.
 *   automatically collects gold if moved to a gold location.
 *   updates visibility for all players after move.
 */
bool player_move(player_t* player, grid_t* grid, int dx, int dy);

/***************** player_quit *****************/
/* Mark player as inactive.
 *
 * Caller provides:
 *   valid player object.
 * We guarantee:
 *   player is marked as inactive.
 * Notes:
 *   does not delete the player object; memory cleanup must be done separately.
 */
void player_quit(player_t* player);


/***************** player_get_name *****************/
/* Get the player's real name.
 *
 * Caller provides:
 *   valid player object.
 * We guarantee:
 *   returns the real name of the player.
 * Notes:
 *   name returned is internal to the player object; caller must not free it.
 */
char* player_get_name(player_t* player);

/***************** player_get_addr *****************/
/* Get the player's address.
 *
 * Caller provides:
 *   valid player object.
 * We guarantee:
 *   returns the address of the player.
 * Notes:
 *   address returned is internal to the player object; caller must not free it.
 */
addr_t* player_get_addr(player_t* player);

/***************** player_get_x *****************/
/* Get the player's x coordinate.
 *
 * Caller provides:
 *   valid player object.
 * We guarantee:
 *   returns the x coordinate of the player.
 * Notes:
 *   assumes player's position is properly maintained.
 */
int player_get_x(player_t* player);

/***************** player_get_y *****************/
/* Get the player's y coordinate.
 *
 * Caller provides:
 *   valid player object.
 * We guarantee:
 *   returns the y coordinate of the player.
 * Notes:
 *   assumes player's position is properly maintained.
 */
int player_get_y(player_t* player);

/***************** player_get_purse *****************/
/* Get the amount of gold the player has collected.
 *
 * Caller provides:
 *   valid player object.
 * We guarantee:
 *   returns the current purse amount of the player.
 * Notes:
 *   purse amount reflects total gold collected by the player.
 */
int player_get_purse(player_t* player);

/***************** player_get_visibility *****************/
/* Get the player's visibility matrix.
 *
 * Caller provides:
 *   valid player object.
 * We guarantee:
 *   returns the visibility matrix of the player.
 * Notes:
 *   matrix structure and content are determined by player position
 */
int** player_get_visibility(player_t* player);


/***************** player_set_visibility *****************/
/* Set visibility for a specific cell in the player's visibility map.
 *
 * Caller provides:
 *   valid player object, cell coordinates (x, y), and visibility value (val).
 * We guarantee:
 *   updates the visibility value for the specified cell in the player's visibility map.
 * Notes:
 *   ensures that the visibility map accurately reflects changes based on player actions or grid updates.
 */
void player_set_visibility(player_t* player, int x, int y, int val);


/***************** player_get_isactive *****************/
/* Check if the player is currently active.
 *
 * Caller provides:
 *   valid player object.
 * We guarantee:
 *   returns true if the player is active, false otherwise.
 * Notes:
 *   allows for checking the player's participation status in the game.
 */
bool player_get_isactive(player_t* player);

#endif //__PLAYER_H

/*
 * player.c - 'player' module
 *
 * see player.h for more information.
 *
 * ctrl-zzz, Winter 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "player.h"
#include "spectator.h"
#include "grid.h"
#include "mem.h"
#include "message.h"

static void player_update_purse(player_t *player, int d_gold);

int MaxNameLength = 50;

static int min(int a, int b);
static int max(int a, int b);

typedef struct player
{
	char *real_name;
	addr_t *connection_info;
	int *purse;
	int *x;
	int *y;
	int **visibility;
	bool *isactive;
} player_t;

player_t *player_new(const addr_t connection_info, char *real_name, int x, int y, int nrows, int ncols)
{
	player_t *player = (player_t *)mem_assert(malloc(sizeof(player_t)), "Error allocating memory for player\n");
	char *copied_name = (char *)mem_assert(malloc(sizeof(char) * (MaxNameLength + 1)), "Error allocating memory for copied_name\n"); // truncate is handled by message processing
	strcpy(copied_name, real_name);																									 // grid is allowed to free "REAL NAME" once sent
	player->real_name = copied_name;
	player->visibility = (int **)mem_assert(calloc(nrows, sizeof(int *)), "Error allocating visibility array\n");
	for (int i = 0; i < nrows; i++)
	{
		player->visibility[i] = (int *)mem_assert(calloc(ncols, sizeof(int)), "Error allocating row in visibility array\n");
	}
	player->connection_info = (addr_t *)mem_assert(malloc(sizeof(addr_t)), "Error allocating space for x");
	*(player->connection_info) = connection_info;
	player->x = (int *)mem_assert(malloc(sizeof(int)), "Error allocating space for x");
	player->y = (int *)mem_assert(malloc(sizeof(int)), "Error allocating space for y");
	player->isactive = (bool *)mem_assert(malloc(sizeof(bool)), "Error allocating space for isactive");
	player->purse = (int *)mem_assert(malloc(sizeof(int)), "Error allocating space for purse");
	*(player->x) = x;
	*(player->y) = y;
	*(player->purse) = 0;
	*(player->isactive) = true;
	return player;
}

void player_update_visibility(player_t *player, grid_t *grid)
{
	int x = *(player->x);
	int y = *(player->y);
	char **map = grid_getcells(grid);
	double cx;
	double cy;
	int cx_floor;
	int cx_ceil;
	int cy_floor;
	int cy_ceil;
	for (int i = 0; i < grid_getnrows(grid); i++)
	{
		for (int j = 0; j < grid_getncols(grid); j++)
		{
			if (player->visibility[i][j] == 1)
			{
				player_set_visibility(player, i, j, 2); // set previously visible squares from "active" to "seen"
			}
		}
	}
	if (map[x][y] != '.')
	{
		if (map[x][y] == '#')
		{
			int count = 0;
			for (int dx = -1; dx <= 1; dx = dx + 2)
			{
				if (0 <= x + dx && x + dx < grid_getnrows(grid))
				{
					if (map[x + dx][y] == '#')
					{
						count++;
					}
				}
			}
			for (int dy = -1; dy <= 1; dy = dy + 2)
			{
				if (0 <= y + dy && y + dy < grid_getncols(grid))
				{
					if (map[x][y + dy] == '#')
					{
						count++;
					}
				}
			}
			if (count == 1)
			{
				for (int i = 0; i < grid_getnrows(grid); i++)
				{
					for (int j = 0; j < grid_getncols(grid); j++)
					{
						bool visible = true;
						if (map[i][j] == '#')
						{
							visible = (abs(x - i) + abs(y - j) <= 1); // only adjacent hashes are shown
						}
						else
						{
							for (int dx = min(x, i) + 1; dx < max(x, i); dx++)
							{
								cy = y + (y - j) / (x - i) * (dx - x);
								cy_floor = floor(cy);
								cy_ceil = ceil(cy);
								if (map[dx][cy_floor] != '.' || map[dx][cy_ceil] != '.')
								{
									visible = false;
									break;
								}
							}
							for (int dy = min(y, j) + 1; dy < max(y, j); dy++)
							{
								cx = x + (x - i) / (y - j) * (dy - y);
								cx_floor = floor(cx);
								cx_ceil = ceil(cx);
								if (map[cx_floor][dy] != '.' || map[cx_ceil][dy] != '.')
								{
									visible = false;
									break;
								}
							}
						}
						if (visible)
						{
							player_set_visibility(player, i, j, 1);
						}
					}
				}
			}
			else
			{
				player_set_visibility(player, x, y, 1);
				for (int dx = -1; dx <= 1; dx = dx + 2)
				{
					if (0 <= x + dx && x + dx < grid_getnrows(grid))
					{
						if (map[x + dx][y] == '#')
						{
							player_set_visibility(player, x + dx, y, 1);
						}
					}
				}
				for (int dy = -1; dy <= 1; dy = dy + 2)
				{
					if (0 <= y + dy && y + dy < grid_getncols(grid))
					{
						if (map[x][y + dy] == '#')
						{
							player_set_visibility(player, x, y + dy, 1);
						}
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < grid_getnrows(grid); i++)
		{
			for (int j = 0; j < grid_getncols(grid); j++)
			{
				bool visible = true;
				for (int dx = min(x, i) + 1; dx < max(x, i); dx++)
				{
					cy = y + ((float)(y - j)) / ((float)(x - i)) * (dx - x);
					cy_floor = floor(cy);
					cy_ceil = ceil(cy);
					if (map[dx][cy_floor] != '.' && map[dx][cy_ceil] != '.')
					{
						visible = false;
						break;
					}
				}
				for (int dy = min(y, j) + 1; dy < max(y, j); dy++)
				{
					cx = x + ((float)(x - i)) / ((float)(y - j)) * (dy - y);
					cx_floor = floor(cx);
					cx_ceil = ceil(cx);
					if (map[cx_floor][dy] != '.' && map[cx_ceil][dy] != '.')
					{
						visible = false;
						break;
					}
				}
				if (visible)
				{
					player_set_visibility(player, i, j, 1);
				}
			}
		}
	}
}

void player_moveto(player_t *player, int x, int y)
{
	*(player->x) = x;
	*(player->y) = y;
}

void player_delete(player_t *player, grid_t *grid)
{
	free(player->real_name);
	free(player->x);
	free(player->y);
	free(player->purse);
	free(player->isactive);
	free(player->connection_info);
	int nr = grid_getnrows(grid);
	for (int i = 0; i < nr; i++)
	{
		free(player->visibility[i]);
	}
	free(player->visibility);
	free(player);
}

void player_collect_gold(player_t *player, grid_t *grid, int gold_x, int gold_y)
{
	int **nuggets = grid_getnuggets(grid);
	int gold_obtained = nuggets[gold_x][gold_y];
	if (gold_obtained != 0)
	{
		player_update_purse(player, gold_obtained);
		player_t **players = grid_getplayers(grid);
		grid_setnuggets(grid, gold_x, gold_y, 0);
		grid_setnuggetcount(grid, grid_getnuggetcount(grid) - 1);
		int num_players = grid_getplayercount(grid);
		for (int i = 0; i < num_players; i++)
		{
			if (*(players[i]->isactive))
			{
				char *message = (char *)mem_assert(malloc(sizeof(char) * 50), "Error allocating memory for gold message string\n"); // GOLD N P R
				sprintf(message, "GOLD %d %d %d", (players[i] == player ? gold_obtained : 0), *player->purse, grid_getnuggetcount(grid));
				if (player_get_addr((players[i])) != NULL)
				{
					message_send(*player_get_addr(players[i]), message);
				}
				else
				{
					printf("Message: %s\n", message);
				}
				free(message);
			}
		}
		if (grid_getspectatorCount(grid) == 1)
		{
			char *message = (char *)mem_assert(malloc(sizeof(char) * 50), "Error allocating memory for gold message string\n"); // GOLD N P R
			sprintf(message, "GOLD 0 0 %d", grid_getnuggetcount(grid));
			message_send(*spectator_get_addr(grid_getspectator(grid)), message);
			free(message);
		}
	}
}

bool player_move(player_t *player, grid_t *grid, int dx, int dy)
{
	int x = *player->x;
	int y = *player->y;
	if (0 <= x + dx && x + dx < grid_getnrows(grid) && 0 <= y + dy && y + dy < grid_getncols(grid))
	{
		if (grid_getcells(grid)[x + dx][y + dy] == '.' || grid_getcells(grid)[x + dx][y + dy] == '#')
		{
			player_t **players = grid_getplayers(grid);
			for (int i = 0; i < grid_getplayercount(grid); i++)
			{
				if (players[i]->isactive)
				{
					if (player_get_x(players[i]) == x + dx && player_get_y(players[i]) == y + dy)
					{
						player_moveto(players[i], x, y);
						break;
					}
				}
			}
			player_moveto(player, x + dx, y + dy);
			player_collect_gold(player, grid, x + dx, y + dy);
			for (int i = 0; i < grid_getplayercount(grid); i++)
			{
				if (players[i]->isactive)
				{
					player_update_visibility(players[i], grid);
				}
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void player_quit(player_t *player, grid_t *grid)
{
	//drop player nuggets on last location
	int x = player_get_x(player);
	int y = player_get_y(player);
	grid_setnuggets(grid, x, y, player_get_purse(player));

	//update nugget count of grid
	grid_setnuggetcount(grid, grid_getnuggetcount(grid) + 1);

	//update nugget count of player
	player_update_purse(player, -player_get_purse(player));

	//send updated messages to players
	player_t** players = grid_getplayers(grid);
	for (int i = 0; i < grid_getplayercount(grid); i++) {
		addr_t currentAddress = *player_get_addr(players[i]);
		//construct GOLD message with recipient's purse and updated total nuggets in game
		char* message = mem_assert(malloc(128), "Error allocating space for message");
		sprintf(message, "GOLD 0 %d %d", player_get_purse(players[i]), grid_getnuggetcount(grid) + player_get_purse(player));
		message_send(currentAddress, message);
		free(message);
		
		//send updated DISPLAY message
		message = grid_send_state(grid, players[i]);
		message_send(currentAddress, message);
		free(message);
	}
	//send updated messages to spectator
	addr_t currentAddress = *spectator_get_addr(grid_getspectator(grid));
	//construct GOLD message with updated total nuggets in game
	char* message = mem_assert(malloc(128), "Error allocating space for message");
	sprintf(message, "GOLD 0 0 %d", grid_getnuggetcount(grid) + player_get_purse(player));
	message_send(currentAddress, message);
	free(message);
	
	//send updated DISPLAY message
	message = grid_send_state_spectator(grid);
	message_send(currentAddress, message);
	free(message);

	//quit player
	*player->isactive = false;
}

char *player_get_name(player_t *player)
{
	return player->real_name;
}

addr_t *player_get_addr(player_t *player)
{
	return player->connection_info;
}

int player_get_x(player_t *player)
{
	return *(player->x);
}

int player_get_y(player_t *player)
{
	return *(player->y);
}

int player_get_purse(player_t *player)
{
	return *(player->purse);
}

int **player_get_visibility(player_t *player)
{
	return player->visibility;
}

void player_set_visibility(player_t *player, int x, int y, int val)
{
	player->visibility[x][y] = val;
}

bool player_get_isactive(player_t *player)
{
	return *player->isactive;
}

static int min(int a, int b)
{
	return a > b ? b : a;
}

static int max(int a, int b)
{
	return a > b ? a : b;
}

static void player_update_purse(player_t *player, int d_gold)
{
	*player->purse = *player->purse + d_gold;
}
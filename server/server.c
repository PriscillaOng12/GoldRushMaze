/*
 * server.c - 'server' file
 *
 * Runs server for Nuggets game
 * Most of the processing is handled by handleMessage.
 * ctrl-zzz, Winter 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "log.h"
#include "mem.h"
#include "message.h"
#include "file.h"
#include "grid.h"
#include "player.h"
#include "spectator.h"

static bool parseArgs(const int argc, const char **argv);
static bool handleMessage(void *arg, const addr_t from, const char *message);
static bool updateall(grid_t *grid);
// grid_t* gameGrid;

int main(const int argc, const char **argv)
{
	// validate args
	if (!parseArgs(argc, argv))
	{
		return 1;
	}

	// initialize message module
	int serverPort;
	FILE *logFP = fopen("server.log", "w");
	if ((serverPort = message_init(logFP)) == 0)
	{
		return 1;
	}
	fprintf(stdout, "Server port is: %d", serverPort);
	FILE *fp = fopen(argv[1], "r");
	grid_t *gameGrid = grid_load(fp);
	fclose(fp);
	grid_init_gold(gameGrid);
	message_loop(gameGrid, 0, NULL, NULL, handleMessage);
	message_done();
	fclose(logFP);
	return 0;
}

static bool parseArgs(const int argc, const char **argv)
{
	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Usage : %s map.txt [seed], your command must have either 1 or two arguments\n", argv[0]);
		return false;
	}

	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		fprintf(stderr, "map file could not be opened");
		return false;
	}
	fclose(fp);

	// assumes seed will be integer
	if (argc == 3)
	{
		// convert to int type
		int seed = atoi(argv[2]);
		if (seed < 0)
		{
			fprintf(stderr, "seed must be positive integer");
			return false;
		}
		srand(seed);
	}
	else
	{
		srand(getpid());
	}

	return true;
}

static bool handleMessage(void *arg, const addr_t from, const char *message)
{
	// set max name length to 50 chars
	int MaxNameLength = 50;
	int MaxPlayers = 26;
	grid_t *gameGrid = (grid_t *)arg;
	player_t **playerList = grid_getplayers(gameGrid);
	int playerCount = grid_getplayercount(gameGrid);
	// get first word from message
	char *firstWord;
	int firstSpace = 0;
	while (!isspace(message[firstSpace]))
	{
		firstSpace++;
	}
	// amount of bytes needed for first word of message + 1 for null terminating
	firstWord = mem_assert(malloc(firstSpace + 1), "Failed to allocate memory for firstWord.");
	strncpy(firstWord, message, firstSpace);
	firstWord[firstSpace] = '\0';

	// if first word is PLAY
	if (strcmp(firstWord, "PLAY") == 0)
	{
		if (playerCount == MaxPlayers)
		{
			message_send(from, "QUIT Game is full: no more players can join.");
		}
		char *real_name = mem_assert(malloc(128), "Error allocating space for real name");
		strcpy(real_name, message + 5);
		if (strcmp(real_name, "") == 0)
		{
			message_send(from, "QUIT Sorry - you must provide player's name.");
			return false;
		}
		// truncate to MaxNameLength and replace characters that are both isgraph() and isblank()
		char *real_name_truncated = mem_assert(malloc((MaxNameLength + 1) * sizeof(char)), "Failed to allocate memory for real_name_truncated.");
		strncpy(real_name_truncated, real_name, MaxNameLength);
		real_name_truncated[50] = '\0';
		for (int i = 0; i < 50; i++)
		{
			if (isgraph(real_name_truncated[i] && isblank(real_name_truncated[i])))
			{
				real_name_truncated[i] = '_';
			}
		}
		grid_spawn_player(gameGrid, from, real_name_truncated);
		free(real_name);
		free(real_name_truncated);
		int playerCount = grid_getplayercount(gameGrid);
		char *messageToSend = mem_assert(malloc(128), "Failed to allocate memory for messageToSend.");
		char playerCharacter = (char)(64 + playerCount);
		sprintf(messageToSend, "OK %c", playerCharacter);
		message_send(from, messageToSend);
		sprintf(messageToSend, "GRID %d %d", grid_getnrows(gameGrid), grid_getncols(gameGrid));
		message_send(from, messageToSend);
		sprintf(messageToSend, "GOLD 0 0 %d", grid_getnuggetcount(gameGrid));
		message_send(from, messageToSend);
		player_move(playerList[grid_getplayercount(gameGrid) - 1], gameGrid, 0, 0); // make sure they get gold if they are standing there
		free(messageToSend);
		free(firstWord);
		return updateall(gameGrid);
	}
	else if (strcmp(firstWord, "KEY") == 0)
	{
		// find matching player in list of players to find out which player to move
		player_t *matchingPlayer = NULL;
		for (int i = 0; i < playerCount; i++)
		{
			const addr_t playerAddr = *player_get_addr(playerList[i]);
			if (message_eqAddr(from, playerAddr))
			{
				matchingPlayer = playerList[i];
				break;
			}
		}
		char *keyStroke = mem_assert(malloc(128), "Failed to allocate memory for keyStroke.");
		strcpy(keyStroke, message + 4);
		// special case: check spectator
		if (strcmp(keyStroke, "Q") == 0 && matchingPlayer == NULL) {
			if (grid_getspectatorCount(gameGrid) == 1) 
			{
				spectator_t* spectator = grid_getspectator(gameGrid);
				if (message_eqAddr(*spectator_get_addr(spectator), from)) {
					spectator_quit(spectator, gameGrid);
					message_send(from, "QUIT Thanks for playing!");
				}
			}
			free(keyStroke);
			free(firstWord);
			return updateall(gameGrid);
		}
		if (matchingPlayer == NULL || player_get_isactive(matchingPlayer) == false) {
			free(keyStroke);
			free(firstWord);
			return updateall(gameGrid);
		}

		if (strcmp(keyStroke, "h") == 0)
		{ // CAPITAL CHARACTER FIX, CHECK IF SPECTATOR
			player_move(matchingPlayer, gameGrid, -1, 0);
		}
		else if (strcmp(keyStroke, "l") == 0)
		{
			player_move(matchingPlayer, gameGrid, 1, 0);
		}
		else if (strcmp(keyStroke, "j") == 0)
		{
			player_move(matchingPlayer, gameGrid, 0, -1);
		}
		else if (strcmp(keyStroke, "k") == 0)
		{
			player_move(matchingPlayer, gameGrid, 0, 1);
		}
		else if (strcmp(keyStroke, "y") == 0)
		{
			player_move(matchingPlayer, gameGrid, -1, 1);
		}
		else if (strcmp(keyStroke, "u") == 0)
		{
			player_move(matchingPlayer, gameGrid, 1, 1);
		}
		else if (strcmp(keyStroke, "b") == 0)
		{
			player_move(matchingPlayer, gameGrid, -1, -1);
		}
		else if (strcmp(keyStroke, "n") == 0)
		{
			player_move(matchingPlayer, gameGrid, 1, -1);
		}
		else if (strcmp(keyStroke, "Q") == 0)
		{
			player_quit(matchingPlayer, gameGrid);
			message_send(from, "QUIT Thanks for playing!");
		}
		else if (strcmp(keyStroke, "H") == 0)
		{
			while (player_move(matchingPlayer, gameGrid, -1, 0)) {} // fancy way of doing till returns false!
		}
		else if (strcmp(keyStroke, "L") == 0)
		{
			while (player_move(matchingPlayer, gameGrid, 1, 0)) {}
		}
		else if (strcmp(keyStroke, "J") == 0)
		{
			while (player_move(matchingPlayer, gameGrid, 0, -1)) {}
		}
		else if (strcmp(keyStroke, "K") == 0)
		{
			while (player_move(matchingPlayer, gameGrid, 0, 1)) {}
		}
		else if (strcmp(keyStroke, "Y") == 0)
		{
			while (player_move(matchingPlayer, gameGrid, -1, 1)) {}
		}
		else if (strcmp(keyStroke, "U") == 0)
		{
			while (player_move(matchingPlayer, gameGrid, 1, 1)) {}
		}
		else if (strcmp(keyStroke, "B") == 0)
		{
			while (player_move(matchingPlayer, gameGrid, -1, -1)) {}
		}
		else if (strcmp(keyStroke, "N") == 0)
		{
			while (player_move(matchingPlayer, gameGrid, 1, -1)) {} 
		}
		else
		{
			message_send(from, "ERROR unknown keystroke");
		}
		free(keyStroke);
		free(firstWord);
		return updateall(gameGrid);
	}
	else if (strcmp(firstWord, "SPECTATE") == 0)
	{
		grid_spawn_spectator(gameGrid, spectator_new(from));
		char* messageToSend = mem_assert(malloc(128), "Failed to allocate memory for messageToSend (re-allocation).");
		sprintf(messageToSend, "GRID %d %d", grid_getnrows(gameGrid), grid_getncols(gameGrid));
		message_send(from, messageToSend);
		sprintf(messageToSend, "GOLD 0 0 %d", grid_getnuggetcount(gameGrid));
		message_send(from, messageToSend);
		free(messageToSend);
		free(firstWord);
		return updateall(gameGrid);
	}
	else
	{
		message_send(from, "invalid message");
		free(firstWord);
		return false; // SHOULD KEEP GOING?
	}
}

static bool updateall(grid_t *grid)
{
	int playerCount = grid_getplayercount(grid);
	player_t **playerList = grid_getplayers(grid);
	for (int i = 0; i < playerCount; i++)
	{
		if (player_get_isactive(playerList[i]))
		{
			char *messageToSend = grid_send_state(grid, playerList[i]);
			message_send(*player_get_addr(playerList[i]), messageToSend);
			free(messageToSend);
		}
	}
	if (grid_getspectatorCount(grid) == 1)
	{
		char *messageToSend = grid_send_state_spectator(grid);
		message_send(*spectator_get_addr(grid_getspectator(grid)), messageToSend);
		free(messageToSend);
	}
	if (grid_getnuggetcount(grid) == 0)
	{
		grid_game_over(grid);
		return true;
	}
	return false;
}

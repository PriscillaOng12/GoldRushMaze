# Gold Rush Maze
## Overview

**Gold Rush Maze** is a real-time, multiplayer treasure hunt game where up to 26 players and 1 spectator compete to collect hidden treasures on a dynamic grid-based map. Built in C, the game features a client-server architecture using the UDP/IP protocol for fast and efficient real-time communication, and an intuitive terminal-based user interface using the ncurses library.

Players navigate through a series of rooms and corridors, seeking gold while avoiding other players. The game supports a spectator mode, where a user can watch all player movements and gold collections as the game progresses.

## Rules
The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are randomly distributed in *piles* within the rooms.
Up to 26 players, and one spectator, may play a given game.
Each player is randomly dropped into a room when joining the game.
Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected, the game ends and a summary is printed.

### Stealing Mechanic

Players can **steal gold** from each other by bumping into another player. When a player moves into the position of a stationary player:

- The **moving player** receives all the gold from the stationary player's purse.
- The **stationary player's** purse is reduced to **0**.

### Invincibility Rules:

- After being stolen from, the stationary player gains **invincibility** (tracked by a boolean in the player struct) and **cannot be stolen from again** until they move.
- The player who stole the gold also gains temporary **invincibility**, preventing them from being stolen from until they move.
- All players start with **invincibility** upon spawning to prevent unfair play at the beginning of the game.

### Quitting Mechanic

When a player quits the game:

- They drop all their gold in a **pile** at their last location before quitting.
- All remaining players and the spectator receive an update with the new **DISPLAY**, showing the dropped gold pile.
- The total gold in the game is updated by adding the quitting player's purse to the total gold remaining in the game.
- The standing of gold collected by each player is updated, and the information is shared with everyone still in the game.

## Gameplay Instructions
**Grid display:**
The remaining *NR* lines present the grid using *map characters:*

 * ` ` solid rock - interstitial space outside rooms
 * `-` a horizontal boundary
 * `|` a vertical boundary
 * `+` a corner boundary
 * `.` an empty room spot
 * `#` an empty passage spot

or *occupant characters:*

 * `@` the player
 * `A`-`Z` another player
 * `*` a pile of gold

**Spectator keystrokes:**
All keystrokes are sent immediately to the server.
The only valid keystroke for the spectator is

 * `Q` quit the game.

**Player keystrokes:**
All keystrokes are sent immediately to the server.
The valid keystrokes for the player are

 * `Q` quit the game.
 * `h` move left, if possible
 * `l` move right, if possible
 * `j` move down, if possible
 * `k` move up	, if possible
 * `y` move diagonally up and left, if possible
 * `u` move diagonally up and right, if possible
 * `b` move diagonally down and left, if possible
 * `n` move diagonally down and right, if possible

* where *possible* means the adjacent gridpoint in the given direction is an empty spot, a pile of gold, or another player.
* for each move key, the corresponding Capitalized character will move automatically and repeatedly in that direction, until it is no longer possible.

The eight movement keys are shown relative to the player (@) below.

|     |     |     |
|:---:|:---:|:---:|
|  y  |  k  |  u  |
|  h  |  @  |  l  |
|  b  |  j  |  n  |


## Features

- **Real-Time Multiplayer**: Supports up to 26 players and 1 spectator simultaneously.
- **Responsive Terminal-Based UI**: Built using ncurses, the game provides smooth player interaction with real-time visibility and input handling.
- **Optimized Client-Server Communication**: Utilizes a custom messaging protocol over UDP/IP, minimizing latency and handling packet loss to ensure smooth gameplay.
- **Dynamic Grid Updates**: The grid-based map is updated in real-time as players move and collect gold, with efficient message parsing and rendering to ensure low-latency UI updates.
- **Spectator Mode**: Watch the game unfold live with a full view of the grid, player movements, and treasure collection events.
- **Scalable and Maintainable**: Modular design for ease of future expansions, with structured client-server communication and error handling mechanisms.

## Installation

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/PriscillaOng12/GoldRushMaze.git
   cd GoldRushMaze
   ```
2. Compile the Game: Make sure you have ncurses and gcc installed. Then, compile both the client and server:
```bash
make all
```
3. Run the Server: The server needs a map file to run. Use the maps/ folder or create your own.
```bash
./server maps/default_map.txt [optional_seed]
```
This will display the port number on which the server is running.

4. Run the Client: Connect to the game server as a player or a spectator.

To join as a player:
```bash
./client <hostname> <port> <playername>
```
To join as a spectator:
```bash
./client <hostname> <port>
```


## Materials provided

See the [support library](support/README.md) for some useful modules.

See the [maps](maps/README.md) for some draft maps. Our custom map is at `maps/ctrl-zzz.txt`.






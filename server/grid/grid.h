

typedef struct {
    char** cells; // initialize 2D array wiht max number of columns already being defined 
    int** nuggets;
    int* rows;
    int* columns;
} grid_t;



grid_t* grid_load(FILE* map_file);
int grid_init_gold(grid_t* grid);
// void grid_spawn_player(grid_t* grid, player_t* player);
// void grid_spawn_spectator(grid_t* grid, spectator_t* spectator);
// void grid_send_state(player_t* player);
// void grid_send_state_spectator(spectator_t* spectator);
// void grid_game_over(grid_t* grid, player_t* players[], int num_players);
void grid_delete(grid_t* grid);
void print_nugget_array(grid_t* grid) ;
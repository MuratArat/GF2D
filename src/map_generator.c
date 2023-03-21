#include <stdlib.h>
#include <time.h>
#include "map_generator.h"

// You can tweak the percentages to influence map generation
#define WALL_PERCENTAGE 20
#define CHEST_PERCENTAGE 5
#define ENEMY_PERCENTAGE 10
#define PERSON_PERCENTAGE 5

int random_tile()
{
    int random_value = rand() % 100;

    if (random_value < WALL_PERCENTAGE) return 1;
    if (random_value < WALL_PERCENTAGE + CHEST_PERCENTAGE) return 2;
    if (random_value < WALL_PERCENTAGE + CHEST_PERCENTAGE + ENEMY_PERCENTAGE) return 3;
    if (random_value < WALL_PERCENTAGE + CHEST_PERCENTAGE + ENEMY_PERCENTAGE + PERSON_PERCENTAGE) return 4;

    return 0;
}

MapGrid *generate_map(int width, int height, int tile_width, int tile_height, const char *tiles_path)
{
    MapGrid *grid = map_grid_create(width, height, tile_width, tile_height, tiles_path);
    if (!grid) return NULL;

    srand(time(NULL));

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int tile_index = random_tile();
            grid->data[i * width + j] = tile_index;
        }
    }

    return grid;
}

void save_map(const char *map_name, MapGrid *grid)
{
    // Implement saving the map data to a file
}

MapGrid *load_map(const char *map_name, int tile_width, int tile_height, const char *tiles_path)
{
    // Load map data from a file and create a MapGrid object
    
    // For now, we will create a MapGrid object filled with zeros
    int width, height;
    
    // Set your desired width and height values here
    width = 19;
    height = 12;
    
    MapGrid *grid = map_grid_create(width, height, tile_width, tile_height, tiles_path);
    
    if (!grid) return NULL;
    
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            grid->data[i * width + j] = 0;
        }
    }
    
    return grid;
}

void regenerate_map(MapGrid *map_grid)
{
    if (!map_grid)
        return;

    int i, j;
    for (i = 0; i < map_grid->height; i++)
    {
        for (j = 0; j < map_grid->width; j++)
        {
                map_grid->data[i * map_grid->width + j] = random_tile(); 
        }
    }
}



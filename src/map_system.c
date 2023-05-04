#include "map_system.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"
#include <stdlib.h>
#include <time.h>
#include <SDL_ttf.h>
#include <SDL.h>
#include <SDL_mixer.h>

#include <stdlib.h>
#include <stdio.h>


MapGrid *map_grid_create(int width, int height, const char *tiles_path)
{
    MapGrid *grid = malloc(sizeof(MapGrid));
    if (!grid) return NULL;

    double tile_scale=1;
    grid->width = width;
    grid->height = height;
    grid->tile_width = 64*tile_scale;
    grid->tile_height = 64*tile_scale;

    if (tiles_path)
    {
        grid->tiles = gf2d_sprite_load_image(tiles_path);
        if (!grid->tiles)
        {
            free(grid);
            return NULL;
        }
    }
    else
    {
        grid->tiles = NULL;
    }


    grid->data = malloc(sizeof(int) * width * height);
    if (!grid->data)
    {
        map_grid_free(grid);
        return NULL;
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            grid->data[i * width + j] = 0;
        }
    }

    return grid;
}

void save_map(const char *map_name, MapGrid *grid)
{
    if (!grid) return;

    SJson *map_data = sj_object_new();
    SJson *tiles = sj_array_new();

    for (int i = 0; i < grid->height; i++)
    {
        for (int j = 0; j < grid->width; j++)
        {
            sj_array_append(tiles, sj_new_int(grid->data[i * grid->width + j]));
        }
    }

    sj_object_insert(map_data, "width", sj_new_int(grid->width));
    sj_object_insert(map_data, "height", sj_new_int(grid->height));
    sj_object_insert(map_data, "tiles", tiles);
    sj_object_insert(map_data, "quest_value", sj_new_int(grid->quest_value));

    // Save the map data to a JSON file
    sj_save(map_data, map_name);

    sj_free(map_data);
}

MapGrid *load_map(const char *map_name, const char *tiles_path)
{
    SJson *map_data = sj_load(map_name);
    if (!map_data)
    {
        printf("Error loading map data from file.\n");
        return NULL;
    }

    int width, height, quest_value;
    SJson *tiles;

    sj_get_integer_value(sj_object_get_value(map_data, "width"), &width);
    sj_get_integer_value(sj_object_get_value(map_data, "height"), &height);
    tiles = sj_object_get_value(map_data, "tiles");
    sj_get_integer_value(sj_object_get_value(map_data, "quest_value"), &quest_value);

    MapGrid *grid = map_grid_create(width, height, tiles_path);
    if (!grid)
    {
        sj_free(map_data);
        return NULL;
    }

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int tile_index;
            sj_get_integer_value(sj_array_get_nth(tiles, i * width + j), &tile_index);
            grid->data[i * width + j] = tile_index;
        }
    }

    sj_free(map_data);
    return grid;
}

void map_grid_free(MapGrid *grid)
{
    if (!grid) return;

    if (grid->tiles)
    {
        gf2d_sprite_free(grid->tiles);
    }

    if (grid->data)
    {
        free(grid->data);
    }

    free(grid);
}

void map_grid_draw(MapGrid *map_grid)
{
    // Load the images for each tile type
    Sprite *wall_tile = gf2d_sprite_load_image("images/wall.png");
    Sprite *empty_tile = gf2d_sprite_load_image("images/empty.png");
    Sprite *enemy_tile = gf2d_sprite_load_image("images/enemy.png");
    Sprite *treasure_tile = gf2d_sprite_load_image("images/treasure.png");
    Sprite *person_tile = gf2d_sprite_load_image("images/person.png");
    Sprite *next_floor_tile = gf2d_sprite_load_image("images/stairs.png");
    double my_scale = 1.0;
    int i, j;
    for (i = 0; i < map_grid->height; i++)
    {
        for (j = 0; j < map_grid->width; j++)
        {
            Vector2D position = vector2d(j * map_grid->tile_width, i * map_grid->tile_height);
            int tile_type = map_grid->data[i * map_grid->width + j];

            switch (tile_type)
            {
                case 0: // Empty tile
                    gf2d_sprite_draw(empty_tile, position, my_scale,my_scale, NULL, NULL, NULL, NULL, 0);
                    break;
                case 1: // Wall tile
                    gf2d_sprite_draw(wall_tile, position, my_scale,my_scale, NULL, NULL, NULL, NULL, 0);
                    break;
                case 2: // Enemy tile
                    gf2d_sprite_draw(enemy_tile, position, my_scale,my_scale, NULL, NULL, NULL, NULL, 0);
                    break;
                case 3: // Treasure tile
                    gf2d_sprite_draw(treasure_tile, position, my_scale,my_scale, NULL, NULL, NULL, NULL, 0);
                    break;
                case 4: // Person tile
                    gf2d_sprite_draw(person_tile, position, my_scale,my_scale, NULL, NULL, NULL, NULL, 0);
                    break;
                case 5: // Next floor tile
                    gf2d_sprite_draw(next_floor_tile, position, my_scale,my_scale, NULL, NULL, NULL, NULL, 0);
                    break;
                default:
                    gf2d_sprite_draw(empty_tile, position, my_scale,my_scale, NULL, NULL, NULL, NULL, 0);
                    break;
            }
        }
    }

    // Free the sprites after drawing the map
    gf2d_sprite_free(wall_tile);
    gf2d_sprite_free(empty_tile);
    gf2d_sprite_free(enemy_tile);
    gf2d_sprite_free(treasure_tile);
    gf2d_sprite_free(person_tile);
}



void populate_tile(MapGrid *map_grid, int x, int y, int start_x, int start_y)
{
    if (!map_grid)
        return;

    int random_value = rand() % 100;
    if (x == start_x && y == start_y) {
        map_grid->data[y * map_grid->width + x] = 0; // Keep the starting position empty
    } else {
    if (random_value < 3) // 3% chance for enemy
    {
        map_grid->data[y * map_grid->width + x] = 2;
    }
    else if (random_value < 4) // 1% chance for treasure
    {
        map_grid->data[y * map_grid->width + x] = 3;
    }
    else if (random_value < 6) // 2% chance for person
    {
        map_grid->data[y * map_grid->width + x] = 4;
    }
    else // 94% chance for empty
    {
        map_grid->data[y * map_grid->width + x] = 0;
    }
    }
}
void place_next_floor(MapGrid *map_grid, int x, int y)
{
    map_grid->data[y * map_grid->width + x] = 5;
}

void regenerate_map(MapGrid *map_grid, int start_x, int start_y)
{
    map_grid->monster_count = 0;
    if (!map_grid)
        return;
    slog("Regenerating map");
    slog("Start position: %d, %d", start_x, start_y);
    slog("Map size: %d, %d", map_grid->width, map_grid->height);
    slog("Map tile size: %d, %d", map_grid->tile_width, map_grid->tile_height);
    start_x = start_x / map_grid->tile_width;
    start_y = start_y / map_grid->tile_height;
    // Fill the map grid with wall tiles
    for (int i = 0; i < map_grid->height; i++)
    {
        for (int j = 0; j < map_grid->width; j++)
        {
            map_grid->data[i * map_grid->width + j] = 1; // Set to wall tile
        }
    }

    // Ensure the starting tile is empty
    map_grid->data[start_y * map_grid->width + start_x] = 0;

    // Define the number of steps for the Drunkard's Walk
    int num_steps = 2500; // Adjust this value to achieve the desired map generation

    // Perform the Drunkard's Walk
    int x = start_x;
    int y = start_y;

    int step = 0;
    while (step < num_steps)
    {
        // Set the current position to a floor tile or other tile type based on probabilities
        int previous_tile = map_grid->data[y * map_grid->width + x];
        if(step == num_steps - 1)
        {
            place_next_floor(map_grid, x, y);
        }
        else
        {
            populate_tile(map_grid, x, y, start_x, start_y);
        }
        int direction = rand() % 4; // Choose a random direction (0: up, 1: down, 2: left, 3: right)
        switch (direction)
        {
            case 0:
                y = y > 0 ? y - 1 : y;// Ensure the new position is within the map bounds
                break;
            case 1:
                y = y < map_grid->height - 1 ? y + 1 : y;// Ensure the new position is within the map bounds
                break;
            case 2:
                x = x > 0 ? x - 1 : x;// Ensure the new position is within the map bounds
                break;
            case 3:
                x = x < map_grid->width - 1 ? x + 1 : x;// Ensure the new position is within the map bounds
                break;
        }

        if (previous_tile != 0)
        {
            step++; // Only increment the step if the previous tile was not empty
        }
    }

    // Fill the perimeter with wall tiles
    for (int i = 0; i < map_grid->height; i++)
    {
        for (int j = 0; j < map_grid->width; j++)
        {
            if (i == 0 || i == map_grid->height - 1 || j == 0 || j == map_grid->width - 1)
            {
                map_grid->data[i * map_grid->width + j] = 1; // Set to wall tile
            }
        }
    }
    split_and_save_maps(map_grid);
    generate_quest(map_grid);
}

void split_and_save_maps(MapGrid *map_grid)
{
    if (!map_grid)
        return;

    int submap_width = map_grid->width / 5;
    int submap_height = map_grid->height / 5;

    for (int submap_y = 0; submap_y < 5; submap_y++)
    {
        for (int submap_x = 0; submap_x < 5; submap_x++)
        {
            // Create a new submap
            MapGrid *submap = map_grid_create(submap_width, submap_height, NULL);
            if (!submap)
                continue;

            for (int y = 0; y < submap_height; y++)
            {
                for (int x = 0; x < submap_width; x++)
                {
                    int source_x = submap_x * submap_width + x;
                    int source_y = submap_y * submap_height + y;
                    submap->data[y * submap_width + x] = map_grid->data[source_y * map_grid->width + source_x];
                }
            }

            // Save the submap
            char submap_name[16];
            snprintf(submap_name, sizeof(submap_name), "json/map%d%d.json", submap_x, submap_y);
            save_map(submap_name, submap);

            // Free the submap
            map_grid_free(submap);
        }
    }
}

int get_monster_count(MapGrid *map_grid)
{
    if (!map_grid)
        return 0;

    int monster_count = 0;
    for (int i = 0; i < map_grid->height; i++)
    {
        for (int j = 0; j < map_grid->width; j++)
        {
            if (map_grid->data[i * map_grid->width + j] == 2)
            {
                monster_count++;
            }
        }
    }
    return monster_count;
}


void set_quest_value(MapGrid *grid, int value){
    grid->quest_value = value;
    slog("quest value1: %d", grid->quest_value);
}

int get_quest_value(MapGrid *grid){
    return grid->quest_value;
}

Bool is_quest_complete(MapGrid *grid){
    if(grid->quest_value == 0){
        return true;
    }
    return false;
}

void generate_quest(MapGrid *grid) {
    int count = 0;

    // Allocate memory for the quest_text field
    grid->quest_text = (char *)malloc(128 * sizeof(char));

        count = get_monster_count(grid) / 5;
        slog("monster count2: %d", count);
        set_quest_value(grid, count);
        slog("quest value: %d", get_quest_value(grid));
        // Format the quest_text using sprintf()
        sprintf(grid->quest_text, "Kill %d enemies", get_quest_value(grid));
    
}


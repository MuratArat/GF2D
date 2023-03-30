#include "map_system.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"
#include <stdlib.h>
#include <time.h>

MapGrid *map_grid_create(int width, int height, int tile_width, int tile_height, const char *tiles_path)
{
    MapGrid *grid = malloc(sizeof(MapGrid));
    if (!grid) return NULL;

    grid->width = width;
    grid->height = height;
    grid->tile_width = 64;
    grid->tile_height = 64;

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
                    gf2d_sprite_draw(empty_tile, position, NULL, NULL, NULL, NULL, NULL, 0);
                    break;
                case 1: // Wall tile
                    gf2d_sprite_draw(wall_tile, position, NULL, NULL, NULL, NULL, NULL, 0);
                    break;
                case 2: // Enemy tile
                    gf2d_sprite_draw(enemy_tile, position, NULL, NULL, NULL, NULL, NULL, 0);
                    break;
                case 3: // Treasure tile
                    gf2d_sprite_draw(treasure_tile, position, NULL, NULL, NULL, NULL, NULL, 0);
                    break;
                case 4: // Person tile
                    gf2d_sprite_draw(person_tile, position, NULL, NULL, NULL, NULL, NULL, 0);
                    break;
                default:
                    gf2d_sprite_draw(empty_tile, position, NULL, NULL, NULL, NULL, NULL, 0);
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

int is_valid_position(MapGrid *grid, int x, int y)
{
    if (x < 0 || x >= grid->width || y < 0 || y >= grid->height) return 0;
    if (grid->data[y * grid->width + x] == 1) return 0;
    return 1;
}

void map_grid_move(MapGrid *grid, int *player_x, int *player_y, SDL_Keycode key, MapTransitionCallback callback)
{
    int new_x = *player_x, new_y = *player_y;

    if (key == SDLK_UP) new_y--;
    if (key == SDLK_DOWN) new_y++;
    if (key == SDLK_LEFT) new_x--;
    if (key == SDLK_RIGHT) new_x++;

    if (is_valid_position(grid, new_x, new_y))
    {
        *player_x = new_x;
        *player_y = new_y;
    }

    // Check if the player has reached an edge
    if (new_x == 0 || new_x == grid->width - 1 || new_y == 0 || new_y == grid->height - 1)
    {
        if (callback)
        {
            callback(grid, player_x, player_y, key);
        }
    }
}

// You can tweak the percentages to influence map generation
#define WALL_PERCENTAGE 20
#define ENEMY_PERCENTAGE 10
#define CHEST_PERCENTAGE 2
#define PERSON_PERCENTAGE 3

int random_tile()
{
    int random_value = rand() % 100;

    if (random_value < WALL_PERCENTAGE) return 1;
    if (random_value < WALL_PERCENTAGE + ENEMY_PERCENTAGE) return 2;
    //if (random_value < WALL_PERCENTAGE + CHEST_PERCENTAGE + ENEMY_PERCENTAGE) return 3;
    //if (random_value < WALL_PERCENTAGE + CHEST_PERCENTAGE + ENEMY_PERCENTAGE + PERSON_PERCENTAGE) return 4;

    return 0;
}

MapGrid *generate_map(int width, int height, int tile_width, int tile_height, const char *tiles_path)
{
    MapGrid *grid = map_grid_create(width, height, 19, 12, tiles_path);
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

    // Save the map data to a JSON file
    sj_save(map_data, map_name);

    sj_free(map_data);
}

MapGrid *load_map(const char *map_name,int tile_width, int tile_height, const char *tiles_path)
{
    SJson *map_data = sj_load(map_name);
    if (!map_data)
    {
        printf("Error loading map data from file.\n");
        return NULL;
    }

    int width, height;
    SJson *tiles;

    sj_get_integer_value(sj_object_get_value(map_data, "width"), &width);
    sj_get_integer_value(sj_object_get_value(map_data, "height"), &height);
    tiles = sj_object_get_value(map_data, "tiles");

    MapGrid *grid = map_grid_create(width, height, 19, 12, tiles_path);
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


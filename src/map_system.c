#include "map_system.h"
#include "gf2d_sprite.h"
#include "gf2d_draw.h"


#include "map_system.h"
#include <stdlib.h>


MapGrid *map_grid_create(int width, int height, int tile_width, int tile_height, const char *tiles_path)
{
    MapGrid *grid = malloc(sizeof(MapGrid));
    if (!grid) return NULL;

    grid->width = width;
    grid->height = height;
    grid->tile_width = tile_width;
    grid->tile_height = tile_height;

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

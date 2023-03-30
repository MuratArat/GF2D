#include "player.h"
#include "map_system.h"
#include "combat_manager.h"
#define SCREEN_WIDTH 1216
#define SCREEN_HEIGHT 768


Player *player_new() {
    Player *player = (Player *)malloc(sizeof(Player));
    if (!player) return NULL;
    memset(player, 0, sizeof(Player));
    return player;
}

void player_free(Player *player) {
    if (!player) return;
    gf2d_sprite_free(player->sprite);
    free(player);
}

void player_init(Player *player) {
    player->position = vector2d(608, 384);
    player->sprite = gf2d_sprite_load_image("assets/player.png");
}

void player_draw(Player *player)
{
    if (!player) return;

    gf2d_sprite_draw_image(
        player->sprite,
        vector2d(player->position.x * 32, player->position.y * 32));
}


void player_update(Player *player, const Uint8 *keys, MapGrid *map_grid)
{
    if (!player || !keys || !map_grid)
        return;

    Vector2D new_position = player->position;
    int wrapped = 0; // Add a flag to check if the player has wrapped around

    if (keys[SDL_SCANCODE_W])
    {
        new_position.y -= map_grid->tile_height;
        if (new_position.y < 0)
        {
            new_position.y = (map_grid->height - 1) * map_grid->tile_height;
            wrapped = 1;
        }
    }
    else if (keys[SDL_SCANCODE_S])
    {
        new_position.y += map_grid->tile_height;
        if (new_position.y >= map_grid->height * map_grid->tile_height)
        {
            new_position.y = 0;
            wrapped = 1;
        }
    }
    else if (keys[SDL_SCANCODE_A])
    {
        new_position.x -= map_grid->tile_width;
        if (new_position.x < 0)
        {
            new_position.x = (map_grid->width - 1) * map_grid->tile_width;
            wrapped = 1;
        }
    }
    else if (keys[SDL_SCANCODE_D])
    {
        new_position.x += map_grid->tile_width;
        if (new_position.x >= map_grid->width * map_grid->tile_width)
        {
            new_position.x = 0;
            wrapped = 1;
        }
    }
    if (wrapped)
    {
        regenerate_map(map_grid); // Regenerate the map if the player wraps around
        player->position = new_position;
    }
    else if (enter_combat(map_grid, new_position))
    {
        
    }
    
    else if (!map_grid_is_colliding(map_grid, new_position))
    {
        player->position = new_position;
    }
}

int map_grid_is_colliding(MapGrid *map_grid, Vector2D position)
{
    if (!map_grid)
        return 0;

    int x = (int)(position.x / map_grid->tile_width);
    int y = (int)(position.y / map_grid->tile_height);

    // Check if the position is within the bounds of the map grid
    if (x < 0 || y < 0 || x >= map_grid->width || y >= map_grid->height)
    {
        return 1; // The position is out of bounds; consider it a collision
    }

    // Assuming the wall tiles have a value of 1 in your MapGrid
    if (map_grid->data[y * map_grid->width + x] == 1)
    {
        return 1; // There's a collision with a wall
    }
    return 0; // No collision
}

void save_player_position(const char *filename, int x, int y)
{
    SJson *player_position = sj_object_new();
    sj_object_insert(player_position, "x", sj_new_int(x));
    sj_object_insert(player_position, "y", sj_new_int(y));

    sj_save(player_position, filename);

    sj_free(player_position);
}

int load_player_position(const char *filename, int *x, int *y)
{
    SJson *player_position = sj_load(filename);

    sj_get_integer_value(sj_object_get_value(player_position, "x"), x);
    sj_get_integer_value(sj_object_get_value(player_position, "y"), y);

    sj_free(player_position);

    return 1;
}



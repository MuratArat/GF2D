#include "player.h"
#include "map_system.h"
#include "combat_manager.h"
#define SCREEN_WIDTH 1216
#define SCREEN_HEIGHT 768
#include <SDL_mixer.h>
#include <SDL_ttf.h>

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


void player_update(Player *player, const Uint8 *keys, MapGrid *map_grid,MapGrid *current_map_grid)
{
    if (!player || !keys || !map_grid)
        return;

    Vector2D new_position = player->position;
    int wrapped = 0;

    // Calculate map_x and map_y
    player->map_x = (int)(player->position.x / SCREEN_WIDTH);
    player->map_y = (int)(player->position.y / SCREEN_HEIGHT);

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
        int start_x = (int)(new_position.x / map_grid->tile_width);
        int start_y = (int)(new_position.y / map_grid->tile_height);
        load_map(("json/map%d%d.json", player->map_x, player->map_y),NULL);
        //regenerate_map(map_grid, start_x, start_y);
        player->position = new_position;
    }
    else if (!map_grid_is_colliding(map_grid, new_position,player)&&!player->edit==1)
    {
        player->position = new_position;// Update the player's position
        if(map_grid->data[(int)new_position.y/64 * map_grid->width + (int)new_position.x/64]==2){
            //set_incombat(1);
            map_grid->data[(int)new_position.y/64 * map_grid->width + (int)new_position.x/64]=0;
            map_grid->quest_value--;
            //enemy encounter
            //combat
        }
        else if(map_grid->data[(int)new_position.y/64 * map_grid->width + (int)new_position.x/64]==3){
            player->gold+=10;
            map_grid->data[(int)new_position.y/64 * map_grid->width + (int)new_position.x/64]=0;
            //add item to inventory
        }
        else if(map_grid->data[(int)new_position.y/64 * map_grid->width + (int)new_position.x/64]==4){
            if(player->gold>=10){
                player->gold-=10;
                map_grid->data[(int)new_position.y/64 * map_grid->width + (int)new_position.x/64]=0;
                //open shop
            }
        }
        if(map_grid->data[(int)new_position.y/64 * map_grid->width + (int)new_position.x/64]==5){
            if(is_quest_complete(map_grid)){
                player->gold+=100;
            }
            regenerate_map(map_grid,(int)new_position.x,(int)new_position.y);//stairs
        }
    }
}

int map_grid_is_colliding(MapGrid *map_grid, Vector2D position,Player *player)
{
    if (!map_grid)
        return 0;

    int x = (int)(position.x / map_grid->tile_width);
    int y = (int)(position.y / map_grid->tile_height);
    if(player->edit==1){
       return 0;
    }
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

void save_player_position(const char *filename, int x, int y,int map_x,int map_y,int gold,int edit)
{
    SJson *player_position = sj_object_new();
    sj_object_insert(player_position, "x", sj_new_int(x));
    sj_object_insert(player_position, "y", sj_new_int(y));
    sj_object_insert(player_position, "map_x", sj_new_int(map_x));
    sj_object_insert(player_position, "map_y", sj_new_int(map_y));
    sj_object_insert(player_position, "gold" , sj_new_int(gold));
    sj_object_insert(player_position, "edit" , sj_new_int(edit));
    sj_save(player_position, filename);

    sj_free(player_position);
}

void load_player_x(const char *filename, int *x)
{
    SJson *player_position = sj_load(filename);
    sj_get_integer_value(sj_object_get_value(player_position, "x"), x);
    sj_free(player_position);
}
void load_player_y(const char *filename, int *y)
{
    SJson *player_position = sj_load(filename);
    sj_get_integer_value(sj_object_get_value(player_position, "y"), y);
    sj_free(player_position);
}
void load_player_map_x(const char *filename, int *map_x)
{
    SJson *player_position = sj_load(filename);
    sj_get_integer_value(sj_object_get_value(player_position, "map_x"), map_x);
    sj_free(player_position);
}
void load_player_map_y(const char *filename, int *map_y)
{
    SJson *player_position = sj_load(filename);
    sj_get_integer_value(sj_object_get_value(player_position, "map_y"), map_y);
    sj_free(player_position);
}
void load_player_gold(const char *filename, int *gold)
{
    SJson *player_position = sj_load(filename);
    sj_get_integer_value(sj_object_get_value(player_position, "gold"), gold);
    sj_free(player_position);
}
void load_player_edit(const char *filename, int *edit)
{
    SJson *player_position = sj_load(filename);
    sj_get_integer_value(sj_object_get_value(player_position, "edit"), edit);
    sj_free(player_position);
}

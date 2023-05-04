#include <SDL.h>
#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "combat_manager.h"
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "gf2d_draw.h"
#include "text.h"
#include "sounds.h"
#include "shop.h"
#include "inventory.h"
#include "map_system.h"
#include "player.h"

#define WIDTH 19
#define HEIGHT 12
#define CELL_SIZE 64
#define PLAYER_UPDATE_DELAY 100 // Define PLAYER_UPDATE_DELAY

int main(int argc, char *argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 *keys;
    //Sprite *sprite;
    int playerX, playerY;
    Vector2D playerPosition, playerPositionDraw;

    int random_value;
    Hero hero1,hero2,hero3,hero4;
    Monster monster;
    int mx, my;
    float mf = 0;
    Sprite *mouse, *start;
    Color mouseColor = gfc_color8(255, 100, 0, 200);
    Player *player = player_new();
    int player_gold = 0;

    /*program initializtion*/
    init_logger("gf2d.log", 0);
    slog("---==== BEGIN ====---");
    gf2d_graphics_initialize(
        "gf2d",
        WIDTH * CELL_SIZE,
        HEIGHT * CELL_SIZE,
        WIDTH * CELL_SIZE,
        HEIGHT * CELL_SIZE,
        vector4d(0, 0, 0, 255),
        0);
    gf2d_graphics_set_frame_delay(16);
    gf2d_sprite_init(1024);
    SDL_ShowCursor(SDL_DISABLE);

    /*library intialization*/
    TTF_Init();
    SDL_Color color = { 255, 255, 255 };
    TTF_Font* font = TTF_OpenFont("font/ChrustyRock-ORLA.ttf", 16);
    if (font == NULL) {
        slog("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
    }
    else {
        slog("Font loaded!");
    }
    SDL_Renderer* renderer = gf2d_graphics_get_renderer();
    char goldText[64];
    char questText[64];
    Text *text_gold = Text_Load(font, goldText, color, renderer);
    Text *text_quest = Text_Load(font, questText, color, renderer);

    /*demo setup*/
//   spriteBG = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png", 32, 32, 16, 0);
    start = gf2d_sprite_load_image("images/backgrounds/Start.png");

    /*sound setup*/
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Chunk *moveSound = Mix_LoadWAV("sounds/click.wav");//load sound file
    Mix_Chunk *bgmusic = Mix_LoadMUS("sounds/bgmusic.mp3");
    Mix_Chunk *battlemusic = Mix_LoadMUS("sounds/battle.mp3");
    Mix_Chunk *startcombat = Mix_LoadMUS("sounds/startcombat.mp3");
    //load music file


    /*map setup*/

    Sprite *player_sprite = gf2d_sprite_load_image("images/player.png");
    player_init(player);
    load_player_x("json/player_pos.json", &player->x);
    load_player_y("json/player_pos.json", &player->y);
    load_player_map_x("json/player_pos.json", &player->map_x);
    load_player_map_y("json/player_pos.json", &player->map_y);
    load_player_gold("json/player_pos.json", &player->gold);
    playerPosition = vector2d(player->x, player->y);
    player->position = playerPosition;
    player->sprite = player_sprite;
    playerX = player->x;
    playerY = player->y;
    while(playerX >= 1216)
    {
        playerX -= 1216;
    }
    while(playerY >= 768)
    {
        playerY -= 768;
    }
    playerPositionDraw = vector2d(playerX, playerY);
    slog("loaded player position");
    slog("Player position: %f, %f", player->position.x, player->position.y);
    slog("Player map position: %d, %d", player->map_x, player->map_y);

    MapGrid *map_grid = load_map("json/map.json",NULL);
    slog("loaded map");
    slog("map_grid width: %i", map_grid->width);
    slog("map_grid height: %i", map_grid->height);
    slog("map_grid tile width: %i", map_grid->tile_width);
    slog("map_grid tile height: %i", map_grid->tile_height);

    char current_map_grid_name[50];
    sprintf(current_map_grid_name, "json/map%i%i.json", player->map_x, player->map_y);
    MapGrid *current_map_grid = load_map((current_map_grid_name),NULL);
    slog(current_map_grid_name);
    slog("current_map_grid width: %i", current_map_grid->width);
    slog("current_map_grid height: %i", current_map_grid->height);
    slog("current_map_grid tile width: %i", current_map_grid->tile_width);
    slog("current_map_grid tile height: %i", current_map_grid->tile_height);


    Uint32 lastPlayerUpdate = 0; // Add a variable to store the elapsed time since the last player movement

    /*main game loop*/
    //make a start screen that asks for the player to press enter to start
    while(!done)
    {
        SDL_PumpEvents(); // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        SDL_GetMouseState(&mx, &my);
        mf += 0.1;

        if (mf >= 16.0)
            mf = 0;
        if (keys[SDL_SCANCODE_RETURN])
        {
            done = 1;
        }
        gf2d_graphics_clear_screen(); // clears drawing buffers
        gf2d_sprite_draw_image(start, vector2d(0, 0));
        gf2d_sprite_draw(
            mouse,
            vector2d(mx, my),
            1,1,
            NULL,
            NULL,
            NULL,
            &mouseColor,
            (int)mf);
        gf2d_graphics_next_frame();

        // render current draw frame and skip to the next frame
    }
    done = 0;
    while (!done)
    {

        SDL_PumpEvents(); // update SDL's internal event structures
        keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
        /*update things here*/
        SDL_GetMouseState(&mx, &my);
        mf += 0.1;

        if (mf >= 16.0)
            mf = 0;

        gf2d_graphics_clear_screen(); // clears drawing buffers

        // all drawing should happen between clear_screen and next_frame
        if(get_incombat()==0)
        {
            // draw the map
            current_map_grid = load_map((current_map_grid_name),NULL);
            sprintf(current_map_grid_name, "json/map%i%i.json", player->map_x, player->map_y);
            map_grid_draw(current_map_grid);
            Uint32 currentTime = SDL_GetTicks(); // Get the current time

            // move the player based on keyboard input
            if (currentTime - lastPlayerUpdate >= PLAYER_UPDATE_DELAY) // Check if enough time has passed
            {
                Vector2D prev_position = player->position; // Store the player's previous position
                int prev_map_x = player->map_x;
                int prev_map_y = player->map_y;

                player_update(player, keys, map_grid,current_map_grid); // Update the player's position
                if (!vector2d_equal(player->position, prev_position)) 
                {
                    if(player->map_x != prev_map_x || player->map_y != prev_map_y)
                    {
                        slog("Player map position: %d, %d", player->map_x, player->map_y);
                        sprintf(current_map_grid_name, "json/map%i%i.json", player->map_x, player->map_y);
                        current_map_grid = load_map((current_map_grid_name),NULL);
                        slog(current_map_grid_name);
                        slog("current_map_grid width: %i", current_map_grid->width);
                        slog("current_map_grid height: %i", current_map_grid->height);
                        slog("current_map_grid tile width: %i", current_map_grid->tile_width);
                        slog("current_map_grid tile height: %i", current_map_grid->tile_height);
                    }
                    playerPosition=player->position;
                    player->x = playerPosition.x;
                    player->y = playerPosition.y;
                    playerX = player->x;
                    playerY = player->y;
                    while(playerX >= 1216)
                    {
                        playerX -= 1216;
                    }
                    while(playerY >= 768)
                    {
                        playerY -= 768;
                    }
                    playerPositionDraw = vector2d(playerX, playerY);
                    Mix_PlayChannel(-1, moveSound, 0); // Play the moveSound
                    //play startcombat
                }

                lastPlayerUpdate = currentTime; // Update the last player movement time
            }
            // draw the player sprite
            gf2d_sprite_draw(
                player->sprite,
                playerPositionDraw,
                1,
                1,
                NULL, // NULL for rotation to avoid rotating the image
                NULL, // NULL for flip to avoid flipping the image
                NULL, // NULL for color
                NULL, // NULL for frame to use the entire image
                0     // 0 for layer to draw on the default layer
            );
        }
        else if(get_incombat()==1)
        {
            random_value = rand() % 100; // random value between 0 and 99
            slog("rand=%d",random_value);
            if (random_value < 20) {
                start_combat(1);
            }
            else if (random_value < 40) {
                start_combat(2);
            }
            else if (random_value < 60) {
                start_combat(3);
            }
            else if (random_value < 80) {
                start_combat(4);
            }
            else if (random_value <= 100) {
                start_combat(5);
            }
        }

        // draw the mouse pointer
        gf2d_sprite_draw(
            mouse,
            vector2d(mx, my),
            1,1,
            NULL,
            NULL,
            NULL,
            &mouseColor,
            (int)mf);


        char goldText[64];
        sprintf(goldText, "Gold: %d", player->gold);

        char questText[64];
        if(!(is_quest_complete(map_grid)<=1))
        {
        sprintf(questText, "kill %i monsters ", map_grid->quest_value);
        }
        slog("quest_value: %i", map_grid->quest_value);
        if(is_quest_complete(map_grid)<=1)
        {
            sprintf(questText, "Quest Complete!");
        }

        text_gold = Text_Load(font, goldText, color, renderer);
        text_quest = Text_Load(font, questText, color, renderer);

        Text_Render(text_gold, renderer, 0, 0);
        Text_Render(text_quest, renderer, 0, 20);

        gf2d_graphics_next_frame(); // render current draw frame and skip to the next frame

        if (keys[SDL_SCANCODE_ESCAPE])
        {
            done = 1; // exit condition
        }
        if (keys[SDL_SCANCODE_P])
        {
            if(player->edit==0)
            {
                player->edit=1;
            }
            else
            {
                player->edit=0;
            }
        }
        if (keys[SDL_SCANCODE_0])
        {
            if(player->edit==1)
            {
                map_grid->data[map_grid->width * player->x + player->y] = 0;
            }
        }
        if (keys[SDL_SCANCODE_1])
        {
            if(player->edit==1)
            {
                map_grid->data[map_grid->width * player->x + player->y] = 1;
            }
        }
        if (keys[SDL_SCANCODE_3])
        {
            if(player->edit==1)
            {
                map_grid->data[map_grid->width * player->x + player->y] = 3;
            }
        }
        if (keys[SDL_SCANCODE_4])
        {
            if(player->edit==1)
            {
                map_grid->data[map_grid->width * player->x + player->y] = 4;
            }
        }
        if (keys[SDL_SCANCODE_5])
        {
            if(player->edit==1)
            {
               map_grid->data[map_grid->width * player->x + player->y] = 5;
            }
        }
        if (keys[SDL_SCANCODE_R])
        {
            regenerate_map(map_grid,player->x,player->y);
        }
        if (keys[SDL_SCANCODE_Q])
        {
            save_player_position("json/player_pos.json", player->x, player->y,player->map_x, player->map_y,player->gold,player->edit);
            save_map("json/map.json", map_grid);
            sprintf(current_map_grid_name, "json/map%i%i.json", player->map_x, player->map_y);
            save_map(current_map_grid_name, current_map_grid);
            slog("saved");
            slog("player pos x=%f y=%f", player->x, player->y);
            slog("player map pos x=%d y=%d", player->map_x, player->map_y);
            slog("map_grid width: %i", map_grid->width);
            slog("map_grid height: %i", map_grid->height);
            slog("map_grid tile width: %i", map_grid->tile_width);
            slog("map_grid tile height: %i", map_grid->tile_height);
            slog(current_map_grid_name);
            slog("current_map_grid width: %i", current_map_grid->width);
            slog("current_map_grid height: %i", current_map_grid->height);
            slog("current_map_grid tile width: %i", current_map_grid->tile_width);
            slog("current_map_grid tile height: %i", current_map_grid->tile_height);
            split_and_save_maps(map_grid);
            set_incombat(0);
        }
        split_and_save_maps(map_grid);

    }
    //player cleanup
    player_free(player);
    //text cleanup
    Text_Free(text_gold);
    Text_Free(text_quest);
    TTF_CloseFont(font);
    Text_Quit();
    /*sound cleanup*/
    Mix_CloseAudio();

    /*program exit*/
    slog("---==== END ====---");
    return 0;
}

/*eol@eof*/

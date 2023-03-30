#include <SDL.h>
#include "simple_logger.h"

#include "gf2d_graphics.h"
#include "gf2d_sprite.h"
#include "combat_manager.h"
#include <SDL_mixer.h>

#include "map_system.h"
#include "player.h"

#define WIDTH 19
#define HEIGHT 12
#define CELL_SIZE 64
#define PLAYER_UPDATE_DELAY 200 // Define PLAYER_UPDATE_DELAY

int main(int argc, char *argv[])
{
    /*variable declarations*/
    int done = 0;
    const Uint8 *keys;
    //    Sprite *sprite;
    int playerX, playerY;
    int random_value;
    Hero hero1,hero2,hero3,hero4;
    Monster monster;
    int mx, my;
    float mf = 0;
    Sprite *mouse;
    Color mouseColor = gfc_color8(255, 100, 0, 200);
    Player *player = player_new();


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

    /*demo setup*/
 //   spriteBG = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    mouse = gf2d_sprite_load_all("images/pointer.png", 32, 32, 16, 0);


    /*sound setup*/
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Chunk *moveSound = Mix_LoadWAV("sounds/click.wav");

    /*map setup*/
    // Load the images for each tile type

    Sprite *player_sprite = gf2d_sprite_load_image("images/player.png");

    MapGrid *map_grid = load_map("json/map.json",19, 12, NULL);
    //generate_map(19, 12, 64, 64, NULL);
    // TODO: Implement map loading from file

    load_player_position("json/player_pos.json", &playerX, &playerY); // Set the initial position of the player
    player_init(player);
    player->position = vector2d(playerX, playerY);
    player->sprite = player_sprite;

    Uint32 lastPlayerUpdate = 0; // Add a variable to store the elapsed time since the last player movement

    /*main game loop*/
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
            map_grid_draw(map_grid);
            //slog("%d",map_grid->height);
            Uint32 currentTime = SDL_GetTicks(); // Get the current time

            // move the player based on keyboard input
            if (currentTime - lastPlayerUpdate >= PLAYER_UPDATE_DELAY) // Check if enough time has passed
            {
                Vector2D prev_position = player->position; // Store the player's previous position
                player_update(player, keys, map_grid);

                if (!vector2d_equal(player->position, prev_position))
                {
                    Mix_PlayChannel(-1, moveSound, 0); // Play the moveSound
                }

                lastPlayerUpdate = currentTime; // Update the last player movement time
            }
            // draw the player sprite
            gf2d_sprite_draw(
                player->sprite,
                player->position,
                NULL, // NULL for scale to use the image's original size
                NULL, // NULL for rotation to avoid rotating the image
                NULL, // NULL for flip to avoid flipping the image
                NULL, // NULL for color
                NULL, // NULL for frame to use the entire image
                0     // 0 for layer to draw on the default layer
            );
            }
            else if(get_startcombat())
            {
                set_incombat(true);
                random_value = rand() % 100;
                if (random_value < 20) {random_value=1;start_combat(random_value);}
                else if (random_value < 40) {random_value=2;start_combat(random_value);}
                else if (random_value < 60) {random_value=3;start_combat(random_value);}
                else if (random_value < 80) {random_value=4;start_combat(random_value);}
                else if (random_value <= 100) {random_value=5;start_combat(random_value);}
            }
            else if(get_incombat())
            {
              update_combat(&hero1, &hero2, &hero3, &hero4, &monster, random_value);

            }

            // draw the mouse pointer
            gf2d_sprite_draw(
                mouse,
                vector2d(mx, my),
                NULL,
                NULL,
                NULL,
                NULL,
                &mouseColor,
                (int)mf);

            gf2d_graphics_next_frame(); // render current draw frame and skip to the next frame

            if (keys[SDL_SCANCODE_ESCAPE])
            {
                done = 1; // exit condition
            }
            if (keys[SDL_SCANCODE_Q])
        {
            save_player_position("json/player_pos.json", player->position.x, player->position.y);
            save_map("json/map.json",map_grid);
            set_incombat(0);
        }
        }

        player_free(player);

        /*sound cleanup*/
        Mix_CloseAudio();

        /*program exit*/
        slog("---==== END ====---");
        return 0;
    }
    /*eol@eof*/
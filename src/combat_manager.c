#include <stdlib.h>
#include "simple_json.h"
#include "combat_manager.h"
#include "gf2d_draw.h"
#include "gf2d_graphics.h"
#include "simple_logger.h"
#include <SDL.h>

Bool incombat;
Bool startcombat;
Sprite *spriteBG;
Hero heroes[MAX_HEROES];
Monster current_monster;



void draw_health_bar(Vector2D position, float health_percentage) {
    Sprite *health_green = gf2d_sprite_load_image("images/health_green.png");
    Sprite *health_red = gf2d_sprite_load_image("images/health_red.png");
    Vector2D red_position = vector2d(position.x, position.y + health_green->frame_h * (1.0 - health_percentage));
    gf2d_sprite_draw_image(health_red, red_position);
    gf2d_sprite_draw_image(health_green, position);
    slog("Drawing health bar at position (%f, %f) with health percentage of %f", position.x, position.y, health_percentage);
}

Hero load_hero_data(const char *filename) {
    SJson *hero_json = sj_load(filename);

    int id, damage, crit, defense, dodge, maxhealth, health, speed, points;

    id = sj_get_integer_value(sj_object_get_value(hero_json, "id"), &id);
    damage = sj_get_integer_value(sj_object_get_value(hero_json, "damage"), &damage);
    crit = sj_get_integer_value(sj_object_get_value(hero_json, "crit"), &crit);
    defense = sj_get_integer_value(sj_object_get_value(hero_json, "defense"), &defense);
    dodge = sj_get_integer_value(sj_object_get_value(hero_json, "dodge"), &dodge);
    maxhealth = sj_get_integer_value(sj_object_get_value(hero_json, "maxhealth"), &maxhealth);
    health = sj_get_integer_value(sj_object_get_value(hero_json, "health"), &health);
    speed = sj_get_integer_value(sj_object_get_value(hero_json, "speed"), &speed);
    points = sj_get_integer_value(sj_object_get_value(hero_json, "points"), &points);

    Hero hero = { id, damage, crit, defense, dodge, maxhealth, health, speed, points };

    sj_free(hero_json);
    slog("Loaded hero data from %s: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, points=%d", filename, hero.id, hero.damage, hero.crit, hero.defense, hero.dodge, hero.maxhealth, hero.health, hero.speed, hero.points);

    return hero;
}

Monster load_monster_data(int monster_id) {
    // Load monster stats from monster.json
    char filename[50];
    sprintf(filename, "json/monster%d.json", monster_id);
    SJson *monster_json = sj_load(filename);

    int id, damage, crit, defense, dodge, maxhealth, health, speed;

    id = sj_get_integer_value(sj_object_get_value(monster_json, "id"), &id);
    damage = sj_get_integer_value(sj_object_get_value(monster_json, "damage"), &damage);
    crit = sj_get_integer_value(sj_object_get_value(monster_json, "crit"), &crit);
    defense = sj_get_integer_value(sj_object_get_value(monster_json, "defense"), &defense);
    dodge = sj_get_integer_value(sj_object_get_value(monster_json, "dodge"), &dodge);
    maxhealth = sj_get_integer_value(sj_object_get_value(monster_json, "maxhealth"), &maxhealth);
    health = sj_get_integer_value(sj_object_get_value(monster_json, "health"), &health);
    speed = sj_get_integer_value(sj_object_get_value(monster_json, "speed"), &speed);

    sj_free(monster_json);

    Monster monster = { id, damage, crit, defense, dodge, maxhealth, health, speed };

    slog("Loaded monster data for id %d: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d", monster_id, monster.id, monster.damage, monster.crit, monster.defense, monster.dodge, monster.maxhealth, monster.health, monster.speed);

    return monster;
}

void start_combat(int enemy_id)
{
    set_startcombat(false);
    set_incombat(true);
    // Load hero stats from heros.json
    Hero hero1 = load_hero_data("json/hero1.json");
    Hero hero2 = load_hero_data("json/hero2.json");
    Hero hero3 = load_hero_data("json/hero3.json");
    Hero hero4 = load_hero_data("json/hero4.json");

    // Load monster data
    Monster monster = load_monster_data(enemy_id);

    // Start combat with the specified enemy_id
    draw_combat_screen(&hero1, &hero2, &hero3, &hero4, enemy_id);
//    update_combat(&hero1, &hero2, &hero3, &hero4, &monster, enemy_id);
}

Bool get_incombat()
{
    return incombat;
}

void set_incombat(Bool value)
{
    incombat = value;
}

Bool get_startcombat()
{
    return startcombat;
}

void set_startcombat(Bool value)
{
    startcombat = value;
}

int enter_combat(MapGrid *map_grid, Vector2D position)
{
    slog("Entering combat at position (%f, %f)", position.x, position.y);
    int x = (int)(position.x / map_grid->tile_width);
    int y = (int)(position.y / map_grid->tile_height);

    // Assuming the enemy tiles have a value of 2 in your MapGrid
    if (map_grid->data[y * map_grid->width + x] == 2)
    {
        // Replace the enemy tile with an empty tile (value 0)
        map_grid->data[y * map_grid->width + x] = 0;
        set_incombat(true);
        set_startcombat(true);
    }
    return 0; // No combat entered
}



void draw_combat_screen(Hero *hero1, Hero *hero2, Hero *hero3, Hero *hero4, int enemy_id) {
    slog("drawing background");
    draw_combat_background();
    slog("drawing heroes");
    draw_heroes(hero1,hero2,hero3,hero4);
    slog("drawing monster");
    draw_monster(enemy_id);
}


void draw_combat_background() {
  // Load and draw the combat background
  spriteBG = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
  gf2d_sprite_draw_image(spriteBG, vector2d(0, 0));
}



void draw_heroes(Hero *hero1, Hero *hero2, Hero *hero3, Hero *hero4) {
    Hero *heroes[4] = {hero1, hero2, hero3, hero4};

    for (int i = 0; i < 4; i++) {
        if (heroes[i] == NULL) {
            continue;
        }
        slog("drawing heros");
        char hero_sprite_path[64];
        snprintf(hero_sprite_path, sizeof(hero_sprite_path), "images/heroes/hero%d.png", i + 1);
        Sprite *hero_sprite = gf2d_sprite_load_image(hero_sprite_path);
        Vector2D hero_position = vector2d(100, 100 + i * 100);
        gf2d_sprite_draw_image(hero_sprite, hero_position);

        // Draw the health bar for each hero
        Vector2D health_bar_position = vector2d(hero_position.x + hero_sprite->frame_w + 10, hero_position.y);
        float health_percentage = (float)heroes[i]->health / (float)heroes[i]->maxhealth;
        draw_health_bar(health_bar_position, health_percentage);
    }
}



void draw_monster(int monster_id) {
    char monster_sprite_path[64];
    snprintf(monster_sprite_path, sizeof(monster_sprite_path), "images/monsters/monster%d.png", monster_id + 1);
    Sprite *monster_sprite = gf2d_sprite_load_image(monster_sprite_path);
    Vector2D monster_position = vector2d(400, 200);
    gf2d_sprite_draw_image(monster_sprite, monster_position);
    Monster monster = load_monster_data(monster_id);

    // Draw the health bar for the monster
    Vector2D health_bar_position = vector2d(monster_position.x + monster_sprite->frame_w + 10, monster_position.y);
    float health_percentage = (float)monster.health / (float)monster.maxhealth;
    draw_health_bar(health_bar_position, health_percentage);
}


int handle_input()
{
    // Return the pressed number key (1-9 and 0) or -1 if no number key is pressed
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        slog("handle_input");
        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_1:
                    slog("1");
                    return 1;
                case SDLK_2:
                    slog("2");
                    return 2;
                case SDLK_3:
                    slog("3");
                    return 3;
                case SDLK_4:
                    slog("4");
                    return 4;
                case SDLK_5:
                    slog("5");
                    return 5;
                case SDLK_6:
                    slog("6");
                    return 6;
                case SDLK_7:
                    slog("7");
                    return 7;
                case SDLK_8:
                    slog("8");
                    return 8;
                case SDLK_9:
                    slog("9");
                    return 9;
                case SDLK_0:
                    slog("0");
                    return 0;
            }
        }
    }
    return -1;
}

int calculate_hero_damage(Hero *attacker, Monster *defender)
{
    int damage=attacker->damage;
    if (rand() % 100 < defender->dodge)
    {
        return 0; // Attack dodged
    }

    if (rand() % 100 < attacker->crit)
    {
        damage *= 2; // Critical hit
    }

    damage -= defender->defense;
    if (damage < 0)
    {
        damage = 0;
    }

    return damage;
}

int calculate_monster_damage(Monster *attacker, Hero *defender)
{
    int damage=attacker->damage;
    if (rand() % 100 < defender->dodge)
    {
    slog("Hero dodged: 0");
        return 0; // Attack dodged
    }

    if (rand() % 100 < attacker->crit)
    {
        damage *= 2; // Critical hit
    slog("Monster crit: %d", damage);
    }

    damage -= defender->defense;
    if (damage < 0)
    {
        damage = 0;
        slog("hero def higher than dmg %d", damage);
    }
    slog("Calculated monster damage: %d", damage);
    return damage;
}

void update_initiative(Hero *hero1, Hero *hero2, Hero *hero3, Hero *hero4, Monster *monster)
{
    slog("Updating initiative...");
    hero1->initiative += hero1->speed;
    slog("hero1 initiative: %i", &hero1->initiative);
    hero2->initiative += hero2->speed;
    slog("hero2 initiative: %i", &hero2->initiative);
    hero3->initiative += hero3->speed;
    slog("hero3 initiative: %i", &hero3->initiative);
    hero4->initiative += hero4->speed;
    slog("hero4 initiative: %i", &hero4->initiative);
    monster->initiative += monster->speed;
    slog("monster initiative: %i", monster->initiative);
}


#include <stdlib.h>     // For rand() function

void update_combat(Hero *hero1, Hero *hero2, Hero *hero3, Hero *hero4, Monster *monster, int enemy_id) {
    if (all_heroes_dead() || monster_dead()) {
        set_incombat(false);
        return;
    }
    
    update_initiative(hero1,hero2,hero3,hero4,monster);
    if (monster->health < 0) {
        monster->health = 0;
    }
    slog("hero1 initiative:: %d", hero1->initiative);
    slog("hero2 initiative:: %d", hero2->initiative);
    slog("hero3 initiative:: %d", hero3->initiative);
    slog("hero4 initiative:: %d", hero4->initiative);

    while (hero1->initiative >= 100) {
        slog("hero1->initiative >= 100");
            draw_combat_screen(hero1,hero2,hero3,hero4,enemy_id);
        int input = 0;
        while (input == -1) {
            slog("1input == -1");
            draw_combat_screen(hero1,hero2,hero3,hero4,enemy_id);
            input = handle_input();
            SDL_Delay(500); // Avoid using 100% CPU while waiting for input
        }
        hero_attack(hero1, monster, input);
        hero1->initiative -= 100;
    }
    while (hero2->initiative >= 100) {
        slog("hero2->initiative >= 100");
        int input = 0;
        while (input == -1) {
            slog("2input == -1");
            input = handle_input();
        }
        hero_attack(hero2, monster, input);
        hero2->initiative -= 100;
    }
    while (hero3->initiative >= 100) {
        slog("hero3->initiative >= 100");
        int input = 0;
        while (input == -1) {
            slog("3input == -1");
            input = handle_input();
            SDL_Delay(500); // Avoid using 100% CPU while waiting for input
        }
        hero_attack(hero3, monster, input);
        hero3->initiative -= 100;
    }
    while (hero4->initiative >= 100) {
        slog("hero4->initiative >= 100");
        int input = 0;
        while (input == -1) {
            slog("4input == -1");
            input = handle_input();
            SDL_Delay(500); // Avoid using 100% CPU while waiting for input
        }
        hero_attack(hero4, monster, input);
        hero4->initiative -= 100;
    }

    int random_hero = rand() % 4; // Generate a random number between 0 and 3
    switch(random_hero) {
        case 0:
            if (hero1->health > 0) { // Check if the selected hero is still alive
                monster_attack(monster, hero1);
            }
            break;
        case 1:
            if (hero2->health > 0) {
                monster_attack(monster, hero2);
            }
            break;
        case 2:
            if (hero3->health > 0) {
                monster_attack(monster, hero3);
            }
            break;
        case 3:
            if (hero4->health > 0) {
                monster_attack(monster, hero4);
            }
            break;
    }

    draw_combat_screen(hero1,hero2,hero3,hero4,enemy_id);
}


void hero_attack(Hero *attacker, Monster *defender, int attack_type) {
    int damage;
    switch (attack_type) {
        case 0: // Normal attack
            damage = calculate_hero_damage(attacker, defender);
            defender->health -= damage;
            break;
        case 1: // Give back 25 initiative to the user
            damage = calculate_hero_damage(attacker, defender);
            defender->health -= damage/2;
            attacker->initiative += 25;
            break;
        case 2: // Deal damage 2 times
            damage = calculate_hero_damage(attacker, defender);
            defender->health -= damage;
            damage = calculate_hero_damage(attacker, defender);
            defender->health -= damage;
            break;
        case 3: // Always crit
            damage = calculate_hero_damage(attacker, defender) * 2;
            defender->health -= damage;
            break;
        case 4: // Give back 75 initiative to the user
            attacker->initiative += 75;
            break;
        case 5: // Reduce the enemy speed for the rest of combat
            defender->speed = (int)(defender->speed * 0.90);
            break;
        case 6: // Raise the user's defense for this combat
            attacker->defense += 5;
            break;
        case 7: // Heal the hero
            attacker->health += 20;
            break;
        case 8: // Increase the user's dodge stat
            attacker->dodge += 5;
            break;
        case 9: // Deal 999 damage to the user and the target
            damage = 999;
            attacker->health -= damage;
            defender->health -= damage;
            break;
    }

    // Ensure monster health does not go below 0
    if (defender->health < 0) {
        defender->health = 0;
    }
}


void monster_attack(Monster *attacker, Hero *defender) {
    int damage = calculate_monster_damage(attacker, defender);
    defender->health -= damage;
    if (defender->health < 0) {
        defender->health = 0;
    }
}

Bool all_heroes_dead() {
    for (int i = 0; i < MAX_HEROES; i++) {
        if (heroes[i].health >= 0) {
            return false;
        }
    }
    set_incombat(false);
    set_startcombat(false);
    return true;
}

Bool monster_dead() {
    if (current_monster.health >= 0) {
        return false;
    }
    set_incombat(false);
    set_startcombat(false);
    return true;
    
}


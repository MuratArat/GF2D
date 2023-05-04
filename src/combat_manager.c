#include <stdlib.h>
#include "simple_json.h"
#include "combat_manager.h"
#include "gf2d_draw.h"
#include "gf2d_graphics.h"
#include "simple_logger.h"
#include <SDL.h>
#include <stdlib.h>

Bool incombat;
Sprite *spriteBG;
Hero heroes[MAX_HEROES];
Monster current_monster;



void draw_health_bar(Vector2D position, float health_percentage) {
    Sprite *health_green = gf2d_sprite_load_image("images/health_green.png");
    Sprite *health_red = gf2d_sprite_load_image("images/health_red.png");
    Vector2D health_position = vector2d(position.x, position.y);
    gf2d_sprite_draw(
        health_red,
        health_position,
        1,1,
        NULL,
        NULL,
        NULL,
        NULL,
        0);
    gf2d_sprite_draw(
        health_green,
        health_position,
        health_percentage,1,
        NULL,
        NULL,
        NULL,
        NULL,
        0);
    slog("Drawing health bar at position (%f, %f) with health percentage of %f", position.x, position.y, health_percentage);
}
void *save_hero_data(Hero *hero, const char *filename)
{
    SJson *hero_json = sj_object_new();
    sj_object_insert(hero_json, "id", sj_new_int(hero->id));
    sj_object_insert(hero_json, "damage", sj_new_int(hero->damage));
    sj_object_insert(hero_json, "crit", sj_new_int(hero->crit));
    sj_object_insert(hero_json, "defense", sj_new_int(hero->defense));
    sj_object_insert(hero_json, "dodge", sj_new_int(hero->dodge));
    sj_object_insert(hero_json, "maxhealth", sj_new_int(hero->maxhealth));
    sj_object_insert(hero_json, "health", sj_new_int(hero->health));
    sj_object_insert(hero_json, "speed", sj_new_int(hero->speed));
    sj_object_insert(hero_json, "item_id", sj_new_int(hero->item_id));

    sj_save(hero_json, filename);
    sj_free(hero_json);
    return NULL;
}

Hero *load_hero_data(const char *filename) {
    SJson *hero_json = sj_load(filename);
    if (!hero_json) {
        printf("Error loading hero data from file.\n");
        return NULL;
    }

    int id, damage, crit, defense, dodge, maxhealth, health, speed, item_id;

    if (!sj_get_integer_value(sj_object_get_value(hero_json, "id"), &id) ||
            !sj_get_integer_value(sj_object_get_value(hero_json, "damage"), &damage) ||
            !sj_get_integer_value(sj_object_get_value(hero_json, "crit"), &crit) ||
            !sj_get_integer_value(sj_object_get_value(hero_json, "defense"), &defense) ||
            !sj_get_integer_value(sj_object_get_value(hero_json, "dodge"), &dodge) ||
            !sj_get_integer_value(sj_object_get_value(hero_json, "maxhealth"), &maxhealth) ||
            !sj_get_integer_value(sj_object_get_value(hero_json, "health"), &health) ||
            !sj_get_integer_value(sj_object_get_value(hero_json, "speed"), &speed) ||
            !sj_get_integer_value(sj_object_get_value(hero_json, "item_id"), &item_id)) {
        printf("Error parsing hero data from file.\n");
        sj_free(hero_json);
        return NULL;
    }

    Hero *hero = (Hero *)malloc(sizeof(Hero));
    sethero(hero, id, damage, crit, defense, dodge, maxhealth, health, speed, item_id);

    sj_free(hero_json);
    return hero;
}

void sethero(Hero *hero, int id, int damage, int crit, int defense, int dodge, int maxhealth, int health, int speed, int item_id) {
    hero->id = id;
    hero->damage = damage;
    hero->crit = crit;
    hero->defense = defense;
    hero->dodge = dodge;
    hero->maxhealth = maxhealth;
    hero->health = health;
    hero->speed = speed;
    hero->item_id = item_id;
}

Monster *load_monster_data(int monster_id) {
    // Load monster stats from monster.json
    char filename[50];
    sprintf(filename, "json/monster%d.json", monster_id);
    SJson *monster_json = sj_load(filename);

    if (!monster_json) {
        printf("Error loading monster data from file.\n");
        return NULL;
    }

    int id, damage, crit, defense, dodge, maxhealth, health, speed;

    if (!sj_get_integer_value(sj_object_get_value(monster_json, "id"), &id) ||
            !sj_get_integer_value(sj_object_get_value(monster_json, "damage"), &damage) ||
            !sj_get_integer_value(sj_object_get_value(monster_json, "crit"), &crit) ||
            !sj_get_integer_value(sj_object_get_value(monster_json, "defense"), &defense) ||
            !sj_get_integer_value(sj_object_get_value(monster_json, "dodge"), &dodge) ||
            !sj_get_integer_value(sj_object_get_value(monster_json, "maxhealth"), &maxhealth) ||
            !sj_get_integer_value(sj_object_get_value(monster_json, "health"), &health) ||
            !sj_get_integer_value(sj_object_get_value(monster_json, "speed"), &speed)) {
        sj_free(monster_json);
        printf("Error reading monster data from JSON.\n");
        return NULL;
    }

    Monster *monster = (Monster *)malloc(sizeof(Monster));
    monster->id = id;
    monster->damage = damage;
    monster->crit = crit;
    monster->defense = defense;
    monster->dodge = dodge;
    monster->maxhealth = maxhealth;
    monster->health = health;
    monster->speed = speed;

    sj_free(monster_json);

    return monster;
}

void start_combat(int enemy_id)
{
    set_incombat(true);
    // Start combat with the specified enemy_id
    draw_combat_screen(enemy_id);
    while(get_incombat())
    {
        update_combat(enemy_id);
    }
}

Bool get_incombat()
{
    return incombat;
}

void set_incombat(Bool value)
{
    incombat = value;
}

void draw_combat_screen(int enemy_id) {
    // Load hero stats from heros.json
    Hero *hero1 = load_hero_data("json/hero1.json");
    slog("Loaded hero data from 1: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero1->id, hero1->damage, hero1->crit, hero1->defense, hero1->dodge, hero1->maxhealth, hero1->health, hero1->speed, hero1->item_id);

    Hero *hero2 = load_hero_data("json/hero2.json");
    slog("Loaded hero data from 2: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero2->id, hero2->damage, hero2->crit, hero2->defense, hero2->dodge, hero2->maxhealth, hero2->health, hero2->speed, hero2->item_id);

    Hero *hero3 = load_hero_data("json/hero3.json");
    slog("Loaded hero data from 3: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero3->id, hero3->damage, hero3->crit, hero3->defense, hero3->dodge, hero3->maxhealth, hero3->health, hero3->speed, hero3->item_id);

    Hero *hero4 = load_hero_data("json/hero4.json");
    slog("Loaded hero data from 4: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero4->id, hero4->damage, hero4->crit, hero4->defense, hero4->dodge, hero4->maxhealth, hero4->health, hero4->speed, hero4->item_id);

    // Load monster data
    Monster *monster = load_monster_data(enemy_id);
    slog("Loaded monster data for id %d: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d", enemy_id, monster->id, monster->damage, monster->crit, monster->defense, monster->dodge, monster->maxhealth, monster->health, monster->speed);

    
    gf2d_graphics_clear_screen();// clears drawing buffers
    slog("drawing background");
    draw_combat_background();
    slog("drawing heroes");
    draw_heroes();
    slog("drawing monster");
    draw_monster(enemy_id);
    gf2d_graphics_next_frame();
}

void draw_combat_background() {
    // Load and draw the combat background
    spriteBG = gf2d_sprite_load_image("images/backgrounds/bg_flat.png");
    gf2d_sprite_draw_image(spriteBG, vector2d(0, 0));
}

void draw_heroes() {
    // Load hero stats from heros.json
    Hero *hero1 = load_hero_data("json/hero1.json");
    slog("Loaded hero data from 1: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero1->id, hero1->damage, hero1->crit, hero1->defense, hero1->dodge, hero1->maxhealth, hero1->health, hero1->speed, hero1->item_id);

    Hero *hero2 = load_hero_data("json/hero2.json");
    slog("Loaded hero data from 2: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero2->id, hero2->damage, hero2->crit, hero2->defense, hero2->dodge, hero2->maxhealth, hero2->health, hero2->speed, hero2->item_id);

    Hero *hero3 = load_hero_data("json/hero3.json");
    slog("Loaded hero data from 3: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero3->id, hero3->damage, hero3->crit, hero3->defense, hero3->dodge, hero3->maxhealth, hero3->health, hero3->speed, hero3->item_id);

    Hero *hero4 = load_hero_data("json/hero4.json");
    slog("Loaded hero data from 4: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero4->id, hero4->damage, hero4->crit, hero4->defense, hero4->dodge, hero4->maxhealth, hero4->health, hero4->speed, hero4->item_id);

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
        slog("hero %d:",i+1);
        slog("hero health: %d",(float)heroes[i]->health);
        slog("hero maxhealth: %d",(float)heroes[i]->maxhealth);
        draw_health_bar(health_bar_position, health_percentage);
    }
}

void draw_monster(int monster_id) {
    char monster_sprite_path[64];
    snprintf(monster_sprite_path, sizeof(monster_sprite_path), "images/monsters/monster%d.png", monster_id + 1);
    Sprite *monster_sprite = gf2d_sprite_load_image(monster_sprite_path);
    Vector2D monster_position = vector2d(400, 200);
    gf2d_sprite_draw_image(monster_sprite, monster_position);
    Monster monster = *load_monster_data(monster_id);

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

int calculate_hero_damage(int attacker_id,int defender_id)
{
    // Load hero stats from heros.json
    char hero[64];
    sprintf(hero, "json/hero%d.json",attacker_id);
    Hero *attacker = load_hero_data(hero);

    // Load monster data
    Monster *defender = load_monster_data(defender_id);

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

int calculate_monster_damage(int attacker_id,int defender_id)
{
    // Load hero stats from heros.json
    char hero[64];
    sprintf(hero, "json/hero%d.json",defender_id);
    Hero *defender = load_hero_data(hero);

    // Load monster data
    Monster *attacker = load_monster_data(attacker_id);

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

void update_initiative(int enemy_id)
{
    // Load hero stats from heros.json
    Hero *hero1 = load_hero_data("json/hero1.json");
    slog("Loaded hero data from 1: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero1->id, hero1->damage, hero1->crit, hero1->defense, hero1->dodge, hero1->maxhealth, hero1->health, hero1->speed, hero1->item_id);

    Hero *hero2 = load_hero_data("json/hero2.json");
    slog("Loaded hero data from 2: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero2->id, hero2->damage, hero2->crit, hero2->defense, hero2->dodge, hero2->maxhealth, hero2->health, hero2->speed, hero2->item_id);

    Hero *hero3 = load_hero_data("json/hero3.json");
    slog("Loaded hero data from 3: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero3->id, hero3->damage, hero3->crit, hero3->defense, hero3->dodge, hero3->maxhealth, hero3->health, hero3->speed, hero3->item_id);

    Hero *hero4 = load_hero_data("json/hero4.json");
    slog("Loaded hero data from 4: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero4->id, hero4->damage, hero4->crit, hero4->defense, hero4->dodge, hero4->maxhealth, hero4->health, hero4->speed, hero4->item_id);

    // Load monster data
    Monster *monster = load_monster_data(enemy_id);
    slog("Loaded monster data for id %d: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d", enemy_id, monster->id, monster->damage, monster->crit, monster->defense, monster->dodge, monster->maxhealth, monster->health, monster->speed);

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

void update_combat(int enemy_id) {
    // Load hero stats from heros.json
    Hero *hero1 = load_hero_data("json/hero1.json");
    slog("Loaded hero data from 1: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero1->id, hero1->damage, hero1->crit, hero1->defense, hero1->dodge, hero1->maxhealth, hero1->health, hero1->speed, hero1->item_id);

    Hero *hero2 = load_hero_data("json/hero2.json");
    slog("Loaded hero data from 2: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero2->id, hero2->damage, hero2->crit, hero2->defense, hero2->dodge, hero2->maxhealth, hero2->health, hero2->speed, hero2->item_id);

    Hero *hero3 = load_hero_data("json/hero3.json");
    slog("Loaded hero data from 3: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero3->id, hero3->damage, hero3->crit, hero3->defense, hero3->dodge, hero3->maxhealth, hero3->health, hero3->speed, hero3->item_id);

    Hero *hero4 = load_hero_data("json/hero4.json");
    slog("Loaded hero data from 4: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero4->id, hero4->damage, hero4->crit, hero4->defense, hero4->dodge, hero4->maxhealth, hero4->health, hero4->speed, hero4->item_id);

    // Load monster data
    Monster *monster = load_monster_data(enemy_id);
    slog("Loaded monster data for id %d: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d", enemy_id, monster->id, monster->damage, monster->crit, monster->defense, monster->dodge, monster->maxhealth, monster->health, monster->speed);

    monster_dead();
    all_heroes_dead();
    slog("update_combat(%i,%i,%i,%i,%i,%i);",hero1->initiative,hero2->initiative,hero3->initiative,hero4->initiative,monster->initiative,enemy_id);
    if (all_heroes_dead() || monster_dead()) {
        set_incombat(false);
        return;
    }

    update_initiative(enemy_id);
    if (monster->health < 0) {
        monster->health = 0;
    }
    slog("hero1 initiative:: %i", hero1->initiative);
    slog("hero2 initiative:: %i", hero2->initiative);
    slog("hero3 initiative:: %i", hero3->initiative);
    slog("hero4 initiative:: %i", hero4->initiative);

    while (hero1->initiative >= 10000) {
        slog("hero1->initiative >= 10000");
        int input = 0;
        hero_attack(hero1, monster, input);
        hero1->initiative -= 10000;
    }
    while (hero2->initiative >= 10000) {
        slog("hero2->initiative >= 10000");
        int input = 0;
        hero_attack(hero2, monster, input);
        hero2->initiative -= 10000;
    }
    while (hero3->initiative >= 10000) {
        slog("hero3->initiative >= 10000");
        int input = 0;
        hero_attack(hero3, monster, input);
        hero3->initiative -= 10000;
    }
    while (hero4->initiative >= 10000) {
        slog("hero4->initiative >= 10000");
        int input = 0;
        hero_attack(hero4, monster, input);
        hero4->initiative -= 10000;
    }
    while (monster->initiative >= 10000) {
        slog("monster->initiative >= 10000");
        monster_attack_random(enemy_id);
        monster->initiative -= 10000;
    }
    draw_combat_screen(enemy_id);
}

void monster_attack_random(int enemy_id) {
    // Load hero stats from heros.json
    Hero *hero1 = load_hero_data("json/hero1.json");
    slog("Loaded hero data from 1: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero1->id, hero1->damage, hero1->crit, hero1->defense, hero1->dodge, hero1->maxhealth, hero1->health, hero1->speed, hero1->item_id);

    Hero *hero2 = load_hero_data("json/hero2.json");
    slog("Loaded hero data from 2: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero2->id, hero2->damage, hero2->crit, hero2->defense, hero2->dodge, hero2->maxhealth, hero2->health, hero2->speed, hero2->item_id);

    Hero *hero3 = load_hero_data("json/hero3.json");
    slog("Loaded hero data from 3: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero3->id, hero3->damage, hero3->crit, hero3->defense, hero3->dodge, hero3->maxhealth, hero3->health, hero3->speed, hero3->item_id);

    Hero *hero4 = load_hero_data("json/hero4.json");
    slog("Loaded hero data from 4: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d, item_id=%d", hero4->id, hero4->damage, hero4->crit, hero4->defense, hero4->dodge, hero4->maxhealth, hero4->health, hero4->speed, hero4->item_id);

    // Load monster data
    Monster *monster = load_monster_data(enemy_id);
    slog("Loaded monster data for id %d: id=%d, damage=%d, crit=%d, defense=%d, dodge=%d, maxhealth=%d, health=%d, speed=%d", enemy_id, monster->id, monster->damage, monster->crit, monster->defense, monster->dodge, monster->maxhealth, monster->health, monster->speed);

    int random_hero = rand() % 4; // Generate a random number between 0 and 3
    switch(random_hero) {
    case 0:
        if (hero1->health > 0) { // Check if the selected hero is still alive
            monster_attack(monster->id, hero1->id);
        }
        break;
    case 1:
        if (hero2->health > 0) {
            monster_attack(monster->id, hero2->id);
        }
        break;
    case 2:
        if (hero3->health > 0) {
            monster_attack(monster->id, hero3->id);
        }
        break;
    case 3:
        if (hero4->health > 0) {
            monster_attack(monster->id, hero4->id);
        }
        break;
    }
}

void hero_attack(int attacker_id, int defender_id, int attack_type) {
    // Load hero stats from heros.json
    char hero[64];
    sprintf(hero, "json/hero%d.json",attacker_id);
    Hero *attacker = load_hero_data(hero);

    // Load monster data
    Monster *defender = load_monster_data(defender_id);

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

void monster_attack(int attacker_id, int defender_id) {
    // Load hero stats from heros.json
    char hero[64];
    sprintf(hero, "json/hero%d.json",defender_id);
    Hero *defender = load_hero_data(hero);
    // Load monster data
    Monster *attacker = load_monster_data(attacker_id);
    int damage = calculate_monster_damage(attacker_id, defender_id);
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
    return true;
}

Bool monster_dead() {
    if (current_monster.health >= 0) {
        return false;
    }
    set_incombat(false);
    return true;
}
/*eol@eof*/
#include "raylib.h"

#define PILLARS 8
#define RESOURCES "resources"

struct player {
    float radius;
    float jump_force;
    float velocity_y;
    Color color;
    Vector2 position;
};

struct pillar {
    int x;
    int y;
    bool not_passed;
};

struct pillars {
    int gap; // Gap between upper & lower pillar
    int distance; // Distance between pillar & it's adjacent
    int width;
    int height;
    float speed;
    struct pillar objects[PILLARS];
};

enum state {
    STATE_MENU,
    STATE_MAIN, // The Main Gameloop
    STATE_PAUSE,
};

struct game {
    enum state state;
    const char *title;
    Vector2 window;
    Music music;
    float dt;
    float gravity;
    bool is_game_over;
    int score;
    int highscore;
    struct player player;
    struct pillars pillars;
};

void init_player(struct player *player, Vector2 window) {
    player->position.x = window.x / 2.5; 
    player->position.y = window.y / 2;
    player->radius = 30; // TODO: adjust radius to window
    player->jump_force = -300;
    player->velocity_y = 0;
    player->color = RAYWHITE;
}

void init_pillars(struct pillars *pillars, Vector2 window) {
    pillars->gap = 200;
    pillars->distance = 300;
    pillars->width = 70;
    pillars->height = 600;
    pillars->speed = -100;
    
    int x = window.x;
    int gap = pillars->gap / 2;
    for (int i = 0; i < PILLARS; ++i) {
        struct pillar *pillar = pillars->objects + i;
        pillar->x = x;
        pillar->y = GetRandomValue(0 + gap, window.y - gap);
        pillar->not_passed = true;

        x += pillars->distance;
    }
}

void init_highscore(int *highscore) {
    if (!FileExists(RESOURCES "/highscore")) {
        int data = 0;
        SaveFileData(RESOURCES "/highscore", &data, sizeof data);
    }

    int size = 0;
    unsigned char *data = LoadFileData(RESOURCES "/highscore", &size);
    *highscore = *(int *)data;
    UnloadFileData(data);
}

void init(struct game *game) {
    game->title = "Floppy Dot";
    game->window.x = 800;
    game->window.y = 800;
    game->dt = 0;
    game->score = 0;
    game->gravity = 600;
    game->is_game_over = false;
    game->state = STATE_MENU;
    
    InitAudioDevice(); // Exception from start()
    game->music = LoadMusicStream(RESOURCES "/music.mp3");

    init_player(&game->player, game->window);
    init_pillars(&game->pillars, game->window);
    init_highscore(&game->highscore);
}

void start(struct game *game) {
    InitWindow(game->window.x, game->window.y, game->title);
    SetTargetFPS(60);
}

static inline bool signal_jump(void) {
    return IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void update_player(struct player *player, float gravity, float window_y, bool *is_game_over, float dt) {
    
    if (player->position.y + player->radius < -20 || player->position.y - player->radius > window_y + 50) {
        *is_game_over = true;
    }

    if (signal_jump()) {
        player->velocity_y = player->jump_force;
    } else {
        player->velocity_y += gravity * dt;
    }
    player->position.y += player->velocity_y * dt;
}

void update_pillars(struct pillars *pillars, const struct player *player, int *score, bool *is_game_over, float window_y, float dt) {
    int gap = pillars->gap / 2;
    Rectangle rec = {.width = pillars->width, .height = pillars->height};
    for (int i = 0; i < PILLARS; ++i) {
        struct pillar *pillar = pillars->objects + i;
        
        rec.x = pillar->x;
        rec.y = pillar->y + gap;
        *is_game_over |= CheckCollisionCircleRec(player->position, player->radius, rec);
        rec.y = pillar->y - gap - pillars->height;
        *is_game_over |= CheckCollisionCircleRec(player->position, player->radius, rec);

        if (pillar->not_passed && pillar->x + pillars->width < player->position.x - player->radius) {
            pillar->not_passed = false;
            ++*score;
        }

        if (pillar->x + pillars->width < 0) {
            int j = i - 1;
            if (j == -1) {
                j = PILLARS - 1;
            }
            pillar->not_passed = true;
            pillar->x = pillars->objects[j].x + pillars->distance;
            pillar->y = GetRandomValue(0 + gap, window_y - gap);
        } else {
            pillar->x += pillars->speed * dt;
        }
    }
}

static inline bool signal_pause(void) {
    return IsKeyPressed(KEY_P);
}

void soft_reset(struct game *game) {
    game->score = 0;
    game->is_game_over = false;
    init_player(&game->player, game->window);
    init_pillars(&game->pillars, game->window);
}

void game_over(struct game *game) {
    if (game->score > game->highscore) {
        game->highscore = game->score;
        SaveFileData(RESOURCES "/highscore", &game->highscore, sizeof game->highscore);
    }

    game->state = STATE_MENU;
    soft_reset(game);
}

void update_main(struct game *game) {
    if (game->is_game_over) {
        game_over(game);
    }

    if (signal_pause()) {
        game->state = STATE_PAUSE;
    }

    if (!IsMusicStreamPlaying(game->music)) {
        PlayMusicStream(game->music);
    }

    update_player(&game->player, game->gravity, game->window.y, &game->is_game_over, game->dt);
    update_pillars(&game->pillars, &game->player, &game->score, &game->is_game_over, game->window.y, game->dt);
}

static inline bool signal_press(void) {
    return !!GetKeyPressed() || IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void update_menu(struct game *game) {
    if (signal_press()) {
        game->state = STATE_MAIN;
    }
}

void update_pause(struct game *game) {
    if (signal_pause()) {
        game->state = STATE_MAIN;
    }
}

void update(struct game *game) {
    game->dt = GetFrameTime();
    UpdateMusicStream(game->music);
    switch (game->state) {
    case STATE_MENU:
        update_menu(game);
        break;
    case STATE_MAIN:
        update_main(game);
        break;
    case STATE_PAUSE:
        update_pause(game);
        break;
    default:
        // Nothing
        break;
    }
}

static inline void render_player(const struct player *player) {
    DrawCircleV(player->position, player->radius, player->color);
}

static void render_pillars(const struct pillars *pillars) {
    int gap = pillars->gap / 2;
    Rectangle rec = {.width = pillars->width, .height = pillars->height};

    for (int i = 0; i < PILLARS; ++i) {
        struct pillar pillar = pillars->objects[i];
        rec.x = pillar.x;
        rec.y = pillar.y + gap;
        DrawRectangleRounded(rec, 0.4, 4, GRAY);
        rec.y = pillar.y - gap - pillars->height;
        DrawRectangleRounded(rec, 0.4, 4, GRAY);
    }
}

void render_score(unsigned short score, Vector2 window) {
    DrawText(TextFormat("%d", score), window.x / 2, 50, 80, WHITE);
}

void render_menu(const struct game *game) {
    ClearBackground(BLACK);

    DrawText("Floppy Dot!?", 110, 100, 85, WHITE);
    DrawText("Press SPACE to Play", 120, 400, 50, WHITE);
    DrawText(TextFormat("High Score: %d", game->highscore), 300, 275, 30, WHITE);
}

void render_main(const struct game *game) {
    ClearBackground(BLACK);

    render_player(&game->player);
    render_pillars(&game->pillars);
    render_score(game->score, game->window);
}

void render_pause(const struct game *game) {
    render_main(game);
    ClearBackground(CLITERAL(Color){.r = 75, .g = 75, .b = 75, .a = 75});
}

void render(const struct game *game) {
    BeginDrawing();
    switch (game->state) {
    case STATE_MENU:
        render_menu(game);
        break;
    case STATE_PAUSE:
        render_pause(game);
        break;
    case STATE_MAIN:
        render_main(game);
        break;
    default:
        // Nothing
        break;
    }
    EndDrawing();
}

void quit(struct game *game) {
    CloseWindow();
    CloseAudioDevice();
    UnloadMusicStream(game->music);
}

void update_and_render(struct game *game) {
    update(game);
    render(game);
}


int main(void) {
    struct game game;
    init(&game);

    start(&game);

    while (!WindowShouldClose()) {
	    update_and_render(&game);
    }

    quit(&game);
    return 0;
}

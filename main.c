#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define MAX_BULLETS 50
#define MAX_ENEMIES 20
#define PLAYER_SPEED 5
#define BULLET_SPEED 10
#define ENEMY_SPEED 2

typedef struct Bullet {
    Vector2 position;
    Vector2 velocity;
    bool active;
} Bullet;

typedef struct Enemy {
    Vector2 position;
    bool active;
} Enemy;

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Top-Down Shooter - raylib");

    SetTargetFPS(60);
    srand(time(NULL));

    // --- Player setup ---
    Vector2 player = { screenWidth/2, screenHeight/2 };
    float playerRadius = 20.0f;
    int score = 0;
    bool gameOver = false;

    // --- Bullet setup ---
    Bullet bullets[MAX_BULLETS] = { 0 };

    // --- Enemy setup ---
    Enemy enemies[MAX_ENEMIES] = { 0 };
    float spawnTimer = 0;

    while (!WindowShouldClose()) {
        if (!gameOver) {
            // --- Player movement ---
            if (IsKeyDown(KEY_W)) player.y -= PLAYER_SPEED;
            if (IsKeyDown(KEY_S)) player.y += PLAYER_SPEED;
            if (IsKeyDown(KEY_A)) player.x -= PLAYER_SPEED;
            if (IsKeyDown(KEY_D)) player.x += PLAYER_SPEED;

            // Keep player in bounds
            if (player.x < playerRadius) player.x = playerRadius;
            if (player.x > screenWidth - playerRadius) player.x = screenWidth - playerRadius;
            if (player.y < playerRadius) player.y = playerRadius;
            if (player.y > screenHeight - playerRadius) player.y = screenHeight - playerRadius;

            // --- Shooting bullets ---
            if (IsKeyPressed(KEY_SPACE)) {
                for (int i = 0; i < MAX_BULLETS; i++) {
                    if (!bullets[i].active) {
                        bullets[i].position = player;
                        bullets[i].velocity = (Vector2){ 0, -BULLET_SPEED };
                        bullets[i].active = true;
                        break;
                    }
                }
            }

            // --- Update bullets ---
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active) {
                    bullets[i].position.x += bullets[i].velocity.x;
                    bullets[i].position.y += bullets[i].velocity.y;

                    // Deactivate if off screen
                    if (bullets[i].position.y < 0)
                        bullets[i].active = false;
                }
            }

            // --- Spawn enemies ---
            spawnTimer += GetFrameTime();
            if (spawnTimer > 1.0f) {
                spawnTimer = 0;
                for (int i = 0; i < MAX_ENEMIES; i++) {
                    if (!enemies[i].active) {
                        enemies[i].active = true;
                        enemies[i].position = (Vector2){ rand() % screenWidth, -20 };
                        break;
                    }
                }
            }

            // --- Update enemies ---
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) {
                    // Move toward player
                    Vector2 dir = { player.x - enemies[i].position.x, player.y - enemies[i].position.y };
                    float length = sqrtf(dir.x * dir.x + dir.y * dir.y);
                    dir.x /= length; dir.y /= length;

                    enemies[i].position.x += dir.x * ENEMY_SPEED;
                    enemies[i].position.y += dir.y * ENEMY_SPEED;

                    // --- Collision with player ---
                    if (CheckCollisionCircles(player, playerRadius, enemies[i].position, 15)) {
                        gameOver = true;
                    }

                    // --- Collision with bullets ---
                    for (int j = 0; j < MAX_BULLETS; j++) {
                        if (bullets[j].active &&
                            CheckCollisionCircles(bullets[j].position, 5, enemies[i].position, 15)) {
                            bullets[j].active = false;
                            enemies[i].active = false;
                            score += 10;
                        }
                    }
                }
            }
        } else {
            if (IsKeyPressed(KEY_R)) {
                // Reset game
                player = (Vector2){ screenWidth/2, screenHeight/2 };
                score = 0;
                gameOver = false;
                for (int i = 0; i < MAX_BULLETS; i++) bullets[i].active = false;
                for (int i = 0; i < MAX_ENEMIES; i++) enemies[i].active = false;
            }
        }

        // --- Draw everything ---
        BeginDrawing();
        ClearBackground(BLACK);

        if (!gameOver) {
            // Player
            DrawCircleV(player, playerRadius, GREEN);

            // Bullets
            for (int i = 0; i < MAX_BULLETS; i++) {
                if (bullets[i].active)
                    DrawCircleV(bullets[i].position, 5, YELLOW);
            }

            // Enemies
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active)
                    DrawCircleV(enemies[i].position, 15, RED);
            }

            DrawText(TextFormat("SCORE: %d", score), 10, 10, 20, RAYWHITE);
        } else {
            DrawText("GAME OVER!", screenWidth/2 - 150, screenHeight/2 - 40, 40, RED);
            DrawText(TextFormat("FINAL SCORE: %d", score), screenWidth/2 - 120, screenHeight/2 + 20, 20, RAYWHITE);
            DrawText("Press R to Restart", screenWidth/2 - 100, screenHeight/2 + 60, 20, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

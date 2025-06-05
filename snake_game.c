#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGEUR 800
#define HAUTEUR 600
#define TAILLE_CASE 20
#define NB_CASES_X (LARGEUR / TAILLE_CASE)
#define NB_CASES_Y (HAUTEUR / TAILLE_CASE)
#define NB_FRUITS 3
#define NB_OBSTACLES 10

typedef struct {
    int x;
    int y;
} Position;

typedef enum { HAUT, BAS, GAUCHE, DROITE } Direction;
typedef enum { FACILE = 5, MOYEN = 10, DIFFICILE = 15 } Vitesse;

bool PositionEgale(Position a, Position b) {
    return a.x == b.x && a.y == b.y;
}

// Fonction pour initialiser les fruits hors zone UI
void InitialiserFruits(Position fruits[], int nb) {
    for (int i = 0; i < nb; i++) {
        do {
            fruits[i].x = rand() % NB_CASES_X;
            fruits[i].y = rand() % NB_CASES_Y;
        } while (fruits[i].y < 4); // éviter zone score/temps
    }
}

// Fonction pour initialiser obstacles hors zone UI
void InitialiserObstacles(Position obstacles[], int nb) {
    for (int i = 0; i < nb; i++) {
        do {
            obstacles[i].x = rand() % NB_CASES_X;
            obstacles[i].y = rand() % NB_CASES_Y;
        } while (obstacles[i].y < 4); // éviter zone score/temps
    }
}

int main() {
    InitWindow(LARGEUR, HAUTEUR, "Snake en C - Raylib");
    InitAudioDevice();
    SetRandomSeed(time(NULL));

    Vitesse vitesse = MOYEN;
    bool niveauChoisi = false;

    // Choix du niveau
    while (!niveauChoisi && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Choisissez le niveau:", 250, 200, 30, DARKGRAY);
        DrawText("[1] Facile", 250, 250, 25, GREEN);
        DrawText("[2] Moyen", 250, 300, 25, ORANGE);
        DrawText("[3] Difficile", 250, 350, 25, RED);
        EndDrawing();

        if (IsKeyPressed(KEY_ONE)) { vitesse = FACILE; niveauChoisi = true; }
        else if (IsKeyPressed(KEY_TWO)) { vitesse = MOYEN; niveauChoisi = true; }
        else if (IsKeyPressed(KEY_THREE)) { vitesse = DIFFICILE; niveauChoisi = true; }
    }

    SetTargetFPS(60);

    Music fond = LoadMusicStream("tayc.mp3");
    if (fond.ctxData) PlayMusicStream(fond);
    float volume = 0.5f;
    SetMusicVolume(fond, volume);

    bool rejouer = true;

    while (rejouer && !WindowShouldClose()) {
        Direction direction = DROITE;
        Direction ancienneDirection = DROITE;
        int tailleSerpent = 3;
        Position serpent[200] = { {5, 5}, {4, 5}, {3, 5} };

        Position fruits[NB_FRUITS];
        InitialiserFruits(fruits, NB_FRUITS);

        Position obstacles[NB_OBSTACLES];
        InitialiserObstacles(obstacles, NB_OBSTACLES);

        int score = 0, scoreMax = 0;
        float temps = 0;
        bool pause = false;
        bool gameOver = false;

        float intervalle = 1.0f / vitesse;
        float tempsEcoule = 0.0f;

        while (!WindowShouldClose() && !gameOver) {
            UpdateMusicStream(fond);

            // Ajustement volume avec + et -
            if (IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_EQUAL)) { // + (égal)
                volume += 0.1f;
                if (volume > 1.0f) volume = 1.0f;
                SetMusicVolume(fond, volume);
            }
            if (IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressed(KEY_MINUS)) { // -
                volume -= 0.1f;
                if (volume < 0.0f) volume = 0.0f;
                SetMusicVolume(fond, volume);
            }

            if (IsKeyPressed(KEY_SPACE)) pause = !pause;

            if (pause) {
                BeginDrawing();
                ClearBackground(RAYWHITE);
                DrawText("PAUSE", LARGEUR / 2 - 50, HAUTEUR / 2, 40, GRAY);
                DrawText("Volume: ", 10, HAUTEUR - 30, 20, DARKGRAY);
                DrawText(TextFormat("%.1f", volume), 100, HAUTEUR - 30, 20, DARKGRAY);
                EndDrawing();
                continue;
            }

            // Gestion direction chaque frame
            if (IsKeyDown(KEY_UP) && ancienneDirection != BAS) direction = HAUT;
            else if (IsKeyDown(KEY_DOWN) && ancienneDirection != HAUT) direction = BAS;
            else if (IsKeyDown(KEY_LEFT) && ancienneDirection != DROITE) direction = GAUCHE;
            else if (IsKeyDown(KEY_RIGHT) && ancienneDirection != GAUCHE) direction = DROITE;

            float delta = GetFrameTime();
            temps += delta;
            tempsEcoule += delta;

            if (tempsEcoule >= intervalle) {
                tempsEcoule = 0;

                for (int i = tailleSerpent; i > 0; i--) serpent[i] = serpent[i - 1];
                if (direction == HAUT) serpent[0].y--;
                else if (direction == BAS) serpent[0].y++;
                else if (direction == GAUCHE) serpent[0].x--;
                else if (direction == DROITE) serpent[0].x++;
                ancienneDirection = direction;

                if (serpent[0].x < 0 || serpent[0].x >= NB_CASES_X || serpent[0].y < 4 || serpent[0].y >= NB_CASES_Y)
                    gameOver = true;

                for (int i = 1; i < tailleSerpent; i++)
                    if (PositionEgale(serpent[0], serpent[i]))
                        gameOver = true;

                for (int i = 0; i < NB_OBSTACLES; i++)
                    if (PositionEgale(serpent[0], obstacles[i]))
                        gameOver = true;

                for (int i = 0; i < NB_FRUITS; i++) {
                    if (PositionEgale(serpent[0], fruits[i])) {
                        tailleSerpent++;
                        do {
                            fruits[i].x = rand() % NB_CASES_X;
                            fruits[i].y = rand() % NB_CASES_Y;
                        } while (fruits[i].y < 4);
                        score++;
                        if (score > scoreMax) scoreMax = score;
                    }
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            // Grille
            for (int x = 0; x < LARGEUR; x += TAILLE_CASE)
                DrawLine(x, 0, x, HAUTEUR, LIGHTGRAY);
            for (int y = 0; y < HAUTEUR; y += TAILLE_CASE)
                DrawLine(0, y, LARGEUR, y, LIGHTGRAY);

            // Fruits
            for (int i = 0; i < NB_FRUITS; i++) {
                DrawCircle(fruits[i].x * TAILLE_CASE + TAILLE_CASE / 2, fruits[i].y * TAILLE_CASE + TAILLE_CASE / 2, TAILLE_CASE / 2 - 2, RED);
                DrawCircle(fruits[i].x * TAILLE_CASE + TAILLE_CASE / 2 - 3, fruits[i].y * TAILLE_CASE + TAILLE_CASE / 2 - 6, 2, DARKGREEN);
            }

            // Obstacles
            for (int i = 0; i < NB_OBSTACLES; i++)
                DrawRectangle(obstacles[i].x * TAILLE_CASE, obstacles[i].y * TAILLE_CASE, TAILLE_CASE, TAILLE_CASE, BROWN);

            // Serpent
            for (int i = 0; i < tailleSerpent; i++)
                DrawRectangle(serpent[i].x * TAILLE_CASE, serpent[i].y * TAILLE_CASE, TAILLE_CASE, TAILLE_CASE, GREEN);

            // UI
            DrawText(TextFormat("Score: %d", score), 10, 10, 20, BLACK);
            DrawText(TextFormat("Temps: %.1fs", temps), 10, 35, 20, DARKGRAY);
            DrawText(TextFormat("Score Max: %d", scoreMax), 10, 60, 20, DARKBLUE);
            DrawText(TextFormat("Volume: %.1f", volume), 10, HAUTEUR - 30, 20, DARKGRAY);

            EndDrawing();
        }

        // Ecran Game Over avec options
        bool attendreChoix = true;
        while (attendreChoix && !WindowShouldClose()) {
            UpdateMusicStream(fond);

            if (IsKeyPressed(KEY_R)) {
                attendreChoix = false;  // Rejouer
            }
            if (IsKeyPressed(KEY_Q)) {
                attendreChoix = false;
                rejouer = false;  // Quitter
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("GAME OVER", LARGEUR / 2 - 100, HAUTEUR / 2 - 60, 40, RED);
            DrawText(TextFormat("Score final: %d", score), LARGEUR / 2 - 100, HAUTEUR / 2 - 10, 30, BLACK);
            DrawText("[R] Rejouer", LARGEUR / 2 - 100, HAUTEUR / 2 + 40, 25, DARKGREEN);
            DrawText("[Q] Quitter", LARGEUR / 2 - 100, HAUTEUR / 2 + 80, 25, RED);
            DrawText(TextFormat("Volume: %.1f (Utilisez + et -)", volume), LARGEUR / 2 - 100, HAUTEUR / 2 + 130, 20, DARKGRAY);
            EndDrawing();
        }
    }

    if (fond.ctxData) UnloadMusicStream(fond);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

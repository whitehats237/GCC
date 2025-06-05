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
#define MAX_TAILLE 200

typedef struct {
    int x;
    int y;
} Position;

typedef enum { HAUT, BAS, GAUCHE, DROITE } Direction;
typedef enum { FACILE = 5, MOYEN = 10, DIFFICILE = 15 } Vitesse;

bool PositionEgale(Position a, Position b) {
    return a.x == b.x && a.y == b.y;
}

void InitialiserFruits(Position fruits[], int nb) {
    for (int i = 0; i < nb; i++) {
        do {
            fruits[i].x = rand() % NB_CASES_X;
            fruits[i].y = rand() % NB_CASES_Y;
        } while (fruits[i].y < 4);
    }
}

void InitialiserObstacles(Position obstacles[], int nb) {
    for (int i = 0; i < nb; i++) {
        do {
            obstacles[i].x = rand() % NB_CASES_X;
            obstacles[i].y = rand() % NB_CASES_Y;
        } while (obstacles[i].y < 4);
    }
}

int main() {
    InitWindow(LARGEUR, HAUTEUR, "Snake en C - Raylib (1 ou 2 joueurs)");
    InitAudioDevice();
    SetRandomSeed(time(NULL));

    Vitesse vitesse = MOYEN;
    bool niveauChoisi = false;

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

    bool deuxJoueurs = false;
    bool modeChoisi = false;
    while (!modeChoisi && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Mode de jeu:", 250, 200, 30, DARKGRAY);
        DrawText("[1] Un joueur", 250, 250, 25, BLUE);
        DrawText("[2] Deux joueurs (ZQSD)", 250, 300, 25, PURPLE);
        EndDrawing();

        if (IsKeyPressed(KEY_ONE)) { modeChoisi = true; deuxJoueurs = false; }
        if (IsKeyPressed(KEY_TWO)) { modeChoisi = true; deuxJoueurs = true; }
    }

    Music fond = LoadMusicStream("tayc.mp3");
    if (fond.ctxData) PlayMusicStream(fond);
    float volume = 0.5f;
    SetMusicVolume(fond, volume);

    bool rejouer = true;
    while (rejouer && !WindowShouldClose()) {
        Direction dir1 = DROITE, ancienneDir1 = DROITE;
        Direction dir2 = GAUCHE, ancienneDir2 = GAUCHE;

        int taille1 = 3;
        Position serpent1[MAX_TAILLE] = { {5, 5}, {4, 5}, {3, 5} };

        int taille2 = 3;
        Position serpent2[MAX_TAILLE] = { {30, 5}, {31, 5}, {32, 5} };

        Position fruits[NB_FRUITS];
        InitialiserFruits(fruits, NB_FRUITS);

        Position obstacles[NB_OBSTACLES];
        InitialiserObstacles(obstacles, NB_OBSTACLES);

        int score1 = 0, score2 = 0, scoreMax1 = 0, scoreMax2 = 0;
        float temps = 0;
        bool pause = false;
        bool gameOver = false;

        float intervalle = 1.0f / vitesse;
        float tempsEcoule = 0.0f;

        while (!WindowShouldClose() && !gameOver) {
            UpdateMusicStream(fond);

            if (IsKeyPressed(KEY_KP_ADD) || IsKeyPressed(KEY_EQUAL)) {
                volume += 0.1f; if (volume > 1.0f) volume = 1.0f;
                SetMusicVolume(fond, volume);
            }
            if (IsKeyPressed(KEY_KP_SUBTRACT) || IsKeyPressed(KEY_MINUS)) {
                volume -= 0.1f; if (volume < 0.0f) volume = 0.0f;
                SetMusicVolume(fond, volume);
            }

            if (IsKeyPressed(KEY_SPACE)) pause = !pause;
            if (pause) {
                BeginDrawing(); ClearBackground(RAYWHITE);
                DrawText("PAUSE", LARGEUR/2 - 50, HAUTEUR/2, 40, GRAY);
                EndDrawing(); continue;
            }

            if (IsKeyDown(KEY_UP) && ancienneDir1 != BAS) dir1 = HAUT;
            else if (IsKeyDown(KEY_DOWN) && ancienneDir1 != HAUT) dir1 = BAS;
            else if (IsKeyDown(KEY_LEFT) && ancienneDir1 != DROITE) dir1 = GAUCHE;
            else if (IsKeyDown(KEY_RIGHT) && ancienneDir1 != GAUCHE) dir1 = DROITE;

            if (deuxJoueurs) {
                if (IsKeyDown(KEY_W) && ancienneDir2 != BAS) dir2 = HAUT;
                else if (IsKeyDown(KEY_S) && ancienneDir2 != HAUT) dir2 = BAS;
                else if (IsKeyDown(KEY_A) && ancienneDir2 != DROITE) dir2 = GAUCHE;
                else if (IsKeyDown(KEY_D) && ancienneDir2 != GAUCHE) dir2 = DROITE;
            }

            float delta = GetFrameTime();
            temps += delta; tempsEcoule += delta;

            if (tempsEcoule >= intervalle) {
                tempsEcoule = 0;

                for (int i = taille1; i > 0; i--) serpent1[i] = serpent1[i - 1];
                if (dir1 == HAUT) serpent1[0].y--;
                else if (dir1 == BAS) serpent1[0].y++;
                else if (dir1 == GAUCHE) serpent1[0].x--;
                else if (dir1 == DROITE) serpent1[0].x++;
                ancienneDir1 = dir1;

                if (deuxJoueurs) {
                    for (int i = taille2; i > 0; i--) serpent2[i] = serpent2[i - 1];
                    if (dir2 == HAUT) serpent2[0].y--;
                    else if (dir2 == BAS) serpent2[0].y++;
                    else if (dir2 == GAUCHE) serpent2[0].x--;
                    else if (dir2 == DROITE) serpent2[0].x++;
                    ancienneDir2 = dir2;
                }

                for (int i = 1; i < taille1; i++)
                    if (PositionEgale(serpent1[0], serpent1[i])) gameOver = true;
                if (serpent1[0].x < 0 || serpent1[0].x >= NB_CASES_X || serpent1[0].y < 4 || serpent1[0].y >= NB_CASES_Y) gameOver = true;

                if (deuxJoueurs) {
                    for (int i = 1; i < taille2; i++)
                        if (PositionEgale(serpent2[0], serpent2[i])) gameOver = true;
                    if (serpent2[0].x < 0 || serpent2[0].x >= NB_CASES_X || serpent2[0].y < 4 || serpent2[0].y >= NB_CASES_Y) gameOver = true;
                }

                for (int i = 0; i < NB_OBSTACLES; i++) {
                    if (PositionEgale(serpent1[0], obstacles[i]) || (deuxJoueurs && PositionEgale(serpent2[0], obstacles[i]))) {
                        gameOver = true;
                    }
                }

                for (int i = 0; i < NB_FRUITS; i++) {
                    if (PositionEgale(serpent1[0], fruits[i])) {
                        taille1++;
                        score1++;
                        if (score1 > scoreMax1) scoreMax1 = score1;
                        do {
                            fruits[i].x = rand() % NB_CASES_X;
                            fruits[i].y = rand() % NB_CASES_Y;
                        } while (fruits[i].y < 4);
                    }
                    if (deuxJoueurs && PositionEgale(serpent2[0], fruits[i])) {
                        taille2++;
                        score2++;
                        if (score2 > scoreMax2) scoreMax2 = score2;
                        do {
                            fruits[i].x = rand() % NB_CASES_X;
                            fruits[i].y = rand() % NB_CASES_Y;
                        } while (fruits[i].y < 4);
                    }
                }
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);

            for (int x = 0; x < LARGEUR; x += TAILLE_CASE)
                DrawLine(x, 0, x, HAUTEUR, LIGHTGRAY);
            for (int y = 0; y < HAUTEUR; y += TAILLE_CASE)
                DrawLine(0, y, LARGEUR, y, LIGHTGRAY);

            for (int i = 0; i < NB_FRUITS; i++)
                DrawCircle(fruits[i].x * TAILLE_CASE + TAILLE_CASE / 2, fruits[i].y * TAILLE_CASE + TAILLE_CASE / 2, TAILLE_CASE / 2 - 2, RED);

            for (int i = 0; i < NB_OBSTACLES; i++)
                DrawRectangle(obstacles[i].x * TAILLE_CASE, obstacles[i].y * TAILLE_CASE, TAILLE_CASE, TAILLE_CASE, BROWN);

            for (int i = 0; i < taille1; i++)
                DrawRectangle(serpent1[i].x * TAILLE_CASE, serpent1[i].y * TAILLE_CASE, TAILLE_CASE, TAILLE_CASE, GREEN);

            if (deuxJoueurs)
                for (int i = 0; i < taille2; i++)
                    DrawRectangle(serpent2[i].x * TAILLE_CASE, serpent2[i].y * TAILLE_CASE, TAILLE_CASE, TAILLE_CASE, PURPLE);

            DrawText(TextFormat("J1 Score: %d", score1), 10, 10, 20, BLACK);
            if (deuxJoueurs) DrawText(TextFormat("J2 Score: %d", score2), 200, 10, 20, DARKPURPLE);
            DrawText(TextFormat("Temps: %.1fs", temps), 10, 35, 20, DARKGRAY);
            DrawText(TextFormat("Volume: %.1f", volume), 10, HAUTEUR - 30, 20, DARKGRAY);

            EndDrawing();
        }

        bool attendreChoix = true;
        while (attendreChoix && !WindowShouldClose()) {
            UpdateMusicStream(fond);

            if (IsKeyPressed(KEY_R)) attendreChoix = false;
            if (IsKeyPressed(KEY_Q)) {
                attendreChoix = false;
                rejouer = false;
            }

            BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("GAME OVER", LARGEUR / 2 - 100, HAUTEUR / 2 - 60, 40, RED);
            DrawText(TextFormat("Score J1: %d", score1), LARGEUR / 2 - 100, HAUTEUR / 2 - 10, 25, BLACK);
            if (deuxJoueurs)
                DrawText(TextFormat("Score J2: %d", score2), LARGEUR / 2 - 100, HAUTEUR / 2 + 20, 25, DARKPURPLE);
            DrawText("[R] Rejouer", LARGEUR / 2 - 100, HAUTEUR / 2 + 60, 25, DARKGREEN);
            DrawText("[Q] Quitter", LARGEUR / 2 - 100, HAUTEUR / 2 + 100, 25, RED);
            EndDrawing();
        }
    }

    if (fond.ctxData) {
        StopMusicStream(fond);
        UnloadMusicStream(fond);
    }
    CloseAudioDevice();
    CloseWindow();
    return 0;
}

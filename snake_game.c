#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define LARGEUR 800
#define HAUTEUR 600
#define TAILLE_CASE 20
#define NB_CASES_X (LARGEUR / TAILLE_CASE)
#define NB_CASES_Y (HAUTEUR / TAILLE_CASE)

typedef struct {
    int x;
    int y;
} Position;

typedef enum { HAUT, BAS, GAUCHE, DROITE } Direction;

int main() {
    InitWindow(LARGEUR, HAUTEUR, "Snake en C - Raylib");
    SetTargetFPS(10); // Vitesse du jeu

    // Initialisation des variables
    Direction direction = DROITE;
    int tailleSerpent = 3;
    Position serpent[100] = {
        {5, 5},
        {4, 5},
        {3, 5}
    };

    Position fruit = {rand() % NB_CASES_X, rand() % NB_CASES_Y};
    int score = 0;

    // Pour les directions opposées
    Direction ancienneDirection = DROITE;
    while (!WindowShouldClose()) {
        // a. Gérer les touches du clavier
        if (IsKeyPressed(KEY_UP) && ancienneDirection != BAS) direction = HAUT;
        else if (IsKeyPressed(KEY_DOWN) && ancienneDirection != HAUT) direction = BAS;
        else if (IsKeyPressed(KEY_LEFT) && ancienneDirection != DROITE) direction = GAUCHE;
        else if (IsKeyPressed(KEY_RIGHT) && ancienneDirection != GAUCHE) direction = DROITE;

        // b. Déplacement du serpent
        for (int i = tailleSerpent; i > 0; i--) {
            serpent[i] = serpent[i - 1]; // Décalage du corps
        }

        // Mise à jour de la tête
        if (direction == HAUT) serpent[0].y--;
        else if (direction == BAS) serpent[0].y++;
        else if (direction == GAUCHE) serpent[0].x--;
        else if (direction == DROITE) serpent[0].x++;

        ancienneDirection = direction;

        // c. Collision mur
        if (serpent[0].x < 0 || serpent[0].x >= NB_CASES_X ||
            serpent[0].y < 0 || serpent[0].y >= NB_CASES_Y) {
            break; // Game Over
        }

        // Collision avec soi-même
        for (int i = 1; i < tailleSerpent; i++) {
            if (serpent[0].x == serpent[i].x && serpent[0].y == serpent[i].y) {
                break; // Game Over
            }
        }

        // d. Manger le fruit
        if (serpent[0].x == fruit.x && serpent[0].y == fruit.y) {
            tailleSerpent++; // Ne pas supprimer la queue
            fruit.x = rand() % NB_CASES_X;
            fruit.y = rand() % NB_CASES_Y;
            score++;
        }

        // e. Dessin
        BeginDrawing();
        ClearBackground(BLACK);

        // Dessiner le fruit
        DrawRectangle(fruit.x * TAILLE_CASE, fruit.y * TAILLE_CASE, TAILLE_CASE, TAILLE_CASE, RED);

        // Dessiner le serpent
        for (int i = 0; i < tailleSerpent; i++) {
            DrawRectangle(serpent[i].x * TAILLE_CASE, serpent[i].y * TAILLE_CASE, TAILLE_CASE, TAILLE_CASE, GREEN);
        }

        // Affichage du score
        DrawText(TextFormat("Score : %d", score), 10, 10, 20, WHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

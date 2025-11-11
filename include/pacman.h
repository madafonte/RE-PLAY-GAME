#ifndef PACMAN_H
#define PACMAN_H

#include "game.h" // Inclui nossas definições de cena

// --- Protótipos das Funções do Pac-Man ---
// (Apenas as declarações, sem { ... })

void UpdatePacman(GameScene *currentScene);
void DrawPacman(void);

#endif // PACMAN_H
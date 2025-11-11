#ifndef PACMAN_H
#define PACMAN_H

#include "raylib.h"
#include "game.h" // Precisamos disso para o tipo GameScene

// --- Funções Públicas ---
// (As funções que o main.c pode "ver" e chamar)

/**
 * @brief Atualiza a lógica do jogo Pac-Man para um único frame.
 * @param currentScene Ponteiro para a cena atual, permitindo voltar ao menu.
 */
void UpdatePacman(GameScene *currentScene);

/**
 * @brief Desenha o estado atual do jogo Pac-Man na tela.
 */
void DrawPacman(void);

#endif // PACMAN_H
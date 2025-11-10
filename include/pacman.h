#ifndef PACMAN_H
#define PACMAN_H

#include "raylib.h"
#include "game.h" // Para sabermos o que é 'GameScene'

// --- Declarações das Funções (Interface) ---

// Inicializa o jogo (Carrega mapa, aloca memória, etc.)
void InitPacman(void); 

// Atualiza a lógica do jogo
// Recebe um ponteiro para a 'currentScene' para poder
// voltar ao menu quando o jogo acabar.
void UpdatePacman(GameScene *currentScene);

// Desenha o jogo na tela
void DrawPacman(void);

// Libera toda a memória (free) usada pelo Pac-Man
void CleanupPacman(void);

#endif // PACMAN_H
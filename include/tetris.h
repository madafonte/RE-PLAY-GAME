// Evita que o arquivo seja incluído mais de uma vez,
// o que causaria erros de redefinição.
#ifndef TETRIS_H
#define TETRIS_H

// Precisamos disso para saber o que é a 'GameScene'
#include "game.h"

// --- Protótipos das Funções Públicas ---
// Estas são as 3 funções que o main.c poderá chamar.

// Função para inicializar/reiniciar o jogo Tetris
void InitTetris(void);

// Função da lógica (Update) do Tetris (chamada 60x por segundo)
void UpdateTetris(GameScene *currentScene);

// Função de desenho (Draw) do Tetris (chamada 60x por segundo)
void DrawTetris(void);

#endif // TETRIS_H
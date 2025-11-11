#ifndef PACMAN_H
#define PACMAN_H

#include "raylib.h"
#include "game.h" // Inclui o GameScene

// --- (REQUISITO: Structs) ---
// Struct para o jogador
typedef struct {
    int x, y;       // Posição na MATRIZ (não em pixels)
    int score;
    int lives;
    int direction;  // 0=Direita, 1=Esquerda, 2=Cima, 3=Baixo
} Player;

// --- (REQUISITO: Structs E Listas Encadeadas) ---
// Um "nó" da nossa lista encadeada de scores
typedef struct ScoreNode {
    char name[4]; // 3 letras (ex: "ANA") + \0
    int score;
    struct ScoreNode *next; // (REQUISITO: Ponteiro) - Ponteiro para o próximo nó
} ScoreNode;

// --- Estrutura Principal do Jogo ---
// (REQUISITO: Structs)
// Esta é a struct que a main.c vai criar e passar por ponteiro
typedef struct {
    Player player;
    // Ghost ghosts[4]; // (Futuro)

    // --- (REQUISITO: Matriz e Alocação Dinâmica) ---
    char **map;     // Ponteiro para ponteiros (será alocado dinamicamente)
    int mapWidth;
    int mapHeight;
    int remainingPills; // Para saber quando vencer

    // --- (REQUISITO: Ponteiro para Lista Encadeada) ---
    ScoreNode *highScores; // Ponteiro para o *início* (cabeça) da lista de scores

    // Estado do jogo (0=Jogando, 1=Game Over, 2=Venceu)
    int gameState; 
    
} PacmanGameState;


// --- Protótipos das Funções ---
// Estas são as funções que main.c vai chamar

// (REQUISITO: Ponteiro) - Recebe o ponteiro para carregar o estado
void InitPacman(PacmanGameState *state);

// (REQUISITO: Ponteiro) - Recebe o ponteiro para modificar o estado e a cena
void UpdatePacman(PacmanGameState *state, GameScene *currentScene); 

// (REQUISITO: Ponteiro) - Recebe o ponteiro para ler o estado e desenhar
void DrawPacman(PacmanGameState *state);

// (REQUISITO: Ponteiro) - Recebe o ponteiro para liberar a memória do estado
void UnloadPacman(PacmanGameState *state); 


// --- (REQUISITO: Leitura/Escrita de Arquivo) ---
// Funções auxiliares (internas) para carregar e salvar a lista encadeada
// Elas são chamadas por InitPacman e UpdatePacman
void LoadHighScores(PacmanGameState *state);
void SaveHighScores(PacmanGameState *state);

#endif // PACMAN_H
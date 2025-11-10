#include "pacman.h"   // O "contrato" que este arquivo implementa
#include <raylib.h>
#include "scores.h"   // Funções de high score (Load, Save, etc)
#include <stdlib.h>   // Para malloc e free
#include <string.h>   // Para memcpy
#include <math.h>

// --- Constantes da Matriz ---
#define MAP_ALTURA 21
#define MAP_LARGURA 19
#define TAMANHO_BLOCO 24 // Ajustado para caber melhor na sua tela de 960x540

// --- Estruturas (Structs) Internas ---
// (Estas structs só são "vistas" dentro deste arquivo)

typedef struct Player {
    Vector2 position;
    Vector2 speed;
    int score;
    int lives;
} Player;

typedef struct Ghost {
    Vector2 position;
    Vector2 speed;
    Color color;
} Ghost;

// Struct principal do JOGO PACMAN
typedef struct PacmanGame {
    int mapa[MAP_ALTURA][MAP_LARGURA]; // A Matriz
    Player *player;    // Ponteiro para o jogador
    Ghost *ghosts[4];  // Array de Ponteiros para fantasmas
    ScoreNode *highScores; // Ponteiro para a Lista Encadeada
    int gameState; // 0 = Jogando, 1 = Game Over
} PacmanGame;


// --- Variável de Estado Estática (Global Interna) ---
/*
 * Esta é a "mágica" que faz funcionar com o seu main.c.
 * 'game' é uma variável global, mas 'static' significa que
 * ela SÓ pode ser acessada por funções dentro deste arquivo (pacman.c).
 *
 * InitPacman() vai preenchê-la.
 * UpdatePacman() vai modificá-la.
 * DrawPacman() vai lê-la.
 * CleanupPacman() vai limpá-la.
 */
static PacmanGame game;

// --- Funções Auxiliares (só deste arquivo) ---

void InicializarMapa(int mapa[MAP_ALTURA][MAP_LARGURA]) {
    // 1 = Parede, 0 = Vazio, 2 = Pac-Dot
    int tempMap[MAP_ALTURA][MAP_LARGURA] = {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,1,2,1,2,1,2,1,2,1,1,1,2,1,2,1},
        {1,2,1,0,0,2,1,2,1,2,1,2,0,0,1,2,1,2,1}, // 0 = "casa" do fantasma
        {1,2,1,1,1,2,1,2,1,2,1,2,1,1,1,2,1,2,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,1,1,2,1,1,1,2,1,2,1,2,1,1,1,2,1,1,1},
        {1,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,1,2,1,1,1,1,1,1,1,2,1,1,1,2,1},
        {1,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,2,1},
        // ... (complete o resto das 21 linhas)
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1} 
    };
    memcpy(mapa, tempMap, sizeof(int) * MAP_ALTURA * MAP_LARGURA);
}

void DrawMap(int mapa[MAP_ALTURA][MAP_LARGURA]) {
    // (Centraliza o mapa na tela)
    int offsetX = (GetScreenWidth() - (MAP_LARGURA * TAMANHO_BLOCO)) / 2;
    int offsetY = (GetScreenHeight() - (MAP_ALTURA * TAMANHO_BLOCO)) / 2;
    
    for (int y = 0; y < MAP_ALTURA; y++) {
        for (int x = 0; x < MAP_LARGURA; x++) {
            Vector2 pos = { (float)offsetX + x * TAMANHO_BLOCO, (float)offsetY + y * TAMANHO_BLOCO };
            if (mapa[y][x] == 1) { // Parede
                DrawRectangleV(pos, (Vector2){TAMANHO_BLOCO, TAMANHO_BLOCO}, BLUE);
            } else if (mapa[y][x] == 2) { // Pac-Dot
                Vector2 center = { pos.x + TAMANHO_BLOCO / 2, pos.y + TAMANHO_BLOCO / 2 };
                DrawCircleV(center, 4, YELLOW);
            }
        }
    }
}

// --- Funções Públicas (declaradas em pacman.h) ---

void InitPacman(void) {
    // REQUISITO: Alocação Dinâmica (malloc) e Ponteiros
    game.player = (Player*) malloc(sizeof(Player));
    game.player->position = (Vector2){ (MAP_LARGURA / 2) * TAMANHO_BLOCO, (MAP_ALTURA - 5) * TAMANHO_BLOCO };
    game.player->speed = (Vector2){ 0, 0 };
    game.player->score = 0;
    game.player->lives = 3;

    for (int i = 0; i < 4; i++) {
        game.ghosts[i] = (Ghost*) malloc(sizeof(Ghost));
        game.ghosts[i]->position = (Vector2){ (MAP_LARGURA / 2) * TAMANHO_BLOCO, (MAP_ALTURA / 2) * TAMANHO_BLOCO };
    }
    game.ghosts[0]->color = RED;
    game.ghosts[1]->color = PINK;
    game.ghosts[2]->color = (Color){ 0, 228, 255, 255 }; // Cor Ciano
    game.ghosts[3]->color = ORANGE;

    // REQUISITO: Matriz
    InicializarMapa(game.mapa);
    
    // REQUISITO: Leitura de Arquivo / Lista Encadeada
    game.highScores = LoadScores("pacman_scores.txt");
    
    game.gameState = 0;
}

// Em pacman.c

// ... (todo o código anterior, InitPacman, etc.) ...

void UpdatePacman(GameScene *currentScene) {
    if (game.gameState == 1) { // Se Game Over
        if (IsKeyPressed(KEY_ENTER)) {
            *currentScene = SCENE_MENU; // Volta ao Menu
        }
        return;
    }

    // --- 1. LÓGICA DO JOGADOR (MOVIMENTO E PÍLULAS) ---
    // (Todo o código de movimento do jogador e de comer pílulas
    // que fizemos da última vez continua aqui...)
    
    // [Seu código de movimento do jogador e colisão com pílulas]
    
    // ...

    // --- 2. LÓGICA DOS FANTASMAS (IA E MOVIMENTO) ---
    float ghostSpeed = 1.5f; // Velocidade dos fantasmas (um pouco mais lentos que o jogador)

    for (int i = 0; i < 4; i++) {
        Ghost *fantasma = game.ghosts[i];
        Vector2 *playerPos = &game.player->position;
        Vector2 *ghostPos = &fantasma->position;

        Vector2 desiredMove = {0, 0};

        // 1. Achar direção para o jogador
        float dx = playerPos->x - ghostPos->x;
        float dy = playerPos->y - ghostPos->y;

        // 2. Priorizar o eixo com maior distância
        if (fabs(dx) > fabs(dy)) {
            desiredMove.x = (dx > 0) ? ghostSpeed : -ghostSpeed; // Tenta mover horizontalmente
        } else {
            desiredMove.y = (dy > 0) ? ghostSpeed : -ghostSpeed; // Tenta mover verticalmente
        }

        // 3. Checar colisão com a MATRIZ para o movimento desejado
        Vector2 nextPos = { ghostPos->x + desiredMove.x, ghostPos->y + desiredMove.y };
        
        // Converte a próxima posição para coordenadas da matriz
        int targetMapX = (int)((nextPos.x + (TAMANHO_BLOCO / 2)) / TAMANHO_BLOCO);
        int targetMapY = (int)((nextPos.y + (TAMANHO_BLOCO / 2)) / TAMANHO_BLOCO);
        
        // 4. Mover
        if (game.mapa[targetMapY][targetMapX] != 1) {
            // Caminho livre! Mover
            ghostPos->x = nextPos.x;
            ghostPos->y = nextPos.y;
        } else {
            // 5. Bloqueado! Tenta o outro eixo.
            if (desiredMove.x != 0) { // Estava tentando horizontal
                desiredMove.x = 0;
                desiredMove.y = (dy > 0) ? ghostSpeed : -ghostSpeed; // Tenta vertical
            } else { // Estava tentando vertical
                desiredMove.y = 0;
                desiredMove.x = (dx > 0) ? ghostSpeed : -ghostSpeed; // Tenta horizontal
            }
            
            // Recalcula o alvo e checa de novo
            nextPos = (Vector2){ ghostPos->x + desiredMove.x, ghostPos->y + desiredMove.y };
            targetMapX = (int)((nextPos.x + (TAMANHO_BLOCO / 2)) / TAMANHO_BLOCO);
            targetMapY = (int)((nextPos.y + (TAMANHO_BLOCO / 2)) / TAMANHO_BLOCO);

            if (game.mapa[targetMapY][targetMapX] != 1) {
                // Agora sim, caminho livre no outro eixo
                ghostPos->x = nextPos.x;
                ghostPos->y = nextPos.y;
            }
            // (Se bloqueou de novo, o fantasma fica parado 1 frame)
        }
    }


    // --- 3. COLISÃO FANTASMA-JOGADOR ---
    
    // (Usamos retângulos para a colisão. Ajustamos o tamanho para ser "justo")
    Rectangle playerRect = { 
        game.player->position.x + 2, 
        game.player->position.y + 2, 
        TAMANHO_BLOCO - 4, 
        TAMANHO_BLOCO - 4 
    };

    for (int i = 0; i < 4; i++) {
        Rectangle ghostRect = { 
            game.ghosts[i]->position.x + 2, 
            game.ghosts[i]->position.y + 2, 
            TAMANHO_BLOCO - 4, 
            TAMANHO_BLOCO - 4 
        };

        if (CheckCollisionRecs(playerRect, ghostRect)) {
            // COLIDIU! Perde uma vida
            game.player->lives--;

            // Reinicia a posição do jogador e dos fantasmas
            game.player->position = (Vector2){ (MAP_LARGURA / 2) * TAMANHO_BLOCO, (MAP_ALTURA - 5) * TAMANHO_BLOCO };
            for (int j = 0; j < 4; j++) {
                game.ghosts[j]->position = (Vector2){ (MAP_LARGURA / 2) * TAMANHO_BLOCO, (MAP_ALTURA / 2) * TAMANHO_BLOCO };
            }

            // Checa se é Game Over
            if (game.player->lives <= 0 && game.gameState != 1) {
                game.gameState = 1;
                AddScore(&(game.highScores), game.player->score);
                SaveScores("pacman_scores.txt", game.highScores);
            }
            
            break; // Sai do loop 'for', pois o jogador já foi pego
        }
    }

    // --- Lógica de Game Over (de teste) ---
    // (Pode remover a tecla Q agora)
    // if (IsKeyPressed(KEY_Q)) game.player->lives = 0; 
}


// Em src/pacman.c (cole isso no final do arquivo)

/*
 * Desenha o estado atual do jogo na tela
 */
void DrawPacman(void) {
    // (Centraliza o mapa)
    int offsetX = (GetScreenWidth() - (MAP_LARGURA * TAMANHO_BLOCO)) / 2;
    int offsetY = (GetScreenHeight() - (MAP_ALTURA * TAMANHO_BLOCO)) / 2;

    // REQUISITO: Matriz (desenha o mapa)
    DrawMap(game.mapa);

    // REQUISITO: Ponteiros (desenha o jogador)
    Vector2 playerPos = { offsetX + game.player->position.x, offsetY + game.player->position.y };
    DrawCircleV(playerPos, TAMANHO_BLOCO / 2 - 2, YELLOW);
    
    // Desenha Fantasmas
    for (int i = 0; i < 4; i++) {
        Vector2 ghostPos = { offsetX + game.ghosts[i]->position.x, offsetY + game.ghosts[i]->position.y };
        DrawRectangleV(ghostPos, (Vector2){TAMANHO_BLOCO, TAMANHO_BLOCO}, game.ghosts[i]->color);
    }
    
    // Desenha UI (Score, Vidas)
    DrawText(TextFormat("Score: %d", game.player->score), 20, 10, 20, WHITE);
    DrawText(TextFormat("Vidas: %d", game.player->lives), GetScreenWidth() - 100, 10, 20, WHITE);
    
    // Desenha High Scores (da Lista Encadeada)
    DrawScores(game.highScores, 20, 40);

    if (game.gameState == 1) {
        // Corrigido (sem FADE): Desenha um retângulo preto com 50% de transparência
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){ 0, 0, 0, 127 });
        DrawText("GAME OVER", 300, 250, 40, RED);
        DrawText("Pressione ENTER para voltar ao Menu", 280, 300, 20, WHITE);
    }
}

/*
 * Libera toda a memória usada pelo jogo Pac-Man
 */
void CleanupPacman(void) {
    
    // REQUISITO: Alocação Dinâmica (free)
    
    // 1. Libera a memória do jogador
    free(game.player);
    game.player = NULL;

    // 2. Libera a memória de cada fantasma
    for (int i = 0; i < 4; i++) {
        free(game.ghosts[i]);
        game.ghosts[i] = NULL;
    }

    // REQUISITOS: Lista Encadeada (free)
    // 3. Libera toda a memória da lista de scores
    FreeScoreList(game.highScores);
    game.highScores = NULL;
}
#include "pacman.h"
#include <stdio.h>  // Para FILE* (leitura/escrita)
#include <stdlib.h> // Para malloc/free (alocação)
#include <string.h> // Para strcpy (scores)

// --- Constantes Globais ---
// (REQUISITO: Leitura/Escrita de Arquivo)
#define MAP_FILE "assets/pacman_map.txt"
#define SCORES_FILE "assets/scores.txt"

// Tamanho em pixels de cada "bloco" do mapa
#define TILE_SIZE 16 // Ajustado para caber na tela de 960x540

// --- Funções Auxiliares (Protótipos) ---
// (Estas funções são "internas" de pacman.c)
static void AllocateMapMemory(PacmanGameState *state);
static void LoadMapFromFile(PacmanGameState *state);
static void FreeMapMemory(PacmanGameState *state);
static void FreeHighScoresList(PacmanGameState *state);
static void AddScoreToList(PacmanGameState *state, const char *name, int score);

// ===================================================================
// REQUISITO: Leitura de Arquivo + Lista Encadeada + Alocação Dinâmica
// ===================================================================

void LoadHighScores(PacmanGameState *state) {
    state->highScores = NULL; // Começa com a lista vazia
    
    FILE *file = fopen(SCORES_FILE, "r"); // "r" = read (leitura)
    if (file == NULL) {
        perror("Aviso: Nao foi possivel abrir scores.txt (sera criado no fim)");
        return;
    }

    char lineBuffer[100];
    if (fgets(lineBuffer, sizeof(lineBuffer), file) == NULL) {
        fclose(file);
        return; // Arquivo vazio
    }

    char name[4];
    int score;
    while (fscanf(file, "%3s %d\n", name, &score) == 2) {
        // (REQUISITO: Alocação Dinâmica)
        ScoreNode *newNode = (ScoreNode *)malloc(sizeof(ScoreNode));
        if (newNode == NULL) {
            perror("Falha ao alocar memoria para ScoreNode");
            continue;
        }

        strcpy(newNode->name, name);
        newNode->score = score;
        
        // (REQUISITOS: Ponteiros + Lista Encadeada)
        // Adiciona o novo nó no *início* da lista
        newNode->next = state->highScores;
        state->highScores = newNode;
    }

    fclose(file);
}

// ===================================================================
// REQUISITO: Escrita em Arquivo + Lista Encadeada
// ===================================================================

void SaveHighScores(PacmanGameState *state) {
    FILE *file = fopen(SCORES_FILE, "w"); // "w" = write (escrita)
    if (file == NULL) {
        perror("Erro ao salvar scores.txt");
        return;
    }

    fprintf(file, "TOP SCORES\n"); 

    // (REQUISITO: Ponteiros + Lista Encadeada)
    ScoreNode *current = state->highScores;
    while (current != NULL) {
        fprintf(file, "%s %d\n", current->name, current->score);
        current = current->next; 
    }

    fclose(file);
}

// ===================================================================
// REQUISITO: Alocação Dinâmica + Matrizes
// ===================================================================

static void AllocateMapMemory(PacmanGameState *state) {
    // Definimos o tamanho do mapa aqui
    state->mapWidth = 28;
    state->mapHeight = 31;

    // (REQUISITO: Alocação Dinâmica de Matriz)
    // 1. Aloca um array de ponteiros (linhas)
    state->map = (char **)malloc(state->mapHeight * sizeof(char *));
    if (state->map == NULL) {
        perror("Falha ao alocar linhas do mapa");
        exit(1); 
    }

    // 2. Aloca um array de chars para cada linha (colunas)
    for (int i = 0; i < state->mapHeight; i++) {
        state->map[i] = (char *)malloc(state->mapWidth * sizeof(char));
        if (state->map[i] == NULL) {
            perror("Falha ao alocar colunas do mapa");
            exit(1); 
        }
    }
}

static void LoadMapFromFile(PacmanGameState *state) {
    FILE *file = fopen(MAP_FILE, "r");
    if (file == NULL) {
        perror("Erro ao abrir pacman_map.txt");
        exit(1); 
    }

    state->remainingPills = 0; // Zera a contagem de pílulas

    for (int y = 0; y < state->mapHeight; y++) {
        for (int x = 0; x < state->mapWidth; x++) {
            char c = fgetc(file);
            
            if (c == '\n') {
                x--; 
                continue;
            }

            state->map[y][x] = c; // (REQUISITO: Matriz)

            if (c == 'P') {
                state->player.x = x;
                state->player.y = y;
                state->map[y][x] = ' '; 
            }
            else if (c == '.') {
                state->remainingPills++; // Conta as pílulas
            }
        }
    }

    fclose(file);
}

// ===================================================================
// FUNÇÕES PRINCIPAIS DO JOGO (Chamadas pela main.c)
// ===================================================================

void InitPacman(PacmanGameState *state) {
    // (REQUISITO: Ponteiro) - 'state' é o ponteiro para a struct da main
    
    // 1. Aloca e Carrega o Mapa
    AllocateMapMemory(state); // (Alocação Dinâmica)
    LoadMapFromFile(state);   // (Leitura de Arquivo, Matriz)

    // 2. Carrega os Scores
    LoadHighScores(state);    // (Lista Encadeada, Alocação, Leitura)

    // 3. Inicializa o estado do jogador
    state->player.score = 0;
    state->player.lives = 3;
    state->player.direction = 0; 
    state->gameState = 0; // 0=Jogando
}

void UpdatePacman(PacmanGameState *state, GameScene *currentScene) {
    // (REQUISITO: Ponteiro) - 'state' e 'currentScene' são ponteiros
    
    // Lógica de Input (placeholder)
    if (IsKeyPressed(KEY_RIGHT)) state->player.direction = 0;
    if (IsKeyPressed(KEY_LEFT)) state->player.direction = 1;
    if (IsKeyPressed(KEY_UP)) state->player.direction = 2;
    if (IsKeyPressed(KEY_DOWN)) state->player.direction = 3;
    
    // Lógica de Movimento (placeholder simples)
    // (Uma lógica real checaria colisão com 'W' na matriz state->map)
    // ...
    
    // Lógica de Coleta de Pílula (placeholder)
    // (REQUISITO: Matriz)
    if (state->map[state->player.y][state->player.x] == '.') {
        state->map[state->player.y][state->player.x] = ' '; // Comeu a pílula
        state->player.score += 10;
        state->remainingPills--;
    }

    // Lógica de Vitória
    if (state->remainingPills == 0) {
        state->gameState = 2; // Venceu
    }
    
    // Lógica de voltar ao Menu (ex: Game Over ou Venceu)
    if (IsKeyPressed(KEY_ENTER) || state->gameState == 2) { 
        state->player.score += 150; // Dá uns pontos de teste
        
        // Adiciona o score na lista
        AddScoreToList(state, "NEW", state->player.score);
        
        // (REQUISITO: Escrita em Arquivo)
        SaveHighScores(state);
        
        // Muda a cena de volta para o Menu
        *currentScene = SCENE_MENU; 
    }
}

void DrawPacman(PacmanGameState *state) {
    // (REQUISITO: Ponteiro) - 'state' é o ponteiro para ler os dados
    
    // O BeginDrawing/EndDrawing está na main.c!
    // Não os coloque aqui.

    // Centraliza o mapa na tela
    int offsetX = (GetScreenWidth() - (state->mapWidth * TILE_SIZE)) / 2;
    int offsetY = (GetScreenHeight() - (state->mapHeight * TILE_SIZE)) / 2;


    // (REQUISITO: Matriz)
    // Desenha o mapa lendo a matriz
    for (int y = 0; y < state->mapHeight; y++) {
        for (int x = 0; x < state->mapWidth; x++) {
            char tile = state->map[y][x];
            
            if (tile == 'W') { // Parede
                DrawRectangle(offsetX + x * TILE_SIZE, offsetY + y * TILE_SIZE, TILE_SIZE, TILE_SIZE, BLUE);
            } 
            else if (tile == '.') { // Pílula
                DrawCircle(offsetX + x * TILE_SIZE + TILE_SIZE / 2, offsetY + y * TILE_SIZE + TILE_SIZE / 2, 2, WHITE);
            }
        }
    }

    // (REQUISITO: Struct)
    // Desenha o jogador
    DrawCircle(offsetX + state->player.x * TILE_SIZE + TILE_SIZE / 2, 
               offsetY + state->player.y * TILE_SIZE + TILE_SIZE / 2, 
               TILE_SIZE / 2 - 1, YELLOW);

    // Desenha UI
    DrawText(TextFormat("SCORE: %d", state->player.score), 10, 10, 20, WHITE);
    DrawText(TextFormat("LIVES: %d", state->player.lives), GetScreenWidth() - 100, 10, 20, WHITE);
    
    if (state->gameState == 2) {
        DrawText("VOCE VENCEU!", 300, 200, 40, GREEN);
    }
    DrawText("Pressione ENTER para sair", 10, GetScreenHeight() - 30, 20, DARKGRAY);

    // (REQUISITO: Lista Encadeada)
    // Desenha os High Scores lidos da lista
    DrawText("High Scores:", GetScreenWidth() - 150, 40, 20, GOLD);
    ScoreNode *current = state->highScores;
    int y_pos = 60;
    while (current != NULL && y_pos < 200) { // Limita para não estourar a tela
        DrawText(TextFormat("%s - %d", current->name, current->score), GetScreenWidth() - 150, y_pos, 20, GOLD);
        y_pos += 20;
        current = current->next; // (REQUISITO: Ponteiro)
    }
}

// ===================================================================
// REQUISITO: Alocação Dinâmica (Liberação)
// ===================================================================

static void FreeMapMemory(PacmanGameState *state) {
    if (state->map == NULL) return;
    for (int i = 0; i < state->mapHeight; i++) {
        free(state->map[i]);
    }
    free(state->map);
    state->map = NULL;
}

static void FreeHighScoresList(PacmanGameState *state) {
    ScoreNode *current = state->highScores;
    ScoreNode *next;
    while (current != NULL) {
        next = current->next; 
        free(current);        
        current = next;       
    }
    state->highScores = NULL;
}

// Função principal de liberação (Chamada pela main.c)
void UnloadPacman(PacmanGameState *state) {
    // (REQUISITO: Ponteiro)
    FreeMapMemory(state);       // Libera o mapa
    FreeHighScoresList(state);  // Libera a lista
}

// Função auxiliar para adicionar um score (usada no Update)
static void AddScoreToList(PacmanGameState *state, const char *name, int score) {
    // (REQUISITO: Alocação Dinâmica)
    ScoreNode *newNode = (ScoreNode *)malloc(sizeof(ScoreNode));
    if (newNode == NULL) return; 

    strcpy(newNode->name, name);
    newNode->score = score;
    
    // (REQUISITO: Ponteiros + Lista Encadeada)
    newNode->next = state->highScores;
    state->highScores = newNode;
    // (Uma lógica real ordenaria a lista antes de salvar)
}
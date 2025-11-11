#include "pacman.h" // Inclui o nosso próprio header
#include "raylib.h"
#include "game.h"   // Para SCENE_MENU
#include <math.h>   // Para a função fabs()

// --- Constantes do Jogo ---
#define MAP_ROWS 21
#define MAP_COLS 21
#define TILE_SIZE 24
#define PLAYER_SPEED 3
#define GHOST_SPEED 1

// Cores
#define WALL_COLOR BLUE
#define PELLET_COLOR YELLOW
#define POWERUP_COLOR WHITE
#define PLAYER_COLOR YELLOW
#define GHOST_COLOR_1 RED
#define GHOST_COLOR_2 PINK

// --- Estruturas de Dados ---

// Estado do jogo (jogando, ganho, perdido)
typedef enum {
    STATE_PLAYING,
    STATE_WIN,
    STATE_GAME_OVER
} PacmanState;

// Direções
typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_NONE
} Direction;

// Estrutura para o jogador
typedef struct {
    Vector2 position;
    Vector2 speed;
    Direction direction;       // Direção atual
    Direction nextDirection;   // Próxima direção (buffer de input)
    int lives;
    int score;
} Player;

// Estrutura para o fantasma
typedef struct {
    Vector2 position;
// vvv ADICIONE ESTA LINHA vvv
    Direction direction; // Direção atual do fantasma
    Vector2 speed;
    Color color;
    bool isScared; // Fantasma está com medo (depois do power-up)
} Ghost;


// --- Variáveis Estáticas (Globais do Pac-Man) ---
// Usamos 'static' para que estas variáveis só existam dentro deste arquivo

static bool isInitialized = false;   // Flag para saber se o jogo foi inicializado
static PacmanState gameState;        // Estado atual do jogo (jogando, etc.)
static int screenWidth = 0;
static int screenHeight = 0;

// Mapa do jogo
// # = Parede
// . = Bolinha (Pellet)
// O = Super-Bolinha (Power-up)
// P = Posição inicial do Player
// G = Posição inicial do Fantasma
// ' ' = Espaço vazio
static char map[MAP_ROWS][MAP_COLS + 1] = {
    "#####################",
    "#P..#...........#..O#",
    "#.##.#.#####.#.##.#",
    "#...#...#...#...#...#",
    "#.#####.#.#.#####.#",
    "#.......#.#.......#",
    "#####.#.#.#.#.#####",
    "    #.#.G.#.#.#    ",
    "#####.#.#.#.#.#####",
    "#.......#.#.......#",
    "#.#####.#.#.#####.#",
    "#...#...#...#...#...#",
    "#.##.#.#####.#.##.#",
    "#O..#...........#..G#",
    "#####################",
    ".....................", // Linhas extras para preenchimento
    ".....................",
    ".....................",
    ".....................",
    ".....................",
    "....................."
};
// Nota: O mapa acima está incompleto, é apenas um exemplo.
// Vamos usar um mapa padrão mais simples para a lógica de inicialização.

static char defaultMap[MAP_ROWS][MAP_COLS + 1] = {
    "#####################",
    "#P..................#",
    "#.###.#####.#####.#",
    "#O# #.....#.....# #O#",
    "#.###.#####.#####.#",
    "#...................#",
    "#.###.#.#####.#.###.#",
    "#...#.#...#...#.#...#",
    "###.### G ###.###.###",
    "  #.#   #G#   #.#  ",
    "###.### ### ###.###",
    "#...#.....#.....#...#",
    "#.###.#####.#####.#",
    "#O..................O#",
    "#.###.###.###.###.#",
    "#...#...#...#...#...#",
    "###.###.###.###.###",
    "#...................#",
    "#####################",
    "                     ",
    "                     "
};

static Player player;
static Ghost ghosts[2];
static int pelletsCount = 0;
static bool powerUpActive = false;
static int powerUpTimer = 0;

// Offsets para centralizar o mapa na tela
static int offsetX = 0;
static int offsetY = 0;

// --- Protótipos de Funções Internas ---
static void InitPacman(void);       // Inicializa/Reseta o jogo
static void UpdateGame(GameScene *currentScene); // Atualiza a lógica de jogo
static void DrawGame(void);         // Desenha o jogo (paredes, pellets, etc.)
static void DrawUI(void);           // Desenha a pontuação, vidas, etc.
// vvv REMOVA ESTES DOIS PROTÓTIPOS vvv
// static void SnapGhostToGrid(Ghost *ghost); // -> (A função será mantida, mas o protótipo não é necessário se ela for movida)
// static bool TestGhostMove(Ghost *ghost, Vector2 speed);
// ^^^ FIM DA REMOÇÃO ^^^

// --- Implementação das Funções Públicas ---

void UpdatePacman(GameScene *currentScene)
{
    // Inicializa o jogo na primeira vez que entramos nesta cena
    if (!isInitialized) {
        InitPacman();
    }

    // Lógica de atualização principal
    if (gameState == STATE_PLAYING) {
        UpdateGame(currentScene);
    }

    // Lógica para estados de Fim de Jogo
    if (gameState == STATE_GAME_OVER || gameState == STATE_WIN) {
        if (IsKeyPressed(KEY_ENTER)) {
            InitPacman(); // Reinicia o jogo
        }
        if (IsKeyPressed(KEY_Q)) {
            *currentScene = SCENE_MENU; // Volta ao menu
            isInitialized = false;     // Reseta o flag para a próxima vez
        }
    }
}

void DrawPacman(void)
{
    // Não desenha nada se não estiver inicializado (evita bugs)
    if (!isInitialized) return;

    ClearBackground(BLACK);

    // Desenha o jogo (mapa, pellets, personagens)
    DrawGame();

    // Desenha a interface (pontuação, vidas)
    DrawUI();

    // Mensagens de Fim de Jogo
    if (gameState == STATE_GAME_OVER) {
        DrawText("GAME OVER", (screenWidth / 2) - MeasureText("GAME OVER", 40) / 2, screenHeight / 2 - 40, 40, RED);
        DrawText("Pressione [ENTER] para reiniciar", (screenWidth / 2) - MeasureText("Pressione [ENTER] para reiniciar", 20) / 2, screenHeight / 2 + 10, 20, WHITE);
        DrawText("Pressione [Q] para sair", (screenWidth / 2) - MeasureText("Pressione [Q] para sair", 20) / 2, screenHeight / 2 + 40, 20, WHITE);
    } else if (gameState == STATE_WIN) {
        DrawText("VOCÊ VENCEU!", (screenWidth / 2) - MeasureText("VOCÊ VENCEU!", 40) / 2, screenHeight / 2 - 40, 40, GREEN);
        DrawText("Pressione [ENTER] para reiniciar", (screenWidth / 2) - MeasureText("Pressione [ENTER] para reiniciar", 20) / 2, screenHeight / 2 + 10, 20, WHITE);
        DrawText("Pressione [Q] para sair", (screenWidth / 2) - MeasureText("Pressione [Q] para sair", 20) / 2, screenHeight / 2 + 40, 20, WHITE);
    }
}


// --- Implementação das Funções Internas ---

static void InitPacman(void)
{
    // Pega o tamanho da tela (do main.c)
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();

    // Calcula os offsets para centralizar o mapa
    offsetX = (screenWidth - (MAP_COLS * TILE_SIZE)) / 2;
    offsetY = (screenHeight - (MAP_ROWS * TILE_SIZE)) / 2;

    gameState = STATE_PLAYING;
    pelletsCount = 0;
    powerUpActive = false;
    powerUpTimer = 0;

    // Configura o jogador
    player.lives = 3;
    player.score = 0;
    player.direction = DIR_NONE;
    player.nextDirection = DIR_NONE;

    // Reseta o mapa e encontra posições iniciais
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            // Copia o mapa padrão
            map[y][x] = defaultMap[y][x];

            // Encontra a posição inicial do jogador
            if (map[y][x] == 'P') {
                player.position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                map[y][x] = ' '; // Limpa a posição inicial do jogador
            }
            // Encontra a posição inicial dos fantasmas
            else if (map[y][x] == 'G') {
                if (ghosts[0].position.x == 0) { // Primeiro fantasma
                    ghosts[0].position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                    ghosts[0].color = GHOST_COLOR_1;
                    ghosts[0].isScared = false;
// vvv MODIFIQUE ESTA LINHA vvv
                    ghosts[0].direction = DIR_UP; // Começa movendo PARA CIMA
                } else { // Segundo fantasma
                    ghosts[1].position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                    ghosts[1].color = GHOST_COLOR_2;
                    ghosts[1].isScared = false;
// vvv MODIFIQUE ESTA LINHA vvv
                    ghosts[1].direction = DIR_UP; // Começa movendo PARA CIMA
                }
                map[y][x] = ' '; // Limpa a posição inicial
            }
            // Conta as bolinhas
            else if (map[y][x] == '.') {
                pelletsCount++;
            }
            else if (map[y][x] == 'O') {
                pelletsCount++; // Power-ups também contam para vencer
            }
        }
    }
    
    isInitialized = true;
}

// Verifica se a célula do mapa [row, col] é uma parede
static bool IsWall(int row, int col) {
    // Verifica limites do mapa
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) {
        return true; // Fora dos limites é considerado parede
    }
    // Verifica o caractere do mapa
    return (map[row][col] == '#');
}

// Converte posição da tela (pixels) para índice do mapa (grid)
static int GetMapCol(float posX) {
    return (int)((posX - offsetX) / TILE_SIZE);
}

static int GetMapRow(float posY) {
    return (int)((posY - offsetY) / TILE_SIZE);
}

// vvv ESTA FUNÇÃO CONTINUA, MAS O PROTÓTIPO FOI REMOVIDO vvv
/**
 * @brief Centraliza o fantasma no "tile" (célula) atual do grid.
 * Isso previne o "drifting" que causa o bug de atravessar paredes.
 */
static void SnapGhostToGrid(Ghost *ghost)
{
    int currentRow = GetMapRow(ghost->position.y);
    int currentCol = GetMapCol(ghost->position.x);
    ghost->position = (Vector2){
        (float)currentCol * TILE_SIZE + offsetX + TILE_SIZE / 2,
        (float)currentRow * TILE_SIZE + offsetY + TILE_SIZE / 2
    };
}

// vvv REMOVA A DEFINIÇÃO INTEIRA DA FUNÇÃO TestGhostMove vvv
// static bool TestGhostMove(Ghost *ghost, Vector2 speed)
// {
//    ... (todo o código da função) ...
// }
// ^^^ FIM DA REMOÇÃO ^^^


// Lógica de atualização principal
static void UpdateGame(GameScene *currentScene)
{
    // --- Voltar ao Menu ---
    if (IsKeyPressed(KEY_Q)) {
        *currentScene = SCENE_MENU; // Define a cena para o menu
        isInitialized = false;     // Garante que o jogo reinicie na próxima vez
        return;                    // Sai da função de atualização
    }

    // --- Input do Jogador ---
    if (IsKeyDown(KEY_RIGHT)) player.nextDirection = DIR_RIGHT;
    else if (IsKeyDown(KEY_LEFT)) player.nextDirection = DIR_LEFT;
    else if (IsKeyDown(KEY_UP)) player.nextDirection = DIR_UP;
    else if (IsKeyDown(KEY_DOWN)) player.nextDirection = DIR_DOWN;

    // --- Lógica de Movimento (Baseado em Grid) ---
    // Pega a posição atual do jogador no grid
    int playerRow = GetMapRow(player.position.y);
    int playerCol = GetMapCol(player.position.x);
    
    // A variável 'moved' foi removida daqui pois não estava sendo usada

    // Tenta aplicar a "próxima direção" (input em buffer)
    // Isso permite que o jogador "vire a esquina"
    if (player.nextDirection != DIR_NONE) {
        int nextRow = playerRow;
        int nextCol = playerCol;
        if (player.nextDirection == DIR_UP) nextRow--;
        if (player.nextDirection == DIR_DOWN) nextRow++;
        if (player.nextDirection == DIR_LEFT) nextCol--;
        if (player.nextDirection == DIR_RIGHT) nextCol++;
        
        if (!IsWall(nextRow, nextCol)) {
            player.direction = player.nextDirection;
            player.nextDirection = DIR_NONE;
        }
    }
    
    // Move o jogador na direção atual
    int nextRow = playerRow;
    int nextCol = playerCol;
    if (player.direction == DIR_UP) {
        nextRow--;
        player.speed = (Vector2){0, -PLAYER_SPEED};
    } else if (player.direction == DIR_DOWN) {
        nextRow++;
        player.speed = (Vector2){0, PLAYER_SPEED};
    } else if (player.direction == DIR_LEFT) {
        nextCol--;
        player.speed = (Vector2){-PLAYER_SPEED, 0};
    } else if (player.direction == DIR_RIGHT) {
        nextCol++;
        player.speed = (Vector2){PLAYER_SPEED, 0};
    } else {
        player.speed = (Vector2){0, 0};
    }

    // Verifica se o próximo "tile" é uma parede
    // Usamos uma pequena "sondagem" à frente para parar antes de bater
    Vector2 nextPos = (Vector2){ player.position.x + player.speed.x, player.position.y + player.speed.y };
    int nextPosRow = GetMapRow(nextPos.y + (player.speed.y > 0 ? TILE_SIZE / 2 - 1 : -TILE_SIZE / 2 + 1));
    int nextPosCol = GetMapCol(nextPos.x + (player.speed.x > 0 ? TILE_SIZE / 2 - 1 : -TILE_SIZE / 2 + 1));
    
    if (!IsWall(nextPosRow, nextPosCol)) {
        player.position = nextPos;
    } else {
        // Se bater, centraliza o jogador no tile atual
        player.position = (Vector2){(float)playerCol * TILE_SIZE + offsetX + TILE_SIZE / 2, (float)playerRow * TILE_SIZE + offsetY + TILE_SIZE / 2};
        player.direction = DIR_NONE; // Para de se mover
    }
    
    // Lógica de "Wrap" (atravessar para o outro lado)
    // (Implementação simplificada: apenas verifica os limites)
    if (player.position.x < offsetX) player.position.x = offsetX + (MAP_COLS * TILE_SIZE);
    if (player.position.x > offsetX + (MAP_COLS * TILE_SIZE)) player.position.x = offsetX;


    // --- Lógica de Coleta ---
    int playerMapRow = GetMapRow(player.position.y);
    int playerMapCol = GetMapCol(player.position.x);
    
    char *cell = &map[playerMapRow][playerMapCol];

    if (*cell == '.') { // Coleta bolinha
        *cell = ' ';
        player.score += 10;
        pelletsCount--;
    }
    else if (*cell == 'O') { // Coleta super-bolinha
        *cell = ' ';
        player.score += 50;
        pelletsCount--;
        powerUpActive = true;
        powerUpTimer = 60 * 7; // 7 segundos de power-up (60 FPS * 7)
        ghosts[0].isScared = true;
        ghosts[1].isScared = true;
    }

    // --- Atualiza Timer do Power-up ---
    if (powerUpActive) {
        powerUpTimer--;
        if (powerUpTimer <= 0) {
            powerUpActive = false;
            ghosts[0].isScared = false;
            ghosts[1].isScared = false;
        }
    }

    // vvv SUBSTITUA TODO O BLOCO DE MOVIMENTO DO FANTASMA POR ESTE vvv
    // --- Movimento dos Fantasmas (IA com Lógica de Decisão) ---
    for (int i = 0; i < 2; i++) {
        
        // 1. Define a velocidade com base na direção atual
        switch (ghosts[i].direction) {
            case DIR_UP: ghosts[i].speed = (Vector2){0, -GHOST_SPEED}; break;
            case DIR_DOWN: ghosts[i].speed = (Vector2){0, GHOST_SPEED}; break;
            case DIR_LEFT: ghosts[i].speed = (Vector2){-GHOST_SPEED, 0}; break;
            case DIR_RIGHT: ghosts[i].speed = (Vector2){GHOST_SPEED, 0}; break;
            case DIR_NONE: ghosts[i].speed = (Vector2){0, 0}; break;
        }

        // 2. Testa a próxima posição (usando a mesma lógica de "sondagem" do jogador)
        Vector2 nextPos = (Vector2){ ghosts[i].position.x + ghosts[i].speed.x, ghosts[i].position.y + ghosts[i].speed.y };
        int nextPosRow = GetMapRow(nextPos.y + (ghosts[i].speed.y > 0 ? TILE_SIZE / 2 - 1 : -TILE_SIZE / 2 + 1));
        int nextPosCol = GetMapCol(nextPos.x + (ghosts[i].speed.x > 0 ? TILE_SIZE / 2 - 1 : -TILE_SIZE / 2 + 1));

        // 3. Verifica se a próxima posição é uma parede
        if (!IsWall(nextPosRow, nextPosCol)) {
            // NÃO É PAREDE: Continua se movendo
            ghosts[i].position = nextPos;
        } else {
            // É PAREDE: Para, centraliza, e TOMA UMA DECISÃO
            SnapGhostToGrid(&ghosts[i]); // Centraliza para evitar "drifting"
            
            // IA: Encontrar uma nova direção
            
            // Direção oposta à atual (para evitar voltar)
            Direction oppositeDir = DIR_NONE;
            if (ghosts[i].direction == DIR_UP) oppositeDir = DIR_DOWN;
            if (ghosts[i].direction == DIR_DOWN) oppositeDir = DIR_UP;
            if (ghosts[i].direction == DIR_LEFT) oppositeDir = DIR_RIGHT;
            if (ghosts[i].direction == DIR_RIGHT) oppositeDir = DIR_LEFT;

            // Posição atual do grid
            int ghostRow = GetMapRow(ghosts[i].position.y);
            int ghostCol = GetMapCol(ghosts[i].position.x);

            // Verifica as 4 direções
            bool canMoveUp = !IsWall(ghostRow - 1, ghostCol);
            bool canMoveDown = !IsWall(ghostRow + 1, ghostCol);
            bool canMoveLeft = !IsWall(ghostRow, ghostCol - 1);
            bool canMoveRight = !IsWall(ghostRow, ghostCol + 1);

            Direction bestDir = DIR_NONE;
            float bestDirScore = -INFINITY; // Queremos maximizar isso

            // Calcula a direção preferida (para o jogador)
            Vector2 directionToPlayer = (Vector2){ player.position.x - ghosts[i].position.x, player.position.y - ghosts[i].position.y };

            // Avalia cada direção *válida*
            if (canMoveUp && oppositeDir != DIR_UP) {
                float score = -directionToPlayer.y; // Pontuação alta se o jogador estiver acima
                if (score > bestDirScore) {
                    bestDirScore = score;
                    bestDir = DIR_UP;
                }
            }
            if (canMoveDown && oppositeDir != DIR_DOWN) {
                float score = directionToPlayer.y; // Pontuação alta se o jogador estiver abaixo
                if (score > bestDirScore) {
                    bestDirScore = score;
                    bestDir = DIR_DOWN;
                }
            }
            if (canMoveLeft && oppositeDir != DIR_LEFT) {
                float score = -directionToPlayer.x; // Pontuação alta se o jogador estiver à esquerda
                if (score > bestDirScore) {
                    bestDirScore = score;
                    bestDir = DIR_LEFT;
                }
            }
            if (canMoveRight && oppositeDir != DIR_RIGHT) {
                float score = directionToPlayer.x; // Pontuação alta se o jogador estiver à direita
                if (score > bestDirScore) {
                    bestDirScore = score;
                    bestDir = DIR_RIGHT;
                }
            }
            
            // Se nenhuma direção "boa" foi encontrada (preso ou só pode voltar)
            if (bestDir == DIR_NONE) {
                // Tenta voltar (último caso, se estiver num beco sem saída)
                if (oppositeDir == DIR_UP && canMoveUp) bestDir = DIR_UP;
                else if (oppositeDir == DIR_DOWN && canMoveDown) bestDir = DIR_DOWN;
                else if (oppositeDir == DIR_LEFT && canMoveLeft) bestDir = DIR_LEFT;
                else if (oppositeDir == DIR_RIGHT && canMoveRight) bestDir = DIR_RIGHT;
                else bestDir = DIR_NONE; // Totalmente preso
            }
            
            ghosts[i].direction = bestDir; // Define a nova direção
        }
    }
    // ^^^ FIM DO BLOCO SUBSTITUÍDO ^^^

    // --- Lógica de Colisão (Jogador vs Fantasma) ---
    Rectangle playerRect = { player.position.x - TILE_SIZE/3, player.position.y - TILE_SIZE/3, TILE_SIZE/1.5f, TILE_SIZE/1.5f };
    
    for (int i = 0; i < 2; i++) {
        Rectangle ghostRect = { ghosts[i].position.x - TILE_SIZE/3, ghosts[i].position.y - TILE_SIZE/3, TILE_SIZE/1.5f, TILE_SIZE/1.5f };
        
        if (CheckCollisionRecs(playerRect, ghostRect)) {
            if (powerUpActive) {
                // Comeu o fantasma
                player.score += 200;
                // Reseta o fantasma
                ghosts[i].position = (Vector2){ (float)(10 * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(9 * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
            // ... (linha 701, aproximadamente)
            // vvv SUBSTITUA O BLOCO 'else' INTEIRO POR ESTE vvv

            } else {
                // Perdeu uma vida
                player.lives--;

                // Verifica Game Over IMEDIATAMENTE
                if (player.lives <= 0) {
                    gameState = STATE_GAME_OVER;
                } else {
                    // Se NÃO for game over, reseta as posições
                    
                    bool ghost1Reset = false; // Flag para achar o primeiro G
                    
                    for (int y = 0; y < MAP_ROWS; y++) {
                        for (int x = 0; x < MAP_COLS; x++) {
                            
                            // Reseta o jogador
                            if (defaultMap[y][x] == 'P') {
                                player.position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                            }
                            
                            // Reseta os fantasmas (LÓGICA CORRIGIDA)
                            if (defaultMap[y][x] == 'G') {
                                if (!ghost1Reset) {
                                    // Coloca o primeiro fantasma no primeiro G
                                    ghosts[0].position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                                    ghosts[0].direction = DIR_UP; // <-- CORREÇÃO IMPORTANTE
                                    ghosts[0].isScared = false;
                                    ghost1Reset = true;
                                } else {
                                    // Coloca o segundo fantasma no segundo G
                                    ghosts[1].position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                                    ghosts[1].direction = DIR_UP; // <-- CORREÇÃO IMPORTANTE
                                    ghosts[1].isScared = false;
                                }
                            }
                        }
                    }
                    
                    // Reseta a direção do jogador
                    player.direction = DIR_NONE;
                    player.nextDirection = DIR_NONE;
                }
            }
// ^^^ FIM DA SUBSTITUIÇÃO ^^^
        }
    }

    // --- Lógica de Vitória ---
    if (pelletsCount <= 0) {
        gameState = STATE_WIN;
    }
}

// Desenha os elementos do jogo
static void DrawGame(void)
{
    // Desenha o mapa (paredes, bolinhas)
    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            Vector2 pos = (Vector2){ (float)(x * TILE_SIZE) + offsetX, (float)(y * TILE_SIZE) + offsetY };
            
            if (map[y][x] == '#') {
                DrawRectangleV(pos, (Vector2){ TILE_SIZE, TILE_SIZE }, WALL_COLOR);
            }
            else if (map[y][x] == '.') {
                DrawCircleV((Vector2){ pos.x + TILE_SIZE / 2, pos.y + TILE_SIZE / 2 }, 3, PELLET_COLOR);
            }
            else if (map[y][x] == 'O') {
                DrawCircleV((Vector2){ pos.x + TILE_SIZE / 2, pos.y + TILE_SIZE / 2 }, 8, POWERUP_COLOR);
            }
        }
    }

    // Desenha o jogador
    DrawCircleV(player.position, (TILE_SIZE / 2) - 2, PLAYER_COLOR);

    // Desenha os fantasmas
    for (int i = 0; i < 2; i++) {
        Color c = ghosts[i].color;
        if (powerUpActive) {
            // Pisca se o power-up estiver acabando
            c = (powerUpTimer < 180 && (powerUpTimer / 30) % 2 == 0) ? WHITE : DARKBLUE;
        }
        DrawCircleV(ghosts[i].position, (TILE_SIZE / 2) - 2, c);
    }
}

// Desenha a Interface do Usuário
static void DrawUI(void)
{
    // Pontuação
    DrawText(TextFormat("SCORE: %04d", player.score), offsetX, 10, 20, WHITE);
    
    // Vidas
    DrawText("LIVES:", screenWidth - offsetX - 120, 10, 20, WHITE);
    for (int i = 0; i < player.lives; i++) {
        DrawCircle(screenWidth - offsetX - 80 + (i * 30), 20, (TILE_SIZE / 2) - 2, PLAYER_COLOR);
    }
}
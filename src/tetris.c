#include "raylib.h"
#include "game.h"
#include "tetris.h"
#include <stdlib.h> // Para a função rand()

// --- Constantes do Jogo Tetris ---
#define GRID_WIDTH 15
#define GRID_HEIGHT 20
#define BLOCK_SIZE 24 
// --- Estruturas ---

// A estrutura para guardar a peça ativa
typedef struct Tetromino {
    int type;       // O tipo da peça (0-6)
    int rotation;   // A rotação (0-3)
    int x;          // Posição X no grid
    int y;          // Posição Y no grid
    Color color;    // A cor da peça
} Tetromino;

// --- Variáveis Estáticas (Globais do Tetris) ---

// O tabuleiro principal do jogo. 0 = vazio. 1-7 = cor da peça.
static int grid[GRID_HEIGHT][GRID_WIDTH] = { 0 };

// A peça que o jogador está controlando
static Tetromino currentPiece;

// Temporizador para a gravidade
static int frameCounter = 0;
static int gravitySpeed = 30; // A peça cai 1 bloco a cada 30 frames (0.5 seg)

// Deslocamento para centralizar o grid na tela
static int gridOffsetX = 0;
static int gridOffsetY = 0;

static bool gameOver = false;

// --- Definição das Peças (Tetrominós) ---
// [tipo][rotação][y][x]
// Esta é a parte mais complexa: define o formato de cada peça em cada rotação
const int TETROMINO_SHAPES[7][4][4][4] = {
    // I-shape
    {
        {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},
        {{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}},
        {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}}
    },
    // O-shape
    {
        {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}}
    },
    // T-shape
    {
        {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}},
        {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}
    },
    // S-shape
    {
        {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
        {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}}
    },
    // Z-shape
    {
        {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}
    },
    // J-shape
    {
        {{0,0,0,0},{1,0,0,0},{1,1,1,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}
    },
    // L-shape
    {
        {{0,0,0,0},{0,0,1,0},{1,1,1,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
        {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}
    }
};

// Cores para cada tipo de peça (o índice 0 é o fundo)
const Color TETROMINO_COLORS[8] = {
    DARKGRAY,   // 0: Fundo do grid
    SKYBLUE,    // 1: I
    YELLOW,     // 2: O
    PURPLE,     // 3: T
    GREEN,      // 4: S
    RED,        // 5: Z
    BLUE,       // 6: J
    ORANGE      // 7: L
};

// --- Funções Auxiliares (A Lógica do Jogo) ---

// Função que cria uma nova peça aleatória no topo
void SpawnNewPiece(void)
{
    currentPiece.type = GetRandomValue(0, 6); // Tipo aleatório (0-6)
    currentPiece.rotation = 0; // Rotação inicial
    currentPiece.x = GRID_WIDTH / 2 - 2; // Posição X inicial (centralizada)
    currentPiece.y = 0; // Posição Y inicial (no topo)
    currentPiece.color = TETROMINO_COLORS[currentPiece.type + 1]; // Pega a cor (index + 1)
}

// Função que checa se a peça colide com algo
bool CheckCollision(Tetromino piece)
{
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            // Se esta parte da peça (matriz 4x4) existe
            if (TETROMINO_SHAPES[piece.type][piece.rotation][y][x] == 1)
            {
                // Calcula a posição real no grid
                int gridX = piece.x + x;
                int gridY = piece.y + y;

                // 1. Colisão com a parede esquerda
                if (gridX < 0) return true;
                // 2. Colisão com a parede direita
                if (gridX >= GRID_WIDTH) return true;
                // 3. Colisão com o chão
                if (gridY >= GRID_HEIGHT) return true;
                // 4. Colisão com outra peça já travada no grid
                if (grid[gridY][gridX] != 0) return true;
            }
        }
    }
    return false; // Não houve colisão
}

// "Trava" a peça no grid quando ela aterrissa
void LockPiece(Tetromino piece)
{
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (TETROMINO_SHAPES[piece.type][piece.rotation][y][x] == 1)
            {
                int gridX = piece.x + x;
                int gridY = piece.y + y;
                // Salva a cor (tipo + 1) no grid
                grid[gridY][gridX] = piece.type + 1;
            }
        }
    }
}

// Checa e limpa linhas completas
void ClearLines(void)
{
    int linesCleared = 0;
    // Checa de baixo para cima
    for (int y = GRID_HEIGHT - 1; y >= 0; y--)
    {
        bool lineIsFull = true;
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            if (grid[y][x] == 0)
            {
                lineIsFull = false;
                break;
            }
        }

        if (lineIsFull)
        {
            linesCleared++;
            // Move todas as linhas acima para baixo
            for (int row = y; row > 0; row--)
            {
                for (int x = 0; x < GRID_WIDTH; x++)
                {
                    grid[row][x] = grid[row - 1][x];
                }
            }
            // Limpa a linha do topo
            for (int x = 0; x < GRID_WIDTH; x++)
            {
                grid[0][x] = 0;
            }
            // Repete a checagem para esta mesma linha (y)
            // pois ela agora tem o conteúdo da linha de cima
            y++;
        }
    }
}

// --- Funções Principais (Init, Update, Draw) ---

// Função chamada uma vez para preparar o jogo
void InitTetris(void)
{
    // Limpa o tabuleiro
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            grid[y][x] = 0;
        }
    }

    // Calcula os offsets de centralização
    gridOffsetX = (GetScreenWidth() - (GRID_WIDTH * BLOCK_SIZE)) / 2;
    gridOffsetY = (GetScreenHeight() - (GRID_HEIGHT * BLOCK_SIZE)) / 2;
    
    // Reseta o estado do jogo
    gameOver = false;
    frameCounter = 0;
    
    // Cria a primeira peça
    SpawnNewPiece();
}

// Função de lógica, chamada a cada frame
void UpdateTetris(GameScene *currentScene)
{
    // Recalcula offsets se a tela mudar
    gridOffsetX = (GetScreenWidth() - (GRID_WIDTH * BLOCK_SIZE)) / 2;
    gridOffsetY = (GetScreenHeight() - (GRID_HEIGHT * BLOCK_SIZE)) / 2;

    // Sair do jogo e voltar ao menu
    if (IsKeyPressed(KEY_ESCAPE))
    {
        *currentScene = SCENE_MENU;
        return; // Sai da função
    }

    // Se o jogo acabou, não faça mais nada
    if (gameOver)
    {
        // Pressione Enter para recomeçar
        if (IsKeyPressed(KEY_ENTER))
        {
            InitTetris(); // Reinicia o jogo
        }
        return;
    }

    // --- Input do Jogador ---
    Tetromino testPiece = currentPiece; // Cria uma peça de "teste"

    // Mover para Esquerda
    if (IsKeyPressed(KEY_LEFT))
    {
        testPiece.x--;
        if (!CheckCollision(testPiece)) // Se a posição de teste for válida
        {
            currentPiece.x = testPiece.x; // Confirma o movimento
        }
    }
    // Mover para Direita
    if (IsKeyPressed(KEY_RIGHT))
    {
        testPiece.x++;
        if (!CheckCollision(testPiece))
        {
            currentPiece.x = testPiece.x;
        }
    }
    // Rotação (Tecla Cima)
    if (IsKeyPressed(KEY_UP))
    {
        testPiece.rotation = (testPiece.rotation + 1) % 4; // Gira
        if (!CheckCollision(testPiece))
        {
            currentPiece.rotation = testPiece.rotation;
        }
    }
    // Acelerar Queda (Tecla Baixo)
    if (IsKeyDown(KEY_DOWN))
    {
        frameCounter += 10; // Adiciona "tempo" ao contador
    }

    // --- Gravidade ---
    frameCounter++;
    if (frameCounter >= gravitySpeed)
    {
        frameCounter = 0; // Reinicia o contador
        testPiece = currentPiece;
        testPiece.y++; // Tenta mover para baixo

        if (CheckCollision(testPiece))
        {
            // Colidiu! Trava a peça antiga
            LockPiece(currentPiece);
            // Limpa linhas
            ClearLines();
            // Cria uma nova peça
            SpawnNewPiece();
            
            // Checa Game Over: Se a nova peça já colide, o jogo acabou.
            if (CheckCollision(currentPiece))
            {
                gameOver = true;
            }
        }
        else
        {
            // Não colidiu, confirma a queda
            currentPiece.y = testPiece.y;
        }
    }
}

// Função de desenho, chamada a cada frame
void DrawTetris(void)
{
    //
    // !!! IMPORTANTE: EU REMOVI O 'ClearBackground(BLACK)' DAQUI !!!
    // Agora o fundo da floresta (desenhado no main.c) vai aparecer.
    //

    // Desenha um título
    DrawText("TETRIS", gridOffsetX, gridOffsetY - 40, 40, WHITE);
    
    // Desenha o "fundo" do tabuleiro (os quadrados cinzas)
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            Color color = DARKGRAY;
            
            // Se o grid tiver uma peça travada, usa a cor dela
            if (grid[y][x] != 0)
            {
                color = TETROMINO_COLORS[grid[y][x]];
            }

            DrawRectangle(gridOffsetX + (x * BLOCK_SIZE), gridOffsetY + (y * BLOCK_SIZE), BLOCK_SIZE - 1, BLOCK_SIZE - 1, color);
        }
    }
    
    // Desenha a peça atual (que está caindo)
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (TETROMINO_SHAPES[currentPiece.type][currentPiece.rotation][y][x] == 1)
            {
                int pieceX = currentPiece.x + x;
                int pieceY = currentPiece.y + y;
                
                DrawRectangle(gridOffsetX + (pieceX * BLOCK_SIZE), gridOffsetY + (pieceY * BLOCK_SIZE), BLOCK_SIZE - 1, BLOCK_SIZE - 1, currentPiece.color);
            }
        }
    }
    
    // Se o jogo acabou, mostra a mensagem
    if (gameOver)
    {
        int textWidth = MeasureText("GAME OVER", 40);
        DrawText("GAME OVER", gridOffsetX + (GRID_WIDTH * BLOCK_SIZE / 2) - (textWidth / 2), gridOffsetY + (GRID_HEIGHT * BLOCK_SIZE / 2) - 40, 40, RED);
        
        textWidth = MeasureText("Pressione ENTER para reiniciar", 20);
        DrawText("Pressione ENTER para reiniciar", gridOffsetX + (GRID_WIDTH * BLOCK_SIZE / 2) - (textWidth / 2), gridOffsetY + (GRID_HEIGHT * BLOCK_SIZE / 2), 20, WHITE);
    }
}
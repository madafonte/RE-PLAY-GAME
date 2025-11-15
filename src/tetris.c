#include "raylib.h"
#include "game.h"
#include "tetris.h"
#include <stdlib.h>

#define GRID_WIDTH 15
#define GRID_HEIGHT 20
#define BLOCK_SIZE 24

typedef struct Tetromino {
    int type;
    int rotation;
    int x;
    int y;
    Color color;
} Tetromino;

static int grid[GRID_HEIGHT][GRID_WIDTH] = { 0 };
static Tetromino currentPiece;
static int frameCounter = 0;
static int gravitySpeed = 30;
static int gridOffsetX = 0;
static int gridOffsetY = 0;
static bool gameOver = false;

const int TETROMINO_SHAPES[7][4][4][4] = {
    {
        {{0,0,0,0},{1,1,1,1},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{0,1,0,0},{0,1,0,0}},
        {{0,0,0,0},{0,0,0,0},{1,1,1,1},{0,0,0,0}},
        {{0,0,1,0},{0,0,1,0},{0,0,1,0},{0,0,1,0}}
    },
    {
        {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,0,0},{0,1,1,0},{0,1,1,0},{0,0,0,0}}
    },
    {
        {{0,0,0,0},{1,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,1,0,0},{1,1,0,0},{0,1,0,0},{0,0,0,0}},
        {{0,1,0,0},{1,1,1,0},{0,0,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}
    },
    {
        {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}},
        {{0,0,0,0},{0,1,1,0},{1,1,0,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,1,0},{0,0,1,0},{0,0,0,0}}
    },
    {
        {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,1,0},{0,1,1,0},{0,1,0,0},{0,0,0,0}}
    },
    {
        {{0,0,0,0},{1,0,0,0},{1,1,1,0},{0,0,0,0}},
        {{0,1,1,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{0,0,1,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{1,1,0,0},{0,0,0,0}}
    },
    {
        {{0,0,0,0},{0,0,1,0},{1,1,1,0},{0,0,0,0}},
        {{0,1,0,0},{0,1,0,0},{0,1,1,0},{0,0,0,0}},
        {{0,0,0,0},{1,1,1,0},{1,0,0,0},{0,0,0,0}},
        {{1,1,0,0},{0,1,0,0},{0,1,0,0},{0,0,0,0}}
    }
};

const Color TETROMINO_COLORS[8] = {
    DARKGRAY,
    SKYBLUE,
    YELLOW,
    PURPLE,
    GREEN,
    RED,
    BLUE,
    ORANGE
};

void SpawnNewPiece(void)
{
    currentPiece.type = GetRandomValue(0, 6);
    currentPiece.rotation = 0;
    currentPiece.x = GRID_WIDTH / 2 - 2;
    currentPiece.y = 0;
    currentPiece.color = TETROMINO_COLORS[currentPiece.type + 1];
}

bool CheckCollision(Tetromino piece)
{
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            if (TETROMINO_SHAPES[piece.type][piece.rotation][y][x] == 1)
            {
                int gridX = piece.x + x;
                int gridY = piece.y + y;

                if (gridX < 0) return true;
                if (gridX >= GRID_WIDTH) return true;
                if (gridY >= GRID_HEIGHT) return true;
                if (grid[gridY][gridX] != 0) return true;
            }
        }
    }
    return false;
}

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
                grid[gridY][gridX] = piece.type + 1;
            }
        }
    }
}

void ClearLines(void)
{
    int linesCleared = 0;
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
            for (int row = y; row > 0; row--)
            {
                for (int x = 0; x < GRID_WIDTH; x++)
                {
                    grid[row][x] = grid[row - 1][x];
                }
            }
            for (int x = 0; x < GRID_WIDTH; x++)
            {
                grid[0][x] = 0;
            }
            y++;
        }
    }
}

void InitTetris(void)
{
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            grid[y][x] = 0;
        }
    }

    gridOffsetX = (GetScreenWidth() - (GRID_WIDTH * BLOCK_SIZE)) / 2;
    gridOffsetY = (GetScreenHeight() - (GRID_HEIGHT * BLOCK_SIZE)) / 2;
    
    gameOver = false;
    frameCounter = 0;
    
    SpawnNewPiece();
}

void UpdateTetris(GameScene *currentScene)
{
    gridOffsetX = (GetScreenWidth() - (GRID_WIDTH * BLOCK_SIZE)) / 2;
    gridOffsetY = (GetScreenHeight() - (GRID_HEIGHT * BLOCK_SIZE)) / 2;

    if (IsKeyPressed(KEY_ESCAPE))
    {
        *currentScene = SCENE_MENU;
        return;
    }

    if (gameOver)
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitTetris();
        }
        return;
    }

    Tetromino testPiece = currentPiece;

    if (IsKeyPressed(KEY_LEFT))
    {
        testPiece.x--;
        if (!CheckCollision(testPiece))
        {
            currentPiece.x = testPiece.x;
        }
    }
    if (IsKeyPressed(KEY_RIGHT))
    {
        testPiece.x++;
        if (!CheckCollision(testPiece))
        {
            currentPiece.x = testPiece.x;
        }
    }
    if (IsKeyPressed(KEY_UP))
    {
        testPiece.rotation = (testPiece.rotation + 1) % 4;
        if (!CheckCollision(testPiece))
        {
            currentPiece.rotation = testPiece.rotation;
        }
    }
    if (IsKeyDown(KEY_DOWN))
    {
        frameCounter += 10;
    }

    frameCounter++;
    if (frameCounter >= gravitySpeed)
    {
        frameCounter = 0;
        testPiece = currentPiece;
        testPiece.y++;

        if (CheckCollision(testPiece))
        {
            LockPiece(currentPiece);
            ClearLines();
            SpawnNewPiece();
            
            if (CheckCollision(currentPiece))
            {
                gameOver = true;
            }
        }
        else
        {
            currentPiece.y = testPiece.y;
        }
    }
}

void DrawTetris(void)
{
    DrawText("TETRIS", gridOffsetX, gridOffsetY - 40, 40, WHITE);
    
    for (int y = 0; y < GRID_HEIGHT; y++)
    {
        for (int x = 0; x < GRID_WIDTH; x++)
        {
            Color color = DARKGRAY;
            
            if (grid[y][x] != 0)
            {
                color = TETROMINO_COLORS[grid[y][x]];
            }

            DrawRectangle(gridOffsetX + (x * BLOCK_SIZE), gridOffsetY + (y * BLOCK_SIZE), BLOCK_SIZE - 1, BLOCK_SIZE - 1, color);
        }
    }
    
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
    
    if (gameOver)
    {
        int textWidth = MeasureText("GAME OVER", 40);
        DrawText("GAME OVER", gridOffsetX + (GRID_WIDTH * BLOCK_SIZE / 2) - (textWidth / 2), gridOffsetY + (GRID_HEIGHT * BLOCK_SIZE / 2) - 40, 40, RED);
        
        textWidth = MeasureText("Pressione ENTER para reiniciar", 20);
        DrawText("Pressione ENTER para reiniciar", gridOffsetX + (GRID_WIDTH * BLOCK_SIZE / 2) - (textWidth / 2), gridOffsetY + (GRID_HEIGHT * BLOCK_SIZE / 2), 20, WHITE);
    }
}
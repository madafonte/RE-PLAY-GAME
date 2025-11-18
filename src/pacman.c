#include "pacman.h" 
#include "raylib.h"
#include "game.h"  
#include <math.h>   

#define MAP_ROWS 21
#define MAP_COLS 21
#define TILE_SIZE 24
#define PLAYER_SPEED 3
#define GHOST_SPEED 1


#define WALL_COLOR BLUE
#define PELLET_COLOR YELLOW
#define POWERUP_COLOR WHITE
#define PLAYER_COLOR YELLOW
#define GHOST_COLOR_1 RED
#define GHOST_COLOR_2 PINK


typedef enum {
    STATE_PLAYING,
    STATE_WIN,
    STATE_GAME_OVER
} PacmanState;


typedef enum {
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_NONE
} Direction;


typedef struct {
    Vector2 position;
    Vector2 speed;
    Direction direction;       
    Direction nextDirection;  
    int lives;
    int score;
} Player;


typedef struct {
    Vector2 position;
    Direction direction; 
    Vector2 speed;
    Color color;
    bool isScared; 
} Ghost;


static bool isInitialized = false;   
static PacmanState gameState;       
static int screenWidth = 0;
static int screenHeight = 0;


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
    ".....................", 
    ".....................",
    ".....................",
    ".....................",
    ".....................",
    "....................."
};

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


static int offsetX = 0;
static int offsetY = 0;


static void InitPacman(void);      
static void UpdateGame(GameScene *currentScene); 
static void DrawGame(void);        
static void DrawUI(void);          


void UpdatePacman(GameScene *currentScene)
{
    if (!isInitialized) {
        InitPacman();
    }

    
    if (gameState == STATE_PLAYING) {
        UpdateGame(currentScene);
    }

   
    if (gameState == STATE_GAME_OVER || gameState == STATE_WIN) {
        if (IsKeyPressed(KEY_ENTER)) {
            InitPacman(); 
        }
        if (IsKeyPressed(KEY_Q)) {
            *currentScene = SCENE_MENU; 
            isInitialized = false;   
        }
    }
}

void DrawPacman(void)
{
    if (!isInitialized) return;

    ClearBackground(BLACK);

   
    DrawGame();


    DrawUI();

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


static void InitPacman(void)
{
    screenWidth = GetScreenWidth();
    screenHeight = GetScreenHeight();

    offsetX = (screenWidth - (MAP_COLS * TILE_SIZE)) / 2;
    offsetY = (screenHeight - (MAP_ROWS * TILE_SIZE)) / 2;

    gameState = STATE_PLAYING;
    pelletsCount = 0;
    powerUpActive = false;
    powerUpTimer = 0;

    player.lives = 3;
    player.score = 0;
    player.direction = DIR_NONE;
    player.nextDirection = DIR_NONE;

    for (int y = 0; y < MAP_ROWS; y++) {
        for (int x = 0; x < MAP_COLS; x++) {
            map[y][x] = defaultMap[y][x];

            if (map[y][x] == 'P') {
                player.position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                map[y][x] = ' '; 
            }
            else if (map[y][x] == 'G') {
                if (ghosts[0].position.x == 0) { 
                    ghosts[0].position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                    ghosts[0].color = GHOST_COLOR_1;
                    ghosts[0].isScared = false;
                    ghosts[0].direction = DIR_UP;
                } else { 
                    ghosts[1].position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                    ghosts[1].color = GHOST_COLOR_2;
                    ghosts[1].isScared = false;
                    ghosts[1].direction = DIR_UP; 
                }
                map[y][x] = ' ';
            }
            else if (map[y][x] == '.') {
                pelletsCount++;
            }
            else if (map[y][x] == 'O') {
                pelletsCount++; 
            }
        }
    }
    
    isInitialized = true;
}

static bool IsWall(int row, int col) {
    if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS) {
        return true;
    }
    return (map[row][col] == '#');
}

static int GetMapCol(float posX) {
    return (int)((posX - offsetX) / TILE_SIZE);
}

static int GetMapRow(float posY) {
    return (int)((posY - offsetY) / TILE_SIZE);
}

static void SnapGhostToGrid(Ghost *ghost)
{
    int currentRow = GetMapRow(ghost->position.y);
    int currentCol = GetMapCol(ghost->position.x);
    ghost->position = (Vector2){
        (float)currentCol * TILE_SIZE + offsetX + TILE_SIZE / 2,
        (float)currentRow * TILE_SIZE + offsetY + TILE_SIZE / 2
    };
}

static void UpdateGame(GameScene *currentScene)
{
   
    if (IsKeyPressed(KEY_Q)) {
        *currentScene = SCENE_MENU; 
        isInitialized = false;    
        return;                
    }

    if (IsKeyDown(KEY_RIGHT)) player.nextDirection = DIR_RIGHT;
    else if (IsKeyDown(KEY_LEFT)) player.nextDirection = DIR_LEFT;
    else if (IsKeyDown(KEY_UP)) player.nextDirection = DIR_UP;
    else if (IsKeyDown(KEY_DOWN)) player.nextDirection = DIR_DOWN;


    int playerRow = GetMapRow(player.position.y);
    int playerCol = GetMapCol(player.position.x);
    
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

    
    Vector2 nextPos = (Vector2){ player.position.x + player.speed.x, player.position.y + player.speed.y };
    int nextPosRow = GetMapRow(nextPos.y + (player.speed.y > 0 ? TILE_SIZE / 2 - 1 : -TILE_SIZE / 2 + 1));
    int nextPosCol = GetMapCol(nextPos.x + (player.speed.x > 0 ? TILE_SIZE / 2 - 1 : -TILE_SIZE / 2 + 1));
    
    if (!IsWall(nextPosRow, nextPosCol)) {
        player.position = nextPos;
    } else {
        player.position = (Vector2){(float)playerCol * TILE_SIZE + offsetX + TILE_SIZE / 2, (float)playerRow * TILE_SIZE + offsetY + TILE_SIZE / 2};
        player.direction = DIR_NONE; 
    }
    
    if (player.position.x < offsetX) player.position.x = offsetX + (MAP_COLS * TILE_SIZE);
    if (player.position.x > offsetX + (MAP_COLS * TILE_SIZE)) player.position.x = offsetX;


    
    int playerMapRow = GetMapRow(player.position.y);
    int playerMapCol = GetMapCol(player.position.x);
    
    char *cell = &map[playerMapRow][playerMapCol];

    if (*cell == '.') { 
        *cell = ' ';
        player.score += 10;
        pelletsCount--;
    }
    else if (*cell == 'O') {
        *cell = ' ';
        player.score += 50;
        pelletsCount--;
        powerUpActive = true;
        powerUpTimer = 60 * 7; 
        ghosts[0].isScared = true;
        ghosts[1].isScared = true;
    }

    if (powerUpActive) {
        powerUpTimer--;
        if (powerUpTimer <= 0) {
            powerUpActive = false;
            ghosts[0].isScared = false;
            ghosts[1].isScared = false;
        }
    }

    for (int i = 0; i < 2; i++) {
        
        switch (ghosts[i].direction) {
            case DIR_UP: ghosts[i].speed = (Vector2){0, -GHOST_SPEED}; break;
            case DIR_DOWN: ghosts[i].speed = (Vector2){0, GHOST_SPEED}; break;
            case DIR_LEFT: ghosts[i].speed = (Vector2){-GHOST_SPEED, 0}; break;
            case DIR_RIGHT: ghosts[i].speed = (Vector2){GHOST_SPEED, 0}; break;
            case DIR_NONE: ghosts[i].speed = (Vector2){0, 0}; break;
        }

        Vector2 nextPos = (Vector2){ ghosts[i].position.x + ghosts[i].speed.x, ghosts[i].position.y + ghosts[i].speed.y };
        int nextPosRow = GetMapRow(nextPos.y + (ghosts[i].speed.y > 0 ? TILE_SIZE / 2 - 1 : -TILE_SIZE / 2 + 1));
        int nextPosCol = GetMapCol(nextPos.x + (ghosts[i].speed.x > 0 ? TILE_SIZE / 2 - 1 : -TILE_SIZE / 2 + 1));

        if (!IsWall(nextPosRow, nextPosCol)) {
            ghosts[i].position = nextPos;
        } else {
            SnapGhostToGrid(&ghosts[i]);
            Direction oppositeDir = DIR_NONE;
            if (ghosts[i].direction == DIR_UP) oppositeDir = DIR_DOWN;
            if (ghosts[i].direction == DIR_DOWN) oppositeDir = DIR_UP;
            if (ghosts[i].direction == DIR_LEFT) oppositeDir = DIR_RIGHT;
            if (ghosts[i].direction == DIR_RIGHT) oppositeDir = DIR_LEFT;

            int ghostRow = GetMapRow(ghosts[i].position.y);
            int ghostCol = GetMapCol(ghosts[i].position.x);

            bool canMoveUp = !IsWall(ghostRow - 1, ghostCol);
            bool canMoveDown = !IsWall(ghostRow + 1, ghostCol);
            bool canMoveLeft = !IsWall(ghostRow, ghostCol - 1);
            bool canMoveRight = !IsWall(ghostRow, ghostCol + 1);

            Direction bestDir = DIR_NONE;
            float bestDirScore = -INFINITY;

           
            Vector2 directionToPlayer = (Vector2){ player.position.x - ghosts[i].position.x, player.position.y - ghosts[i].position.y };

           
            if (canMoveUp && oppositeDir != DIR_UP) {
                float score = -directionToPlayer.y; 
                    bestDirScore = score;
                    bestDir = DIR_UP;
                }
            }
            if (canMoveDown && oppositeDir != DIR_DOWN) {
                float score = directionToPlayer.y;
                if (score > bestDirScore) {
                    bestDirScore = score;
                    bestDir = DIR_DOWN;
                }
            }
            if (canMoveLeft && oppositeDir != DIR_LEFT) {
                float score = -directionToPlayer.x; 
                if (score > bestDirScore) {
                    bestDirScore = score;
                    bestDir = DIR_LEFT;
                }
            }
            if (canMoveRight && oppositeDir != DIR_RIGHT) {
                float score = directionToPlayer.x; 
                if (score > bestDirScore) {
                    bestDirScore = score;
                    bestDir = DIR_RIGHT;
                }
            }
            
           
            if (bestDir == DIR_NONE) {
                
                if (oppositeDir == DIR_UP && canMoveUp) bestDir = DIR_UP;
                else if (oppositeDir == DIR_DOWN && canMoveDown) bestDir = DIR_DOWN;
                else if (oppositeDir == DIR_LEFT && canMoveLeft) bestDir = DIR_LEFT;
                else if (oppositeDir == DIR_RIGHT && canMoveRight) bestDir = DIR_RIGHT;
                else bestDir = DIR_NONE; 
            
            ghosts[i].direction = bestDir; 
        }
    }

    Rectangle playerRect = { player.position.x - TILE_SIZE/3, player.position.y - TILE_SIZE/3, TILE_SIZE/1.5f, TILE_SIZE/1.5f };
    
    for (int i = 0; i < 2; i++) {
        Rectangle ghostRect = { ghosts[i].position.x - TILE_SIZE/3, ghosts[i].position.y - TILE_SIZE/3, TILE_SIZE/1.5f, TILE_SIZE/1.5f };
        
        if (CheckCollisionRecs(playerRect, ghostRect)) {
            if (powerUpActive) {
                player.score += 200;
                ghosts[i].position = (Vector2){ (float)(10 * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(9 * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };


            } else {
                player.lives--;
                if (player.lives <= 0) {
                    gameState = STATE_GAME_OVER;
                } else {
                    
                    bool ghost1Reset = false; 
                    
                    for (int y = 0; y < MAP_ROWS; y++) {
                        for (int x = 0; x < MAP_COLS; x++) {
                            
                            
                            if (defaultMap[y][x] == 'P') {
                                player.position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                            }
                            
                            
                            if (defaultMap[y][x] == 'G') {
                                if (!ghost1Reset) {
                                    ghosts[0].position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                                    ghosts[0].direction = DIR_UP; 
                                    ghosts[0].isScared = false;
                                    ghost1Reset = true;
                                } else {
                                    ghosts[1].position = (Vector2){ (float)(x * TILE_SIZE) + offsetX + (TILE_SIZE / 2), (float)(y * TILE_SIZE) + offsetY + (TILE_SIZE / 2) };
                                    ghosts[1].direction = DIR_UP; 
                                    ghosts[1].isScared = false;
                                }
                            }
                        }
                    }
                    
                    player.direction = DIR_NONE;
                    player.nextDirection = DIR_NONE;
                }
            }
        }
    }

    if (pelletsCount <= 0) {
        gameState = STATE_WIN;
    }
}

static void DrawGame(void)
{
   
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

    DrawCircleV(player.position, (TILE_SIZE / 2) - 2, PLAYER_COLOR);

   
    for (int i = 0; i < 2; i++) {
        Color c = ghosts[i].color;
        if (powerUpActive) {
          
            c = (powerUpTimer < 180 && (powerUpTimer / 30) % 2 == 0) ? WHITE : DARKBLUE;
        }
        DrawCircleV(ghosts[i].position, (TILE_SIZE / 2) - 2, c);
    }
}

static void DrawUI(void)
{
    
    DrawText(TextFormat("SCORE: %04d", player.score), offsetX, 10, 20, WHITE);
    DrawText("LIVES:", screenWidth - offsetX - 120, 10, 20, WHITE);
    for (int i = 0; i < player.lives; i++) {
        DrawCircle(screenWidth - offsetX - 80 + (i * 30), 20, (TILE_SIZE / 2) - 2, PLAYER_COLOR);
    }
}
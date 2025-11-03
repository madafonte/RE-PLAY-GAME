#include "raylib.h"
#include "game.h"     // Nosso header compartilhado
#include "pacman.h"   // Header do Pacman
#include "tetris.h"   // <<-- NOVO HEADER
#include "seaquest.h" // <<-- NOVO HEADER

// Estrutura para o portal
typedef struct GamePortal {
    Rectangle rect;
    const char *name;
    Texture2D portalImg;
    GameScene targetScene;
} GamePortal;

// --- Protótipos das Funções ---
// (Apenas as funções que "moram" neste arquivo)
void UpdateMenu(GameScene *currentScene, GamePortal portals[3]);
void DrawMenu(GamePortal portals[3], const char *title, const char *footer);


// --- Função Principal ---
int main(void)
{
    // O código da função main() continua EXATAMENTE O MESMO
    // (InitWindow, LoadTexture, Definição dos Portais, etc.)

    const int screenWidth = 960;
    const int screenHeight = 540;
    InitWindow(screenWidth, screenHeight, "RE: PLAY");
    SetTargetFPS(60);

    GameScene currentScene = SCENE_MENU;

    Texture2D pacmanPortalImg   = LoadTexture("assets/images/portal_pacman.png");
    Texture2D tetrisPortalImg   = LoadTexture("assets/images/portal_tetris.png");
    Texture2D seaquestPortalImg = LoadTexture("assets/images/portal_seaquest.png");

    const int portalWidth = 180;
    const int portalHeight = 280;
    int spacing = 60;
    int totalWidth = (portalWidth * 3) + (spacing * 2);
    int posY = 120;
    int posX_start = (screenWidth - totalWidth) / 2;

    GamePortal portals[3];
    portals[0] = (GamePortal){ (Rectangle){ posX_start, posY, portalWidth, portalHeight }, "PAC-MAN", pacmanPortalImg, SCENE_PACMAN };
    portals[1] = (GamePortal){ (Rectangle){ posX_start + portalWidth + spacing, posY, portalWidth, portalHeight }, "TETRIS", tetrisPortalImg, SCENE_TETRIS };
    portals[2] = (GamePortal){ (Rectangle){ posX_start + (portalWidth + spacing) * 2, posY, portalWidth, portalHeight }, "SEAQUEST", seaquestPortalImg, SCENE_SEAQUEST };

    const char *title = "RE: PLAY";
    const char *footer = "developed in C with Raylib";

    // O Loop Principal também continua EXATAMENTE IGUAL
    // Ele funciona porque incluímos todos os .h
    while (!WindowShouldClose())
    {
        switch(currentScene)
        {
            case SCENE_MENU:
                UpdateMenu(&currentScene, portals);
                break;
            case SCENE_PACMAN:
                UpdatePacman(&currentScene);
                break;
            case SCENE_TETRIS:
                UpdateTetris(&currentScene); // <-- Chama a função do tetris.c
            break;
            case SCENE_SEAQUEST:
                UpdateSeaquest(&currentScene); // <-- Chama a função do seaquest.c
                break;
            default: break;
        }

        BeginDrawing();
            ClearBackground(LIGHTGRAY);

            switch(currentScene)
            {
                case SCENE_MENU:
                    DrawMenu(portals, title, footer);
                    break;
                case SCENE_PACMAN:
                    DrawPacman();
                    break;
                case SCENE_TETRIS:
                    DrawTetris();
                    break;
                case SCENE_SEAQUEST:
                    DrawSeaquest();
                    break;
                default: break;
            }

        EndDrawing();
    }

    UnloadTexture(pacmanPortalImg);
    UnloadTexture(tetrisPortalImg);
    UnloadTexture(seaquestPortalImg);
    CloseWindow();

    return 0;
}


// --- Definição das Funções ---
// (Agora SÓ temos as funções do Menu aqui)

void UpdateMenu(GameScene *currentScene, GamePortal portals[3])
{
    Vector2 mousePoint = GetMousePosition();
    for (int i = 0; i < 3; i++) {
        bool isHovered = CheckCollisionPointRec(mousePoint, portals[i].rect);
        if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            *currentScene = portals[i].targetScene;
            break;
        }
    }
}

void DrawMenu(GamePortal portals[3], const char *title, const char *footer)
{
    int screenWidth = GetScreenWidth();

    int titleFontSize = 50;
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, (screenWidth - titleWidth) / 2, 30, titleFontSize, MAROON);

    for (int i = 0; i < 3; i++) {

        Rectangle sourceRect = { 0.0f, 0.0f, (float)portals[i].portalImg.width, (float)portals[i].portalImg.height };
        Rectangle destRect = portals[i].rect;

        DrawTexturePro(portals[i].portalImg, sourceRect, destRect, (Vector2){ 0, 0 }, 0.0f, WHITE);

        int textPosY = portals[i].rect.y + portals[i].rect.height + 10;
        DrawText(portals[i].name, portals[i].rect.x + (portals[i].rect.width - MeasureText(portals[i].name, 30)) / 2, textPosY, 30, BLACK);
    }

    int footerFontSize = 10;
    int footerWidth = MeasureText(footer, footerFontSize);
    DrawText(footer, (screenWidth - footerWidth) / 2, 500, footerFontSize, BLACK);
}

//
// TODO O CÓDIGO DO TETRIS E SEAQUEST FOI REMOVIDO DAQUI
//
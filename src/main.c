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

    Texture2D pacmanPortalImg   = LoadTexture("assets/images/portal-sem-fundo.png");
    Texture2D tetrisPortalImg   = LoadTexture("assets/images/portal-sem-fundo.png");
    Texture2D seaquestPortalImg = LoadTexture("assets/images/portal-sem-fundo.png");
    Texture2D background = LoadTexture("assets/images/fundo_inicio.png");

    GamePortal portals[3]; 

    portals[0].portalImg = pacmanPortalImg;
    portals[0].name = "PAC-MAN";
    portals[0].targetScene = SCENE_PACMAN;

    portals[1].portalImg = tetrisPortalImg;
    portals[1].name = "TETRIS";
    portals[1].targetScene = SCENE_TETRIS;

    portals[2].portalImg = seaquestPortalImg;
    portals[2].name = "SEAQUEST";
    portals[2].targetScene = SCENE_SEAQUEST;

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
            ClearBackground(BLACK); // Mantém o fundo preto como fallback

            // --- CÓDIGO PARA DESENHAR O FUNDO COBRINDO A TELA ---
            float screenWidth = (float)GetScreenWidth();
            float screenHeight = (float)GetScreenHeight();
            float imageWidth = (float)background.width;
            float imageHeight = (float)background.height;

            // Calcula as proporções (aspect ratios)
            float screenRatio = screenWidth / screenHeight;
            float imageRatio = imageWidth / imageHeight;

            Rectangle sourceRec = { 0, 0, imageWidth, imageHeight }; // Área da imagem original a ser desenhada
            Rectangle destRec = { 0, 0, screenWidth, screenHeight }; // Onde na tela será desenhada

            // Se a tela for mais "larga" que a imagem, cortamos a altura da imagem
            if (screenRatio > imageRatio) {
                sourceRec.height = imageWidth / screenRatio;
                sourceRec.y = (imageHeight - sourceRec.height) / 2.0f; // Centraliza verticalmente
            } 
            // Se a tela for mais "alta" que a imagem, cortamos a largura da imagem
            else {
                sourceRec.width = imageHeight * screenRatio;
                sourceRec.x = (imageWidth - sourceRec.width) / 2.0f; // Centraliza horizontalmente
            }
            
            // Desenha a parte calculada da imagem, esticando-a para preencher a tela
            DrawTexturePro(background, sourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);
            // --- FIM DO CÓDIGO DO FUNDO --- 
            

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
    UnloadTexture(background);
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
    int currentScreenWidth = GetScreenWidth();
    int currentScreenHeight = GetScreenHeight();

    const int portalWidth = 180; 
    const int portalHeight = 280; 

    float scale = 1.0f; 
    if (currentScreenWidth < 800) scale = 0.8f; 
    if (currentScreenWidth > 1200) scale = 1.2f; 

    int scaledPortalWidth = (int)(portalWidth * scale);
    int scaledPortalHeight = (int)(portalHeight * scale);
    
    int posY = (int)(currentScreenHeight * 0.45f); 
    int spacing = (int)(currentScreenWidth * 0.05f); 
    int totalPortalsWidth = (scaledPortalWidth * 3) + (spacing * 2);
    int posX_start = (int)(currentScreenWidth * 0.55f) - (totalPortalsWidth / 2); 


    portals[0].rect = (Rectangle){ (float)posX_start, (float)posY, (float)scaledPortalWidth, (float)scaledPortalHeight };
    portals[1].rect = (Rectangle){ (float)(posX_start + scaledPortalWidth + spacing), (float)posY, (float)scaledPortalWidth, (float)scaledPortalHeight };
    portals[2].rect = (Rectangle){ (float)(posX_start + (scaledPortalWidth + spacing) * 2), (float)posY, (float)scaledPortalWidth, (float)scaledPortalHeight };

    int titleFontSize = (int)(currentScreenHeight * 0.09f);
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, (currentScreenWidth - titleWidth) / 2, (int)(currentScreenHeight * 0.05f), titleFontSize, MAROON); 

    for (int i = 0; i < 3; i++) {
        Rectangle sourceRect = { 0.0f, 0.0f, (float)portals[i].portalImg.width, (float)portals[i].portalImg.height };
        Rectangle destRect = portals[i].rect;
        
        DrawTexturePro(portals[i].portalImg, sourceRect, destRect, (Vector2){ 0, 0 }, 0.0f, WHITE);

        int textFontSize = (int)(currentScreenHeight * 0.05f);
        int textPosY = portals[i].rect.y + portals[i].rect.height + (int)(currentScreenHeight * 0.02f);
        int textWidth = MeasureText(portals[i].name, textFontSize);
        DrawText(portals[i].name, portals[i].rect.x + (portals[i].rect.width - textWidth) / 2, textPosY, textFontSize, BLACK);
    }

    int footerFontSize = (int)(currentScreenHeight * 0.02f);
    int footerWidth = MeasureText(footer, footerFontSize);
    DrawText(footer, (currentScreenWidth - footerWidth) / 2, (int)(currentScreenHeight * 0.95f), footerFontSize, BLACK);
}
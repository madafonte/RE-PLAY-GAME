#include "raylib.h"
#include "game.h"      
#include "pacman.h"    
#include "tetris.h"  
#include "seaquest.h"  


typedef struct GamePortal {
    Rectangle rect;
    const char *name;
    Texture2D portalImg;
    GameScene targetScene;
} GamePortal;

void UpdateMenu(GameScene *currentScene, GamePortal portals[3]);
void DrawMenu(GamePortal portals[3], const char *title, const char *footer);


void UpdateIntro(GameScene *currentScene, int *currentPage, int *frameCounter);
void DrawIntro(Texture2D placa1, Texture2D placa2, Texture2D placaNome, int currentPage);




int main(void)
{
    const int screenWidth = 960;
    const int screenHeight = 540;
    InitWindow(screenWidth, screenHeight, "RE: PLAY");
    SetTargetFPS(60);

    
    GameScene currentScene = SCENE_INTRO; 
    int introPage = 0;                   
    int introFrameCounter = 0;           
    

   
    Texture2D pacmanPortalImg   = LoadTexture("assets/images/portal-sem-fundo.png");
    Texture2D tetrisPortalImg   = LoadTexture("assets/images/portal-sem-fundo.png");
    Texture2D seaquestPortalImg = LoadTexture("assets/images/portal-sem-fundo.png");
    Texture2D background        = LoadTexture("assets/images/fundo_inicio.png");

   
    Texture2D placaNome   = LoadTexture("assets/images/placa_nome.png");
    Texture2D placaFrase1 = LoadTexture("assets/images/placa_frase1.png");
    Texture2D placaFrase2 = LoadTexture("assets/images/placa_frase2.png");
 
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

   
    while (!WindowShouldClose())
    {
       
        switch(currentScene)
        {
            
            case SCENE_INTRO:
                UpdateIntro(&currentScene, &introPage, &introFrameCounter);
                break;
           

            case SCENE_MENU:
                UpdateMenu(&currentScene, portals);
                break;
            case SCENE_PACMAN:
                UpdatePacman(&currentScene);
                break;
            case SCENE_TETRIS:
                UpdateTetris(&currentScene); 
                break;
            case SCENE_SEAQUEST:
                UpdateSeaquest(&currentScene); 
                break;
            default: break;
        }

       
        BeginDrawing();
            ClearBackground(BLACK); 

          
            float screenWidth_f = (float)GetScreenWidth(); 
            float screenHeight_f = (float)GetScreenHeight();
            float imageWidth = (float)background.width;
            float imageHeight = (float)background.height;
            float screenRatio = screenWidth_f / screenHeight_f;
            float imageRatio = imageWidth / imageHeight;
            Rectangle sourceRec = { 0, 0, imageWidth, imageHeight }; 
            Rectangle destRec = { 0, 0, screenWidth_f, screenHeight_f }; 

            if (screenRatio > imageRatio) {
                sourceRec.height = imageWidth / screenRatio;
                sourceRec.y = (imageHeight - sourceRec.height) / 2.0f; 
            } else {
                sourceRec.width = imageHeight * screenRatio;
                sourceRec.x = (imageWidth - sourceRec.width) / 2.0f; 
            }
            DrawTexturePro(background, sourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);
           
            

            switch(currentScene)
            {
                
                case SCENE_INTRO:
                    DrawIntro(placaFrase1, placaFrase2, placaNome, introPage);
                    break;
                

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

   
    UnloadTexture(placaNome);
    UnloadTexture(placaFrase1);
    UnloadTexture(placaFrase2);
   
    CloseWindow();

    return 0;
}



void UpdateMenu(GameScene *currentScene, GamePortal portals[3])
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
    
    int posY = (int)(currentScreenHeight * 0.5f); 
    
  
    int spacing = (int)(currentScreenWidth * 0.05f); 
    
    
    int totalPortalsWidth = (scaledPortalWidth * 3) + (spacing * 2);
    int posX_start = (int)(currentScreenWidth * 0.6f) - (totalPortalsWidth / 2); 
  
   
    portals[0].rect = (Rectangle){ (float)posX_start, (float)posY, (float)scaledPortalWidth, (float)scaledPortalHeight };
    portals[1].rect = (Rectangle){ (float)(posX_start + scaledPortalWidth + spacing), (float)posY, (float)scaledPortalWidth, (float)scaledPortalHeight };
    portals[2].rect = (Rectangle){ (float)(posX_start + (scaledPortalWidth + spacing) * 2), (float)posY, (float)scaledPortalWidth, (float)scaledPortalHeight };

   
    Vector2 mousePoint = GetMousePosition();
    for (int i = 0; i < 3; i++) {
        bool isHovered = CheckCollisionPointRec(mousePoint, portals[i].rect);
        if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

           
            if (portals[i].targetScene == SCENE_TETRIS)
            {
                InitTetris();
            }

            *currentScene = portals[i].targetScene;
            break;
        }
    }
}


void DrawMenu(GamePortal portals[3], const char *title, const char *footer)
{
    int currentScreenWidth = GetScreenWidth();
    int currentScreenHeight = GetScreenHeight();

  
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

void UpdateIntro(GameScene *currentScene, int *currentPage, int *frameCounter)
{
    (*frameCounter)++; 

    if (*currentPage == 0)
    {
       
        if (*frameCounter > 180) 
        {
            *currentPage = 1;         
            *frameCounter = 0;       
        }
    }
    else if (*currentPage == 1)
    {
        
        if (*frameCounter > 300) 
        {
            *currentPage = 2;        
            *frameCounter = 0;       
        }
    }
    else if (*currentPage == 2)
    {
        
        if (*frameCounter > 300) 
        {
            *currentScene = SCENE_MENU;
        }
    }
}


void DrawIntro(Texture2D placa1, Texture2D placa2, Texture2D placaNome, int currentPage)
{
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    float scaleMargin = 0.9f;
    int desiredWidth = (int)(screenW * scaleMargin); 
    int desiredHeight; 

    Texture2D currentPlaca; 

    if (currentPage == 0)
    {
        currentPlaca = placaNome;
    }
    else if (currentPage == 1)
    {
        currentPlaca = placa1;
    }
    else 
    {
        currentPlaca = placa2;
    }

    if (currentPlaca.width > 0 && currentPlaca.height > 0) {
        desiredHeight = (int)(desiredWidth / ((float)currentPlaca.width / currentPlaca.height));
        
        if (desiredHeight > (int)(screenH * scaleMargin)) {
            desiredHeight = (int)(screenH * scaleMargin);
            desiredWidth = (int)(desiredHeight * ((float)currentPlaca.width / currentPlaca.height));
        }

    } else {
        desiredWidth = (int)(screenW * 0.5f);
        desiredHeight = (int)(screenH * 0.5f);
    }
    
    int displayPosX = ((screenW - desiredWidth) / 2) -350; 
    int displayPosY = -300; 
    
    Rectangle destRec = { (float)displayPosX, (float)displayPosY, (float)desiredWidth, (float)desiredHeight };
    Rectangle sourceRec = { 0.0f, 0.0f, (float)currentPlaca.width, (float)currentPlaca.height };

    DrawTexturePro(currentPlaca, sourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);
}
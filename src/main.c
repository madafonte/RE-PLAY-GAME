#include "raylib.h"
#include "game.h"      // Nosso header compartilhado
#include "pacman.h"    // Header do Pacman
#include "tetris.h"    // Header do Tetris
#include "seaquest.h"  // Header do Seaquest

// Estrutura para o portal
typedef struct GamePortal {
    Rectangle rect;
    const char *name;
    Texture2D portalImg;
    GameScene targetScene;
} GamePortal;

// --- Protótipos das Funções ---
void UpdateMenu(GameScene *currentScene, GamePortal portals[3]);
void DrawMenu(GamePortal portals[3], const char *title, const char *footer);

// <<< MUDANÇA AQUI: Protótipos das novas funções da Intro >>>
void UpdateIntro(GameScene *currentScene, int *currentPage, int *frameCounter);
void DrawIntro(Texture2D placa1, Texture2D placa2, Texture2D placaNome, int currentPage);
// <<< FIM DA MUDANÇA >>>


// --- Função Principal ---
int main(void)
{
    const int screenWidth = 960;
    const int screenHeight = 540;
    InitWindow(screenWidth, screenHeight, "RE: PLAY");
    SetTargetFPS(60);

    // <<< MUDANÇA AQUI: Cena inicial e variáveis da intro >>>
    GameScene currentScene = SCENE_INTRO; // Começa na Intro
    int introPage = 0;                    // Controla qual placa mostrar
    int introFrameCounter = 0;            // Nosso temporizador
    // <<< FIM DA MUDANÇA >>>

    // Texturas do Menu
    Texture2D pacmanPortalImg   = LoadTexture("assets/images/portal-sem-fundo.png");
    Texture2D tetrisPortalImg   = LoadTexture("assets/images/portal-sem-fundo.png");
    Texture2D seaquestPortalImg = LoadTexture("assets/images/portal-sem-fundo.png");
    Texture2D background        = LoadTexture("assets/images/fundo_inicio.png");

    // <<< MUDANÇA AQUI: Carrega texturas da intro >>>
    Texture2D placaNome   = LoadTexture("assets/images/placa_nome.png");
    Texture2D placaFrase1 = LoadTexture("assets/images/placa_frase1.png");
    Texture2D placaFrase2 = LoadTexture("assets/images/placa_frase2.png");
    // <<< FIM DA MUDANÇA >>>

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

    // --- Loop Principal ---
    while (!WindowShouldClose())
    {
        // --- Lógica (Update) ---
        switch(currentScene)
        {
            // <<< MUDANÇA AQUI: Case da lógica da intro >>>
            case SCENE_INTRO:
                UpdateIntro(&currentScene, &introPage, &introFrameCounter);
                break;
            // <<< FIM DA MUDANÇA >>>

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

        // --- Desenho (Draw) ---
        BeginDrawing();
            ClearBackground(BLACK); 

            // --- CÓDIGO PARA DESENHAR O FUNDO COBRINDO A TELA ---
            float screenWidth_f = (float)GetScreenWidth(); // Usei _f para não conflitar
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
            // --- FIM DO CÓDIGO DO FUNDO --- 
            

            switch(currentScene)
            {
                // <<< MUDANÇA AQUI: Case do desenho da intro >>>
                case SCENE_INTRO:
                    DrawIntro(placaFrase1, placaFrase2, placaNome, introPage);
                    break;
                // <<< FIM DA MUDANÇA >>>

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

    // --- Descarregar Texturas ---
    UnloadTexture(pacmanPortalImg);
    UnloadTexture(tetrisPortalImg);
    UnloadTexture(seaquestPortalImg);
    UnloadTexture(background);

    // <<< MUDANÇA AQUI: Descarrega texturas da intro >>>
    UnloadTexture(placaNome);
    UnloadTexture(placaFrase1);
    UnloadTexture(placaFrase2);
    // <<< FIM DA MUDANÇA >>>

    CloseWindow();

    return 0;
}


// --- Definição das Funções do Menu ---
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

// <<< MUDANÇA AQUI: Novas funções da Intro >>>
// ----------------------------------------------------------------------------------
// Implementação da Cena de Intro (Lógica com Temporizador)
// ----------------------------------------------------------------------------------
void UpdateIntro(GameScene *currentScene, int *currentPage, int *frameCounter)
{
    (*frameCounter)++; // Adiciona 1 ao contador a cada frame

    // Seu jogo roda a 60 FPS (SetTargetFPS(60))
    // 3 segundos = 3 * 60 = 180 frames
    // 5 segundos = 5 * 60 = 300 frames

    // Verifica em qual página (imagem) estamos
    if (*currentPage == 0)
    {
        // Página 0 (placa_nome) - 3 segundos
        if (*frameCounter > 180) // Se passaram 3 segundos
        {
            *currentPage = 1;         // Vai para a próxima página
            *frameCounter = 0;        // Reinicia o contador
        }
    }
    else if (*currentPage == 1)
    {
        // Página 1 (placa_frase1) - 5 segundos
        if (*frameCounter > 300) // Se passaram 5 segundos
        {
            *currentPage = 2;         // Vai para a próxima página
            *frameCounter = 0;        // Reinicia o contador
        }
    }
    else if (*currentPage == 2)
    {
        // Página 2 (placa_frase2) - 5 segundos
        if (*frameCounter > 300) // Se passaram 5 segundos
        {
            // Acabou a intro, vai para o Menu
            *currentScene = SCENE_MENU;
        }
    }
}

// ----------------------------------------------------------------------------------
// Implementação da Cena de Intro (Desenho)
// ----------------------------------------------------------------------------------
void DrawIntro(Texture2D placa1, Texture2D placa2, Texture2D placaNome, int currentPage)
{
    // --- Valores para TENTAR OCUPAR QUASE A TELA INTEIRA ---
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();

    // Vamos tentar fazer a placa ocupar 90% da largura da tela e ajustar a altura
    // para manter a proporção da imagem original.
    int desiredWidth = (int)(screenW * 0.9f); 
    int desiredHeight; 

    Texture2D currentPlaca; 

    if (currentPage == 0)
    {
        currentPlaca = placaNome;
    }
    else if (currentPage == 1)
    {
        currentPlaca = placa1; // placaFrase1
    }
    else // currentPage == 2
    {
        currentPlaca = placa2; // placaFrase2
    }

    // Calcular a altura mantendo a proporção da imagem.
    if (currentPlaca.width > 0 && currentPlaca.height > 0) { // Garante que a imagem é válida
        desiredHeight = (int)(desiredWidth / ((float)currentPlaca.width / currentPlaca.height));
        
        // Se a altura calculada for maior que a tela (ex: imagem original muito "alta" e "fina"),
        // ajustamos pela altura da tela, mantendo a proporção.
        if (desiredHeight > screenH) {
            desiredHeight = (int)(screenH * 0.9f);
            desiredWidth = (int)(desiredHeight * ((float)currentPlaca.width / currentPlaca.height));
        }

    } else {
        // Fallback: se a imagem não tiver dimensão válida, use um tamanho fixo grande
        desiredWidth = (int)(screenW * 0.5f);
        desiredHeight = (int)(screenH * 0.5f);
    }
    
    // Posição X: Centraliza horizontalmente
    int displayPosX = ((screenW - desiredWidth) / 2) + 150; 
    // Posição Y: Para ficar no topo, com uma pequena margem (ex: 30 pixels)
    int displayPosY = -300; // Ajuste este valor se quiser mais para cima/baixo
    
    // Retângulo de destino na tela (onde a placa será desenhada)
    Rectangle destRec = { (float)displayPosX, (float)displayPosY, (float)desiredWidth, (float)desiredHeight };

    // Retângulo de origem na textura (a imagem original inteira)
    Rectangle sourceRec = { 0.0f, 0.0f, (float)currentPlaca.width, (float)currentPlaca.height };

    // Desenha a placa
    DrawTexturePro(currentPlaca, sourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);

    // <<< Código de Debugging >>>
    // Imprime os valores no terminal para vermos o que está acontecendo
    TraceLog(LOG_INFO, "DEBUG_SIZE: Screen WxH: %dx%d", screenW, screenH);
    TraceLog(LOG_INFO, "DEBUG_SIZE: Placa Original WxH: %dx%d", currentPlaca.width, currentPlaca.height);
    TraceLog(LOG_INFO, "DEBUG_SIZE: Placa Desired WxH: %dx%d", desiredWidth, desiredHeight);
    TraceLog(LOG_INFO, "DEBUG_SIZE: Placa Pos XxY: %dx%d", displayPosX, displayPosY);
} // <-- O '}' QUE FALTAVA!
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

    CleanupPacman();

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
// --- Definição das Funções do Menu ---

//
// SUBSTITUA SUA FUNÇÃO INTEIRA POR ESTA
//
void UpdateMenu(GameScene *currentScene, GamePortal portals[3])
{
    // --- CÁLCULO DE POSIÇÃO (O que estava faltando) ---
    // Pega o tamanho ATUAL da tela
    int currentScreenWidth = GetScreenWidth();
    int currentScreenHeight = GetScreenHeight();

    // Define o tamanho base dos portais
    const int portalWidth = 180; 
    const int portalHeight = 280; 

    // Define uma escala (para telas muito pequenas ou muito grandes)
    float scale = 1.0f; 
    if (currentScreenWidth < 800) scale = 0.8f; 
    if (currentScreenWidth > 1200) scale = 1.2f; 

    // Aplica a escala
    int scaledPortalWidth = (int)(portalWidth * scale);
    int scaledPortalHeight = (int)(portalHeight * scale);
    
    // Calcula as posições Y e X de forma PROPORCIONAL
    
    // Posição Y: 50% para baixo da altura da tela (centralizado verticalmente)
    // (Ajuste 0.5f para 0.6f para mover mais para baixo, 0.4f para cima)
    int posY = (int)(currentScreenHeight * 0.5f); 
    
    // Espaçamento: 5% da largura da tela
    int spacing = (int)(currentScreenWidth * 0.05f); 
    
    // Posição X: Começa em 60% da largura da tela e centraliza o bloco de portais ali
    // (Ajuste 0.6f para 0.7f para mover mais para a direita, 0.5f para a esquerda)
    int totalPortalsWidth = (scaledPortalWidth * 3) + (spacing * 2);
    int posX_start = (int)(currentScreenWidth * 0.6f) - (totalPortalsWidth / 2); 
    // --- FIM DO CÁLCULO ---

    // Agora que as variáveis existem, podemos definir os retângulos
    portals[0].rect = (Rectangle){ (float)posX_start, (float)posY, (float)scaledPortalWidth, (float)scaledPortalHeight };
    portals[1].rect = (Rectangle){ (float)(posX_start + scaledPortalWidth + spacing), (float)posY, (float)scaledPortalWidth, (float)scaledPortalHeight };
    portals[2].rect = (Rectangle){ (float)(posX_start + (scaledPortalWidth + spacing) * 2), (float)posY, (float)scaledPortalWidth, (float)scaledPortalHeight };

    // Lógica de clique (agora funciona, pois os 'rects' estão corretos)
    Vector2 mousePoint = GetMousePosition();
    for (int i = 0; i < 3; i++) {
        bool isHovered = CheckCollisionPointRec(mousePoint, portals[i].rect);
        if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

            // Se o portal clicado for o do Tetris,
            // chame a função InitTetris() antes de mudar de cena.
            if (portals[i].targetScene == SCENE_TETRIS)
            {
                InitTetris();
            }

            *currentScene = portals[i].targetScene;
            break;
        }

        if (isHovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
    
            // Se o portal clicado for o do Tetris,
            if (portals[i].targetScene == SCENE_TETRIS) {
                InitTetris();
            }
            
            // <<< ADICIONE ESTA LINHA AQUI >>>
            // Se o portal clicado for o do Pac-Man,
            if (portals[i].targetScene == SCENE_PACMAN) {
                InitPacman(); // Inicializa/Reinicia o jogo Pac-Man
            }

            *currentScene = portals[i].targetScene;
            break;
        }
    }
}

//
// SUA FUNÇÃO DrawMenu JÁ ESTAVA CORRETA.
// Ela depende dos cálculos da UpdateMenu.
//
void DrawMenu(GamePortal portals[3], const char *title, const char *footer)
{
    int currentScreenWidth = GetScreenWidth();
    int currentScreenHeight = GetScreenHeight();

    // Desenha o Título
    int titleFontSize = (int)(currentScreenHeight * 0.09f);
    int titleWidth = MeasureText(title, titleFontSize);
    DrawText(title, (currentScreenWidth - titleWidth) / 2, (int)(currentScreenHeight * 0.05f), titleFontSize, MAROON); 

    // Desenha os Portais e seus Nomes
    for (int i = 0; i < 3; i++) {
        Rectangle sourceRect = { 0.0f, 0.0f, (float)portals[i].portalImg.width, (float)portals[i].portalImg.height };
        Rectangle destRect = portals[i].rect; // Usa o 'rect' calculado no UpdateMenu
        
        DrawTexturePro(portals[i].portalImg, sourceRect, destRect, (Vector2){ 0, 0 }, 0.0f, WHITE);

        // Desenha o nome
        int textFontSize = (int)(currentScreenHeight * 0.05f);
        int textPosY = portals[i].rect.y + portals[i].rect.height + (int)(currentScreenHeight * 0.02f);
        int textWidth = MeasureText(portals[i].name, textFontSize);
        DrawText(portals[i].name, portals[i].rect.x + (portals[i].rect.width - textWidth) / 2, textPosY, textFontSize, BLACK);
    }

    // Desenha o Rodapé
    int footerFontSize = (int)(currentScreenHeight * 0.02f);
    int footerWidth = MeasureText(footer, footerFontSize);
    DrawText(footer, (currentScreenWidth - footerWidth) / 2, (int)(currentScreenHeight * 0.95f), footerFontSize, BLACK);
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
    
    int displayPosX = (screenW - desiredWidth) / 2;
    int displayPosY = (screenH - desiredHeight) / 2;
    
    Rectangle destRec = { (float)displayPosX, (float)displayPosY, (float)desiredWidth, (float)desiredHeight };
    Rectangle sourceRec = { 0.0f, 0.0f, (float)currentPlaca.width, (float)currentPlaca.height };

    DrawTexturePro(currentPlaca, sourceRec, destRec, (Vector2){0,0}, 0.0f, WHITE);
}
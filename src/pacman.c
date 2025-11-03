#include "raylib.h"
#include "pacman.h" // Inclui seu próprio "cardápio"
// "game.h" já é incluído através do "pacman.h"

// --- Funções Placeholder dos Jogos (COM A FRASE E LÓGICA ALTERADAS) ---

void UpdatePacman(GameScene *currentScene)
{
    // Agora só checa o ENTER
    if (IsKeyPressed(KEY_ENTER)) {
        *currentScene = SCENE_MENU;
    }
}

void DrawPacman(void)
{
    const char *msg = "TELA DO JOGO PAC-MAN";
    const char *msg2 = "Pressione ENTER para voltar ao menu";

    DrawText(msg, (GetScreenWidth() - MeasureText(msg, 40)) / 2, GetScreenHeight() / 2 - 40, 40, BLACK);
    DrawText(msg2, (GetScreenWidth() - MeasureText(msg2, 20)) / 2, GetScreenHeight() / 2 + 20, 20, DARKGRAY);
}
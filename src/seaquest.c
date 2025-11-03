#include "raylib.h"
#include "seaquest.h"

void UpdateSeaquest(GameScene *currentScene)
{
    if (IsKeyPressed(KEY_ENTER)) {
        *currentScene = SCENE_MENU;
    }
}

void DrawSeaquest(void)
{
    const char *msg = "TELA DO JOGO SEAQUEST";
    const char *msg2 = "Pressione ENTER para voltar ao menu";

    DrawText(msg, (GetScreenWidth() - MeasureText(msg, 40)) / 2, GetScreenHeight() / 2 - 40, 40, BLACK);
    DrawText(msg2, (GetScreenWidth() - MeasureText(msg2, 20)) / 2, GetScreenHeight() / 2 + 20, 20, DARKGRAY);
}
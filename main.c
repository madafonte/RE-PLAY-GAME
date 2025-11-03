#include <stdio.h>
#include "screen.h"
#include "keyboard.h"

int main() {
    screenInit(1);
    keyboardInit();

    screenClear();
    screenSetColor(YELLOW, BLACK);

    screenGotoxy(10, 10);
    printf("Projeto RE-PLAY-GAME iniciado!");

    screenGotoxy(10, 12);
    printf("Pressione 'q' para sair.");

    screenUpdate();

    int ch = 0;
    while (ch != 'q') {
        if (keyhit()) {
            ch = readch();
        }
    }
    keyboardDestroy();
    screenDestroy();

    return 0;
}

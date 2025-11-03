#ifndef GAME_H
#define GAME_H

// 1. Definição das Cenas (Estados do Jogo)
// Colocamos isso aqui para que main.c e pacman.c possam saber o que é GameScene
typedef enum GameScene {
    SCENE_MENU,
    SCENE_PACMAN,
    SCENE_TETRIS,
    SCENE_SEAQUEST
} GameScene;

#endif // GAME_H
#ifndef SCORES_H
#define SCORES_H

// --- Estrutura (Struct) para a Lista Encadeada ---

/*
 * Este é o "nó" (node) da nossa lista.
 * Cada nó armazena um único score e um "endereço" (ponteiro)
 * para o próximo nó da lista.
 */
typedef struct ScoreNode {
    int score;
    struct ScoreNode *next; // Ponteiro para o próximo nó
} ScoreNode;


// --- Declarações das Funções (Interface Pública) ---

/*
 * Estas são as funções que o 'pacman.c' usará para
 * gerenciar a lista de high scores.
 */

// Adiciona um novo score à lista (de forma ordenada)
// Recebe um ponteiro-para-ponteiro (**head) para poder MODIFICAR
// qual nó é o primeiro da lista.
void AddScore(ScoreNode **head, int newScore);

// Carrega os scores do arquivo e retorna o ponteiro para o
// primeiro nó (o 'head') da lista recém-criada.
ScoreNode* LoadScores(const char *filename);

// Salva a lista inteira (a partir do 'head') no arquivo.
void SaveScores(const char *filename, ScoreNode *head);

// Libera toda a memória (free) alocada para a lista.
void FreeScoreList(ScoreNode *head);

// Função bônus para desenhar os scores na tela
void DrawScores(ScoreNode *head, int x, int y);


#endif // SCORES_H
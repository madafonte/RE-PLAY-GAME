#include "scores.h"   // Inclui o "contrato" que este arquivo implementa
#include <stdio.h>    // Para fopen, fclose, fscanf, fprintf
#include <stdlib.h>   // Para malloc, free
#include "raylib.h"   // Para a função de desenhar (DrawScores)

/*
 * Adiciona um novo score na lista de forma ordenada (do maior para o menor)
 */
void AddScore(ScoreNode **head, int newScore) {
    
    // --- REQUISITO: Alocação Dinâmica ---
    // 1. Alocamos memória do tamanho de um "nó" da lista.
    ScoreNode *newNode = (ScoreNode*) malloc(sizeof(ScoreNode));
    
    // Verificação de segurança: se o 'malloc' falhar (faltar memória), ele retorna NULL.
    if (newNode == NULL) {
        printf("Erro: Falha ao alocar memoria para novo score!\n");
        return; 
    }
    
    // 2. Preenchemos o nó recém-criado com os dados
    newNode->score = newScore;
    newNode->next = NULL;

    // --- REQUISITO: Lógica da Lista Encadeada ---
    // 3. Encontramos o local correto para inserir o novo nó.

    // Caso A: A lista está vazia OU o novo score é o maior de todos.
    if (*head == NULL || (*head)->score <= newScore) {
        // O novo nó aponta para o antigo "primeiro" item (que pode ser NULL)
        newNode->next = *head;
        // O ponteiro 'head' (lá do pacman.c) agora aponta para o nosso novo nó.
        *head = newNode;
    } 
    // Caso B: Inserir no meio ou no fim da lista.
    else {
        ScoreNode *current = *head;
        // Percorremos a lista até encontrar um nó menor que o nosso,
        // ou até o fim da lista.
        while (current->next != NULL && current->next->score > newScore) {
            current = current->next;
        }
        
        // 'newNode' aponta para o item que 'current' apontava
        newNode->next = current->next;
        // 'current' agora aponta para o 'newNode', inserindo-o no meio.
        current->next = newNode;
    }
}

/*
 * Libera todos os nós da lista (free)
 */
void FreeScoreList(ScoreNode *head) {
    ScoreNode *current = head;
    ScoreNode *temp;

    // --- REQUISITO: Lógica da Lista Encadeada (percorrer) ---
    // Percorre a lista inteira, um nó de cada vez.
    while (current != NULL) {
        temp = current;          // 1. Guarda o nó atual
        current = current->next; // 2. Avança para o próximo
        
        // --- REQUISITO: Alocação Dinâmica (free) ---
        // 3. Libera a memória do nó que guardamos.
        free(temp);
    }
}

/*
 * Carrega os scores do arquivo
 */
ScoreNode* LoadScores(const char *filename) {
    ScoreNode *head = NULL; // Começa com uma lista vazia

    // --- REQUISITO: Leitura em Arquivo ---
    FILE *file = fopen(filename, "r"); // "r" = read (leitura)
    
    // Se o arquivo não existir (fopen retorna NULL),
    // apenas retornamos a lista vazia (NULL).
    if (file == NULL) {
        printf("Arquivo de scores '%s' nao encontrado. Um novo sera criado ao salvar.\n", filename);
        return NULL;
    }

    int scoreLido;
    // Lê cada linha (um inteiro) do arquivo
    // fscanf retorna 1 se conseguiu ler 1 item (%d)
    while (fscanf(file, "%d\n", &scoreLido) == 1) {
        // Para cada score lido, usamos nossa função para criar
        // um nó (com malloc) e adicioná-lo à lista.
        AddScore(&head, scoreLido);
    }

    fclose(file); // Fecha o arquivo
    return head; // Retorna o ponteiro para o início da lista
}

/*
 * Salva os scores no arquivo
 */
void SaveScores(const char *filename, ScoreNode *head) {
    
    // --- REQUISITO: Escrita em Arquivo ---
    FILE *file = fopen(filename, "w"); // "w" = write (escrita, sobrescreve)
    
    if (file == NULL) {
        printf("Erro: Nao foi possivel abrir '%s' para salvar scores!\n", filename);
        return;
    }

    // --- REQUISITO: Lógica da Lista Encadeada (percorrer) ---
    ScoreNode *current = head;
    int count = 0;
    
    // Percorre a lista, salvando apenas os 10 primeiros scores
    while (current != NULL && count < 10) {
        // Escreve o score (int) e uma quebra de linha no arquivo
        fprintf(file, "%d\n", current->score);
        current = current->next; // Avança na lista
        count++;
    }

    fclose(file); // Fecha o arquivo
}

/*
 * Função bônus para desenhar os scores na tela (usando Raylib)
 */
void DrawScores(ScoreNode *head, int x, int y) {
    DrawText("TOP SCORES:", x, y, 20, GOLD);
    
    ScoreNode *current = head;
    int count = 0;
    
    // Percorre a lista e desenha os 5 primeiros
    while (current != NULL && count < 5) {
        DrawText(TextFormat("%d", current->score), x + 20, y + 25 + (count * 20), 20, LIGHTGRAY);
        current = current->next;
        count++;
    }
}
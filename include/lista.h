/**
 * @file lista.h
 * @brief Lista simplesmente encadeada com inserção no fim.
 *
 * Utilizada para armazenar sequências de IDs de livros (como a ordem de visitação
 * resultante da Busca em Largura - BFS - e listagens auxiliares de adjacência).
 */

#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>

/**
 * @brief Nó da lista simplesmente encadeada.
 */
typedef struct NoLista {
    int id_livro;                   /**< ID do livro armazenado no nó */
    int distancia;                  /**< Distância (nível na BFS) opcional a partir da origem */
    struct NoLista *prox;           /**< Ponteiro para o próximo nó da lista */
} NoLista;

/**
 * @brief Estrutura descritora da Lista encadeada (com ponteiro para início e fim).
 */
typedef struct {
    NoLista *inicio;                /**< Primeiro elemento da lista */
    NoLista *fim;                   /**< Último elemento da lista (otimiza inserção no fim) */
    int tamanho;                    /**< Quantidade atual de elementos na lista */
} Lista;

/**
 * @brief Inicializa uma lista encadeada vazia.
 * @param lista Ponteiro para a estrutura Lista a inicializar.
 */
void lista_inicializar(Lista *lista);

/**
 * @brief Verifica se a lista está vazia.
 * @param lista Ponteiro constante para a lista.
 * @return true se estiver vazia, false caso contrário.
 */
bool lista_vazia(const Lista *lista);

/**
 * @brief Insere um novo ID de livro no final da lista encadeada.
 * @param lista Ponteiro para a lista.
 * @param id_livro Identificador do livro a inserir.
 * @param distancia Nível/distância associada (usar 0 quando não aplicável).
 * @return true se a alocação e inserção ocorreram com sucesso, false em erro de memória.
 */
bool lista_inserir_fim(Lista *lista, int id_livro, int distancia);

/**
 * @brief Remove a primeira ocorrência de um ID da lista.
 * @param lista Ponteiro para a lista.
 * @param id_livro Identificador do livro a remover.
 * @return true se o elemento foi encontrado e removido, false caso contrário.
 */
bool lista_remover(Lista *lista, int id_livro);

/**
 * @brief Verifica se um determinado ID já está presente na lista.
 * @param lista Ponteiro constante para a lista.
 * @param id_livro Identificador do livro a procurar.
 * @return true se o ID estiver na lista, false caso contrário.
 */
bool lista_contem(const Lista *lista, int id_livro);

/**
 * @brief Exibe os elementos da lista na consola pela ordem de inserção.
 * @param lista Ponteiro constante para a lista.
 */
void lista_exibir(const Lista *lista);

/**
 * @brief Liberta toda a memória associada aos nós da lista, deixando-a vazia.
 * @param lista Ponteiro para a lista.
 */
void lista_liberar(Lista *lista);

#endif /* LISTA_H */


/**
 * @file ordenacao.h
 * @brief Algoritmos de ordenação aplicados aos registos de livros exportados da BST.
 *
 * Permite ordenar e listar o catálogo de livros por diferentes critérios
 * (Título, Autor, Ano de Publicação ou ID) sem alterar a estrutura da BST.
 */

#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "bst.h"

/**
 * @brief Critérios disponíveis para ordenação do catálogo de livros.
 */
typedef enum {
    ORDENAR_POR_TITULO = 1,         /**< Ordem alfabética crescente de título (desempate por ID) */
    ORDENAR_POR_AUTOR = 2,          /**< Ordem alfabética crescente de autor (desempate por título) */
    ORDENAR_POR_ANO = 3,            /**< Ordem crescente de ano de publicação (desempate por título) */
    ORDENAR_POR_ID = 4              /**< Ordem crescente de identificador único */
} CriterioOrdenacao;

/**
 * @brief Compara dois livros de acordo com o critério especificado.
 * @param a Ponteiro constante para o primeiro livro.
 * @param b Ponteiro constante para o segundo livro.
 * @param criterio Critério de comparação escolhido.
 * @return Valor < 0 se a precede b, 0 se equivalentes, > 0 se a sucede b.
 */
int livro_comparar(const Livro *a, const Livro *b, CriterioOrdenacao criterio);

/**
 * @brief Ordena um vetor de livros utilizando o algoritmo QuickSort.
 * @param vetor Array de estruturas Livro a ordenar.
 * @param n Quantidade de elementos no vetor.
 * @param criterio Critério de ordenação a aplicar.
 */
void ordenar_livros_quicksort(Livro *vetor, int n, CriterioOrdenacao criterio);

/**
 * @brief Ordena um vetor de livros utilizando o algoritmo MergeSort (estável).
 * @param vetor Array de estruturas Livro a ordenar.
 * @param n Quantidade de elementos no vetor.
 * @param criterio Critério de ordenação a aplicar.
 */
void ordenar_livros_mergesort(Livro *vetor, int n, CriterioOrdenacao criterio);

/**
 * @brief Exporta os livros da BST para um vetor temporário, ordena segundo o critério
 *        indicado e exibe a listagem resultante no ecrã.
 * @param raiz Ponteiro para a raiz da BST.
 * @param criterio Critério de ordenação desejado.
 */
void bst_listar_ordenado(NoBST *raiz, CriterioOrdenacao criterio);

#endif /* ORDENACAO_H */


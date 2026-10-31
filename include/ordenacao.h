/**
 * @file ordenacao.h
 * @brief Algoritmos de ordenação próprios sobre cópias dos registos da BST (Requisito R06).
 *
 * Implementa Merge Sort e Quick Sort próprios (sem uso de qsort da stdlib)
 * sobre um vetor auxiliar de cópias independentes da struct Livro.
 * Ordena de forma crescente pelo campo "titulo" usando strcmp() (sensível a
 * maiúsculas/minúsculas), desempatando por ID crescente quando strcmp() == 0.
 */

#ifndef ORDENACAO_H
#define ORDENACAO_H

#include "bst.h"

/**
 * @brief Critérios disponíveis para ordenação do catálogo de livros.
 */
typedef enum {
    ORDENAR_POR_TITULO = 1,         /**< Ordem crescente de título via strcmp() com desempate por ID crescente (R06) */
    ORDENAR_POR_AUTOR = 2,          /**< Ordem crescente de autor via strcmp() com desempate por ID crescente */
    ORDENAR_POR_ANO = 3,            /**< Ordem crescente de ano de publicação com desempate por ID crescente */
    ORDENAR_POR_ID = 4              /**< Ordem crescente de identificador único (ID) */
} CriterioOrdenacao;

/**
 * @brief Compara dois livros de acordo com o critério especificado.
 *        Para ORDENAR_POR_TITULO (padrão R06), utiliza strcmp(a->titulo, b->titulo)
 *        e, em caso de igualdade (strcmp == 0), desempata por ID crescente (a->id - b->id).
 * @param a Ponteiro constante para o primeiro livro.
 * @param b Ponteiro constante para o segundo livro.
 * @param criterio Critério de comparação escolhido.
 * @return Valor < 0 se 'a' precede 'b', 0 se idênticos, > 0 se 'a' sucede 'b'.
 */
int livro_comparar(const Livro *a, const Livro *b, CriterioOrdenacao criterio);

/**
 * @brief Ordena um vetor de cópias de livros utilizando o algoritmo Quick Sort próprio.
 * @param vetor Array de estruturas Livro (cópias independentes) a ordenar.
 * @param n Quantidade de elementos no vetor.
 * @param criterio Critério de ordenação a aplicar.
 */
void ordenar_livros_quicksort(Livro *vetor, int n, CriterioOrdenacao criterio);

/**
 * @brief Ordena um vetor de cópias de livros utilizando o algoritmo Merge Sort próprio.
 * @param vetor Array de estruturas Livro (cópias independentes) a ordenar.
 * @param n Quantidade de elementos no vetor.
 * @param criterio Critério de ordenação a aplicar.
 */
void ordenar_livros_mergesort(Livro *vetor, int n, CriterioOrdenacao criterio);

/**
 * @brief Exporta cópias dos livros da BST para um vetor auxiliar temporário,
 *        ordena segundo o critério indicado (sem alterar a BST) e exibe a listagem.
 * @param raiz Ponteiro para a raiz da BST.
 * @param criterio Critério de ordenação desejado (ex: ORDENAR_POR_TITULO).
 */
void bst_listar_ordenado(NoBST *raiz, CriterioOrdenacao criterio);

#endif /* ORDENACAO_H */

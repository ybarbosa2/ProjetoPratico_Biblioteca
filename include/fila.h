/**
 * @file fila.h
 * @brief Fila dinâmica encadeada (FIFO) de suporte à Busca em Largura (BFS).
 *
 * Implementa uma fila dinâmica de IDs de livros para exploração em níveis
 * no grafo de relações entre obras da biblioteca.
 */

#ifndef FILA_H
#define FILA_H

#include <stdbool.h>

/**
 * @brief Nó da fila encadeada.
 */
typedef struct NoFila {
    int id_livro;                   /**< ID do livro enfileirado */
    struct NoFila *prox;            /**< Ponteiro para o próximo nó na fila */
} NoFila;

/**
 * @brief Estrutura descritora da Fila dinâmica (FIFO).
 */
typedef struct {
    NoFila *frente;                 /**< Ponteiro para o início da fila (remoção) */
    NoFila *tras;                   /**< Ponteiro para o fim da fila (inserção) */
    int tamanho;                    /**< Número de elementos atualmente na fila */
} Fila;

/**
 * @brief Inicializa uma fila dinâmica vazia.
 * @param fila Ponteiro para a estrutura Fila a inicializar.
 */
void fila_inicializar(Fila *fila);

/**
 * @brief Verifica se a fila está vazia.
 * @param fila Ponteiro constante para a fila.
 * @return true se não houver elementos, false caso contrário.
 */
bool fila_vazia(const Fila *fila);

/**
 * @brief Insere um ID de livro no final da fila (enqueue).
 * @param fila Ponteiro para a fila.
 * @param id_livro Identificador do livro a enfileirar.
 * @return true se foi enfileirado com sucesso, false em falha de alocação.
 */
bool fila_enfileirar(Fila *fila, int id_livro);

/**
 * @brief Remove e obtém o ID do livro na frente da fila (dequeue).
 * @param fila Ponteiro para a fila.
 * @param id_removido Ponteiro onde será guardado o ID desenfileirado.
 * @return true se um elemento foi removido, false se a fila estava vazia.
 */
bool fila_desenfileirar(Fila *fila, int *id_removido);

/**
 * @brief Consulta o elemento na frente da fila sem o remover (peek).
 * @param fila Ponteiro constante para a fila.
 * @param id_frente Ponteiro onde será guardado o ID da frente.
 * @return true se a consulta foi bem-sucedida, false se a fila estava vazia.
 */
bool fila_frente(const Fila *fila, int *id_frente);

/**
 * @brief Liberta toda a memória alocada pelos nós da fila.
 * @param fila Ponteiro para a fila.
 */
void fila_liberar(Fila *fila);

#endif /* FILA_H */


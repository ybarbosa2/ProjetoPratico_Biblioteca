/**
 * @file grafo.h
 * @brief Grafo não direcionado e não ponderado para relações entre livros.
 *
 * Relaciona livros exclusivamente através dos seus IDs (sem ponteiros para nós da BST),
 * utilizando um mapeamento entre o ID único do livro e um índice compacto [0..MAX_VERTICES-1]
 * na matriz de adjacência. Suporta no mínimo 100 vértices simultâneos e Busca em Largura (BFS).
 */

#ifndef GRAFO_H
#define GRAFO_H

#include <stdbool.h>
#include "lista.h"
#include "bst.h"

/** Capacidade mínima exigida de vértices simultâneos no grafo */
#define MAX_VERTICES 100

/**
 * @brief Estrutura do Grafo não direcionado e não ponderado com mapeamento ID -> índice.
 */
typedef struct {
    int ids_vertices[MAX_VERTICES];                 /**< Mapeamento índice compacto -> ID do livro */
    bool ativo[MAX_VERTICES];                       /**< Indica se o slot i está ocupado por um vértice válido */
    int adj[MAX_VERTICES][MAX_VERTICES];            /**< Matriz de adjacência (0 = sem relação, 1 = relacionados) */
    int num_vertices;                               /**< Número atual de vértices ativos no grafo */
    int num_arestas;                                /**< Número atual de arestas (relações) no grafo */
} Grafo;

/**
 * @brief Inicializa o grafo sem vértices e sem arestas.
 * @param grafo Ponteiro para a estrutura Grafo.
 */
void grafo_inicializar(Grafo *grafo);

/**
 * @brief Obtém o índice compacto interno correspondente a um ID de livro.
 * @param grafo Ponteiro constante para o grafo.
 * @param id_livro ID do livro a procurar.
 * @return Índice no intervalo [0, MAX_VERTICES-1] ou -1 se o ID não estiver no grafo.
 */
int grafo_obter_indice(const Grafo *grafo, int id_livro);

/**
 * @brief Regista um novo vértice (ID de livro) no grafo num slot disponível.
 * @param grafo Ponteiro para o grafo.
 * @param id_livro ID positivo e único do livro.
 * @return true se o vértice foi registado (ou já existia), false se atingir MAX_VERTICES ou ID inválido.
 */
bool grafo_adicionar_vertice(Grafo *grafo, int id_livro);

/**
 * @brief Remove um vértice (ID de livro) e todas as arestas incidentes nele.
 *        Garante a integridade quando um livro é removido da BST.
 * @param grafo Ponteiro para o grafo.
 * @param id_livro ID do livro a remover do grafo.
 * @return true se o vértice existia e foi removido, false caso contrário.
 */
bool grafo_remover_vertice(Grafo *grafo, int id_livro);

/**
 * @brief Adiciona uma relação simétrica (aresta não direcionada) entre dois livros pelo ID.
 *        Não permite laços (id_origem == id_destino).
 * @param grafo Ponteiro para o grafo.
 * @param id_origem ID do primeiro livro.
 * @param id_destino ID do segundo livro.
 * @return true se a relação foi criada com sucesso, false se inválida ou já existente.
 */
bool grafo_adicionar_aresta(Grafo *grafo, int id_origem, int id_destino);

/**
 * @brief Remove a relação (aresta não direcionada) entre dois livros pelo ID.
 * @param grafo Ponteiro para o grafo.
 * @param id_origem ID do primeiro livro.
 * @param id_destino ID do segundo livro.
 * @return true se a aresta existia e foi removida, false caso contrário.
 */
bool grafo_remover_aresta(Grafo *grafo, int id_origem, int id_destino);

/**
 * @brief Verifica se existe relação direta (aresta) entre dois livros.
 * @param grafo Ponteiro constante para o grafo.
 * @param id_origem ID do primeiro livro.
 * @param id_destino ID do segundo livro.
 * @return true se os livros estão diretamente relacionados, false caso contrário.
 */
bool grafo_existe_aresta(const Grafo *grafo, int id_origem, int id_destino);

/**
 * @brief Preenche uma Lista encadeada com os IDs dos vizinhos diretos de um livro.
 * @param grafo Ponteiro constante para o grafo.
 * @param id_livro ID do livro a consultar.
 * @param vizinhos Ponteiro para uma Lista previamente inicializada onde os vizinhos serão inseridos no fim.
 * @return true se o vértice existe no grafo, false caso contrário.
 */
bool grafo_obter_vizinhos(const Grafo *grafo, int id_livro, Lista *vizinhos);

/**
 * @brief Executa a Busca em Largura (BFS) a partir de um livro de origem, utilizando
 *        a Fila dinâmica para controlo de fronteira e a Lista encadeada para registar
 *        a ordem de visitação e a distância (grau de separação).
 * @param grafo Ponteiro constante para o grafo.
 * @param id_origem ID do livro inicial da busca.
 * @param visitados_ordem Ponteiro para Lista inicializada que receberá os IDs visitados em ordem de camada.
 * @return true se a BFS foi executada com sucesso, false se o vértice de origem não existir.
 */
bool grafo_bfs(const Grafo *grafo, int id_origem, Lista *visitados_ordem);

/**
 * @brief Exibe as conexões do grafo consultando opcionalmente a BST para mostrar os títulos.
 * @param grafo Ponteiro constante para o grafo.
 * @param raiz_bst Ponteiro para a raiz da BST (pode ser NULL para exibir apenas IDs).
 */
void grafo_exibir(const Grafo *grafo, NoBST *raiz_bst);

/**
 * @brief Reinicia o estado do grafo libertando recursos lógicos.
 * @param grafo Ponteiro para o grafo.
 */
void grafo_liberar(Grafo *grafo);

#endif /* GRAFO_H */


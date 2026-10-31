/**
 * @file grafo.h
 * @brief Grafo não direcionado e não ponderado para relações entre livros (Requisito R08 e Seção 5).
 *
 * Suporta no mínimo 100 vértices simultâneos (MAX_VERTICES = 100) e relaciona entidades
 * exclusivamente pelo ID arbitrário (int positivo > 0), mapeando ID <-> índice compacto.
 * Suporta Busca em Largura (BFS) em ordem crescente de ID e verificação de caminho.
 */

#ifndef GRAFO_H
#define GRAFO_H

#include <stdbool.h>
#include "lista.h"
#include "fila.h"
#include "bst.h"

/** Capacidade máxima de vértices simultâneos no grafo (Requisito R08) */
#define MAX_VERTICES 100

/**
 * @brief Códigos de status retornados pelas operações do Grafo (Requisito R08).
 */
typedef enum {
    GRAFO_SUCESSO = 0,                      /**< Operação concluída com sucesso */
    GRAFO_ERRO_ID_INVALIDO,                 /**< ID inválido (deve ser inteiro positivo > 0) */
    GRAFO_ERRO_VERTICE_DUPLICADO,           /**< Vértice com o ID informado já existe no grafo */
    GRAFO_ERRO_VERTICE_NAO_ENCONTRADO,      /**< Vértice não encontrado no grafo */
    GRAFO_ERRO_CAPACIDADE_MAXIMA,           /**< Limite máximo de 100 vértices simultâneos atingido */
    GRAFO_ERRO_LACO,                        /**< Tentativa inválida de criar laço (A, A) */
    GRAFO_ERRO_ARESTA_DUPLICADA,            /**< Relação simétrica (A,B) ou (B,A) já existente */
    GRAFO_ERRO_ARESTA_NAO_ENCONTRADA        /**< Relação entre os dois vértices não encontrada */
} StatusGrafo;

/**
 * @brief Estrutura do Grafo não direcionado e não ponderado com mapeamento ID -> índice compacto.
 */
typedef struct {
    int ids_vertices[MAX_VERTICES];         /**< Mapeamento índice compacto [0..99] -> ID do livro */
    bool ativo[MAX_VERTICES];               /**< Indica se o slot i está ocupado por um vértice ativo */
    int adj[MAX_VERTICES][MAX_VERTICES];    /**< Matriz de adjacência (0 = sem aresta, 1 = conectados) */
    int num_vertices;                       /**< Quantidade atual de vértices ativos no grafo */
    int num_arestas;                        /**< Quantidade atual de arestas (relações) no grafo */
} Grafo;

/**
 * @brief Inicializa o grafo com 0 vértices e 0 arestas.
 * @param grafo Ponteiro para a estrutura Grafo.
 */
void grafo_inicializar(Grafo *grafo);

/**
 * @brief Retorna uma mensagem legível em português correspondente ao StatusGrafo.
 * @param status Código de status do grafo.
 * @return String descritiva constante.
 */
const char* grafo_status_mensagem(StatusGrafo status);

/**
 * @brief Obtém o índice interno compacto [0..MAX_VERTICES-1] correspondente a um ID de livro.
 * @param grafo Ponteiro constante para o grafo.
 * @param id_livro ID arbitrário do livro a pesquisar.
 * @return Índice [0..MAX_VERTICES-1] se ativo, ou -1 se o ID não existir no grafo.
 */
int grafo_obter_indice(const Grafo *grafo, int id_livro);

/**
 * @brief Regista um novo vértice pelo ID no grafo em um slot disponível (Requisito R08).
 * @param grafo Ponteiro para o grafo.
 * @param id_livro ID positivo (> 0) e único do livro.
 * @return GRAFO_SUCESSO se inserido, ou código de erro (duplicado, inválido, capacidade cheia).
 */
StatusGrafo grafo_adicionar_vertice(Grafo *grafo, int id_livro);

/**
 * @brief Remove um vértice pelo ID e todas as arestas incidentes a ele, liberando o slot
 *        para permitir novos cadastros até a capacidade de 100 vértices ativos.
 * @param grafo Ponteiro para o grafo.
 * @param id_livro ID do livro a remover.
 * @return GRAFO_SUCESSO se removido, ou código de erro (não encontrado, inválido).
 */
StatusGrafo grafo_remover_vertice(Grafo *grafo, int id_livro);

/**
 * @brief Adiciona uma relação simétrica (aresta não direcionada) entre dois livros pelo ID.
 *        Rejeita laços (A,A), arestas duplicadas ((A,B) ou (B,A)) e IDs inexistentes.
 * @param grafo Ponteiro para o grafo.
 * @param id_origem ID do primeiro livro.
 * @param id_destino ID do segundo livro.
 * @return GRAFO_SUCESSO se criada com sucesso, ou código de erro.
 */
StatusGrafo grafo_adicionar_aresta(Grafo *grafo, int id_origem, int id_destino);

/**
 * @brief Remove a relação simétrica entre dois livros pelo ID.
 *        Rejeita IDs inexistentes, laços ou arestas não existentes.
 * @param grafo Ponteiro para o grafo.
 * @param id_origem ID do primeiro livro.
 * @param id_destino ID do segundo livro.
 * @return GRAFO_SUCESSO se removida com sucesso, ou código de erro.
 */
StatusGrafo grafo_remover_aresta(Grafo *grafo, int id_origem, int id_destino);

/**
 * @brief Verifica se existe relação direta (aresta) entre dois livros existentes.
 * @param grafo Ponteiro constante para o grafo.
 * @param id_origem ID do primeiro livro.
 * @param id_destino ID do segundo livro.
 * @return true se conectados diretamente, false caso contrário.
 */
bool grafo_existe_aresta(const Grafo *grafo, int id_origem, int id_destino);

/**
 * @brief Preenche uma Lista encadeada com os IDs dos vizinhos diretos em ordem crescente de ID.
 * @param grafo Ponteiro constante para o grafo.
 * @param id_livro ID do livro a consultar.
 * @param vizinhos Ponteiro para Lista onde os vizinhos serão inseridos no fim.
 * @return true se o vértice existe no grafo, false caso contrário.
 */
bool grafo_obter_vizinhos(const Grafo *grafo, int id_livro, Lista *vizinhos);

/**
 * @brief Executa a Busca em Largura (BFS) a partir de uma origem existente (Seção 6 e R08).
 *        - Utiliza a Fila dinâmica própria (fila.h) para controle da exploração.
 *        - Guarda os IDs alcançáveis (incluindo a origem) na Lista encadeada própria (lista.h)
 *          usando inserção no fim, preservando a ordem exata de descoberta.
 *        - REGRA CRÍTICA: marca cada vértice como visitado no momento em que ele ENTRA na fila (enqueue)
 *          e percorre os vizinhos de cada vértice estritamente em ORDEM CRESCENTE DE ID.
 *        - Origem isolada produz na lista apenas o seu próprio ID.
 * @param grafo Ponteiro constante para o grafo.
 * @param id_origem ID do livro de partida da busca.
 * @param visitados_ordem Ponteiro para Lista que receberá a sequência de visitação da BFS.
 * @return GRAFO_SUCESSO se executada, ou código de erro se o vértice não existir.
 */
StatusGrafo grafo_bfs(const Grafo *grafo, int id_origem, Lista *visitados_ordem);

/**
 * @brief Verifica se existe caminho entre dois IDs existentes (origem e destino) via BFS (Seção 6).
 *        Se qualquer um dos IDs não existir no grafo, rejeita com GRAFO_ERRO_VERTICE_NAO_ENCONTRADO.
 * @param grafo Ponteiro constante para o grafo.
 * @param id_origem ID do livro de partida.
 * @param id_destino ID do livro de destino.
 * @param existe Ponteiro booleano onde será gravado true se existir caminho, false caso contrário.
 * @return GRAFO_SUCESSO se ambos existirem e a verificação ocorrer, ou código de erro.
 */
StatusGrafo grafo_existe_caminho(const Grafo *grafo, int id_origem, int id_destino, bool *existe);

/**
 * @brief Cadastra um livro no sistema de forma atômica (BST + Grafo com Rollback - Seção 4).
 *        Insere o livro na BST e cria o vértice de mesmo ID no grafo. Se a criação do vértice
 *        falhar (ex: capacidade de 100 atingida), desfaz imediatamente a inserção na BST (rollback).
 * @param catalogo Ponteiro para a raiz da BST.
 * @param grafo Ponteiro para o grafo.
 * @param novo_livro Dados do livro a cadastrar.
 * @return GRAFO_SUCESSO se cadastrado em ambas as estruturas, ou código de erro com rollback garantido.
 */
StatusGrafo sistema_cadastrar_livro(NoBST **catalogo, Grafo *grafo, Livro novo_livro);

/**
 * @brief Remove um livro do sistema de forma atômica (BST + Grafo - Seção 4).
 *        Exclui da BST e remove o vértice e todas as arestas incidentes no grafo.
 *        Se o ID não existir, rejeita sem alterar nenhuma das estruturas.
 * @param catalogo Ponteiro para a raiz da BST.
 * @param grafo Ponteiro para o grafo.
 * @param id ID do livro a remover.
 * @param livro_removido Ponteiro opcional onde será guardada cópia dos dados removidos.
 * @return GRAFO_SUCESSO se removido de ambas as estruturas, ou código de erro.
 */
StatusGrafo sistema_remover_livro(NoBST **catalogo, Grafo *grafo, int id, Livro *livro_removido);

/**
 * @brief Exibe visualmente o grafo na consola listando as conexões em ordem crescente de ID.
 * @param grafo Ponteiro constante para o grafo.
 * @param raiz_bst Ponteiro para a raiz da BST (pode ser NULL para exibir apenas IDs).
 */
void grafo_exibir(const Grafo *grafo, NoBST *raiz_bst);

/**
 * @brief Reinicia o estado do grafo libertando todos os recursos.
 * @param grafo Ponteiro para o grafo.
 */
void grafo_liberar(Grafo *grafo);

#endif /* GRAFO_H */

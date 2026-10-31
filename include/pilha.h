/**
 * @file pilha.h
 * @brief Pilha dinâmica encadeada (LIFO) para histórico de alterações de estado (Requisito R04).
 *
 * Regista apenas as operações concluídas com sucesso que modificam o estado do sistema
 * (cadastro de livro, remoção de livro, adição de conexão e remoção de conexão),
 * guardando o tipo de ação e os IDs envolvidos. Limitada apenas pela memória disponível.
 */

#ifndef PILHA_H
#define PILHA_H

#include <stdbool.h>
#include "bst.h"

/** Quantidade máxima de ações recentes exibidas na consulta não destrutiva (R04) */
#define MAX_HISTORICO_EXIBICAO 5

/**
 * @brief Tipos de ações que alteram o estado do sistema e são registadas na Pilha.
 */
typedef enum {
    ACAO_INSERIR_LIVRO = 1,         /**< Cadastro de um novo livro na BST */
    ACAO_REMOVER_LIVRO = 2,         /**< Remoção de um livro da BST */
    ACAO_ADICIONAR_RELACAO = 3,     /**< Adição de conexão entre dois livros no Grafo */
    ACAO_REMOVER_RELACAO = 4        /**< Remoção de conexão entre dois livros no Grafo */
} TipoAcao;

/**
 * @brief Registo de uma operação concluída com sucesso no histórico de alterações.
 */
typedef struct {
    TipoAcao tipo;                  /**< Tipo de operação realizada */
    int id_principal;               /**< ID do livro principal afetado (ou origem da conexão) */
    int id_secundario;              /**< ID do segundo livro (em operações de conexão; 0 se não aplicável) */
    Livro snapshot_livro;           /**< Cópia opcional dos dados do livro envolvido */
    char descricao[120];            /**< Resumo textual da operação realizada */
} RegistoHistorico;

/**
 * @brief Nó da pilha dinâmica encadeada.
 */
typedef struct NoPilha {
    RegistoHistorico registo;       /**< Dados da alteração de estado */
    struct NoPilha *prox;           /**< Ponteiro para o elemento abaixo na pilha */
} NoPilha;

/**
 * @brief Estrutura descritora da Pilha dinâmica (LIFO).
 */
typedef struct {
    NoPilha *topo;                  /**< Ponteiro para o elemento no topo da pilha */
    int tamanho;                    /**< Total de operações registadas na pilha */
} Pilha;

/**
 * @brief Inicializa a pilha de histórico vazia.
 * @param pilha Ponteiro para a estrutura Pilha.
 */
void pilha_inicializar(Pilha *pilha);

/**
 * @brief Verifica se a pilha está vazia.
 * @param pilha Ponteiro constante para a pilha.
 * @return true se estiver vazia, false caso contrário.
 */
bool pilha_vazia(const Pilha *pilha);

/**
 * @brief Empilha um novo registo de alteração de estado no topo (push).
 * @param pilha Ponteiro para a pilha.
 * @param tipo Tipo da ação realizada com sucesso.
 * @param id_principal ID do livro principal envolvido.
 * @param id_secundario ID do segundo livro (0 para operações de um único livro).
 * @param livro Ponteiro opcional para o Livro envolvido (pode ser NULL).
 * @param descricao Texto descritivo opcional da operação (pode ser NULL).
 * @return true se o registo foi empilhado com sucesso, false em erro de memória.
 */
bool pilha_empilhar(Pilha *pilha, TipoAcao tipo, int id_principal, int id_secundario,
                    const Livro *livro, const char *descricao);

/**
 * @brief Desempilha o registo mais recente do topo da pilha (pop).
 * @param pilha Ponteiro para a pilha.
 * @param registo_removido Ponteiro opcional onde será copiado o registo retirado do topo.
 * @return true se desempilhou com sucesso, false se a pilha estava vazia.
 */
bool pilha_desempilhar(Pilha *pilha, RegistoHistorico *registo_removido);

/**
 * @brief Consulta o registo no topo da pilha sem o remover (peek).
 * @param pilha Ponteiro constante para a pilha.
 * @param registo_topo Ponteiro onde será copiado o registo do topo.
 * @return true se a consulta teve sucesso, false se a pilha estava vazia.
 */
bool pilha_topo(const Pilha *pilha, RegistoHistorico *registo_topo);

/**
 * @brief Consulta e imprime de forma não destrutiva até 5 ações mais recentes
 *        (do topo para a base, sem remover elementos da pilha). Se houver menos
 *        de 5 ações, exibe todas as disponíveis; se estiver vazia, informa o utilizador.
 * @param pilha Ponteiro constante para a pilha.
 */
void pilha_exibir_historico(const Pilha *pilha);

/**
 * @brief Retorna uma string legível correspondente ao TipoAcao.
 * @param tipo Enumerador do tipo de ação.
 * @return String constante com o nome da ação.
 */
const char* pilha_nome_acao(TipoAcao tipo);

/**
 * @brief Liberta toda a memória alocada pelos nós da pilha.
 * @param pilha Ponteiro para a pilha.
 */
void pilha_liberar(Pilha *pilha);

#endif /* PILHA_H */

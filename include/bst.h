/**
 * @file bst.h
 * @brief Árvore Binária de Busca (BST) para indexação de livros pelo ID.
 *
 * Define a estrutura de dados do registo Livro, o nó da BST e os protótipos
 * das operações de inserção, busca, remoção, percursos e exportação.
 */

#ifndef BST_H
#define BST_H

#include <stdbool.h>

/** Tamanho máximo dos campos textuais (79 caracteres úteis + '\0') */
#define TAM_TEXTO 80

/** Limites de validação do ano de publicação */
#define ANO_MINIMO 1
#define ANO_MAXIMO 2026

/**
 * @brief Estrutura que representa o registo de um livro no catálogo.
 */
typedef struct {
    int id;                         /**< Identificador único e positivo (> 0) */
    char titulo[TAM_TEXTO];         /**< Título da obra (até 79 caracteres úteis) */
    char autor[TAM_TEXTO];          /**< Autor da obra (até 79 caracteres úteis) */
    int ano_publicacao;             /**< Ano de publicação (entre 1 e 2026) */
} Livro;

/**
 * @brief Nó da Árvore Binária de Busca (BST) indexada pelo ID do livro.
 */
typedef struct NoBST {
    Livro livro;                    /**< Dados do livro armazenado no nó */
    struct NoBST *esq;              /**< Ponteiro para a subárvore esquerda (IDs menores) */
    struct NoBST *dir;              /**< Ponteiro para a subárvore direita (IDs maiores) */
} NoBST;

/**
 * @brief Inicializa uma árvore binária de busca vazia.
 * @return Ponteiro NULL representando a árvore vazia.
 */
NoBST* bst_criar(void);

/**
 * @brief Valida os campos de um registo de livro segundo as regras do domínio.
 * @param livro Estrutura Livro a validar.
 * @return true se todos os campos forem válidos, false caso contrário.
 */
bool livro_validar(Livro livro);

/**
 * @brief Insere um novo livro na BST mantendo a ordenação pelo ID.
 *        Rejeita IDs não positivos, duplicados ou com dados inválidos.
 * @param raiz Ponteiro para o ponteiro da raiz da BST.
 * @param novo_livro Dados do livro a inserir.
 * @return true se a inserção foi concluída com sucesso, false se falhou.
 */
bool bst_inserir(NoBST **raiz, Livro novo_livro);

/**
 * @brief Procura um livro na BST a partir do seu ID.
 * @param raiz Ponteiro para a raiz da BST.
 * @param id Identificador único do livro procurado.
 * @return Ponteiro para o nó encontrado ou NULL se não existir.
 */
NoBST* bst_buscar(NoBST *raiz, int id);

/**
 * @brief Remove um livro da BST pelo seu ID (tratando nós folha, com 1 ou 2 filhos).
 * @param raiz Ponteiro para o ponteiro da raiz da BST.
 * @param id Identificador do livro a remover.
 * @param livro_removido Ponteiro opcional (pode ser NULL) para guardar os dados do livro removido.
 * @return true se o livro foi encontrado e removido, false caso contrário.
 */
bool bst_remover(NoBST **raiz, int id, Livro *livro_removido);

/**
 * @brief Percorre a BST em-ordem (In-Order: Esquerda, Raiz, Direita), exibindo os livros por ID crescente.
 * @param raiz Ponteiro para a raiz da BST.
 */
void bst_percorrer_em_ordem(NoBST *raiz);

/**
 * @brief Percorre a BST em pré-ordem (Pre-Order: Raiz, Esquerda, Direita).
 * @param raiz Ponteiro para a raiz da BST.
 */
void bst_percorrer_pre_ordem(NoBST *raiz);

/**
 * @brief Percorre a BST em pós-ordem (Post-Order: Esquerda, Direita, Raiz).
 * @param raiz Ponteiro para a raiz da BST.
 */
void bst_percorrer_pos_ordem(NoBST *raiz);

/**
 * @brief Conta o número total de livros registados na BST.
 * @param raiz Ponteiro para a raiz da BST.
 * @return Quantidade total de nós na árvore.
 */
int bst_contar_nos(NoBST *raiz);

/**
 * @brief Calcula a altura atual da BST.
 * @param raiz Ponteiro para a raiz da BST.
 * @return Altura da árvore (-1 para árvore vazia, 0 para apenas raiz).
 */
int bst_altura(NoBST *raiz);

/**
 * @brief Copia todos os livros da BST (em-ordem) para um vetor prealocado.
 * @param raiz Ponteiro para a raiz da BST.
 * @param vetor Array de Livro onde os registos serão copiados.
 * @param indice Ponteiro para o índice atual de escrita no vetor (iniciar em 0).
 */
void bst_exportar_vetor(NoBST *raiz, Livro *vetor, int *indice);

/**
 * @brief Exibe os dados formatados de um único livro no ecrã.
 * @param livro Ponteiro constante para o livro a exibir.
 */
void livro_exibir(const Livro *livro);

/**
 * @brief Liberta toda a memória alocada dinamicamente pelos nós da BST.
 * @param raiz Ponteiro para a raiz da BST.
 */
void bst_liberar(NoBST *raiz);

#endif /* BST_H */


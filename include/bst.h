/**
 * @file bst.h
 * @brief Árvore Binária de Busca (BST) para indexação de livros pelo ID (Requisito R07).
 *
 * Define a estrutura de dados do registo Livro, o nó da BST e os protótipos
 * das operações de inserção, busca, remoção completa (3 casos), percursos,
 * consultas de menor/maior ID e exportação independente para ordenação.
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
 * @brief Códigos de status retornados pelas operações da BST (Requisito R07).
 */
typedef enum {
    BST_SUCESSO = 0,                /**< Operação concluída com sucesso */
    BST_ERRO_ID_INVALIDO,           /**< ID inválido (deve ser inteiro positivo > 0) */
    BST_ERRO_DUPLICADO,             /**< Tentativa de inserir ID já existente na árvore */
    BST_ERRO_MEMORIA,               /**< Falha de alocação dinâmica de memória */
    BST_ERRO_NAO_ENCONTRADO,        /**< Registro não localizado na árvore */
    BST_ERRO_DADOS_INVALIDOS        /**< Registro com campos inválidos (título/autor/ano) */
} StatusBST;

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
 * @brief Retorna uma mensagem legível em português correspondente ao StatusBST.
 * @param status Código de status retornado pelas operações da BST.
 * @return String constante descritiva.
 */
const char* bst_status_mensagem(StatusBST status);

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
 * @return BST_SUCESSO em caso de sucesso, ou código de erro específico.
 */
StatusBST bst_inserir(NoBST **raiz, Livro novo_livro);

/**
 * @brief Procura um livro na BST a partir do seu ID guiado pela propriedade da BST (O(h)).
 * @param raiz Ponteiro para a raiz da BST.
 * @param id Identificador único do livro procurado.
 * @return Ponteiro para o nó encontrado ou NULL se não existir.
 */
NoBST* bst_buscar(NoBST *raiz, int id);

/**
 * @brief Remove um livro da BST pelo seu ID tratando os 3 casos clássicos:
 *        - Caso 1: Nó folha (0 filhos)
 *        - Caso 2: Nó com 1 filho
 *        - Caso 3: Nó com 2 filhos (substituição pelo sucessor em-ordem)
 *        Rejeita remoção de ID inexistente sem alterar a árvore.
 * @param raiz Ponteiro para o ponteiro da raiz da BST.
 * @param id Identificador do livro a remover.
 * @param livro_removido Ponteiro opcional (pode ser NULL) para guardar cópia dos dados removidos.
 * @return BST_SUCESSO se removido com sucesso, ou código de erro (ex: BST_ERRO_NAO_ENCONTRADO).
 */
StatusBST bst_remover(NoBST **raiz, int id, Livro *livro_removido);

/**
 * @brief Localiza o nó com o menor ID na subárvore (mais à esquerda).
 * @param raiz Ponteiro para a raiz da subárvore.
 * @return Ponteiro para o nó de menor ID, ou NULL se a árvore estiver vazia.
 */
NoBST* bst_minimo(NoBST *raiz);

/**
 * @brief Localiza o nó com o maior ID na subárvore (mais à direita).
 * @param raiz Ponteiro para a raiz da subárvore.
 * @return Ponteiro para o nó de maior ID, ou NULL se a árvore estiver vazia.
 */
NoBST* bst_maximo(NoBST *raiz);

/**
 * @brief Consulta o menor ID presente na BST.
 * @param raiz Ponteiro para a raiz da BST.
 * @param menor_id Ponteiro onde será gravado o menor ID encontrado.
 * @return true se a árvore não estiver vazia e o ID foi obtido, false caso contrário.
 */
bool bst_obter_menor_id(NoBST *raiz, int *menor_id);

/**
 * @brief Consulta o maior ID presente na BST.
 * @param raiz Ponteiro para a raiz da BST.
 * @param maior_id Ponteiro onde será gravado o maior ID encontrado.
 * @return true se a árvore não estiver vazia e o ID foi obtido, false caso contrário.
 */
bool bst_obter_maior_id(NoBST *raiz, int *maior_id);

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
 * @brief Calcula a altura atual da BST (-1 para vazia, 0 para apenas raiz).
 * @param raiz Ponteiro para a raiz da BST.
 * @return Altura da árvore.
 */
int bst_altura(NoBST *raiz);

/**
 * @brief Copia todos os registros da BST para um vetor auxiliar de cópias independentes dos valores
 *        (não ponteiros para nós da árvore), permitindo que sejam ordenados por título
 *        sem alterar a BST original.
 * @param raiz Ponteiro para a raiz da BST.
 * @param vetor Array de estruturas Livro onde os registos serão copiados.
 * @param indice Ponteiro para o contador de escrita no vetor (iniciar em 0).
 */
void bst_exportar_vetor(NoBST *raiz, Livro *vetor, int *indice);

/**
 * @brief Exibe os dados formatados de um único livro no ecrã.
 * @param livro Ponteiro constante para o livro a exibir.
 */
void livro_exibir(const Livro *livro);

/**
 * @brief Liberta toda a memória alocada dinamicamente pelos nós da BST (pós-ordem).
 * @param raiz Ponteiro para a raiz da BST.
 */
void bst_liberar(NoBST *raiz);

#endif /* BST_H */

/**
 * @file bst.c
 * @brief Implementação (stubs iniciais - Etapa 1) da Árvore Binária de Busca (BST).
 */

#include <stdio.h>
#include <stdlib.h>
#include "bst.h"

NoBST* bst_criar(void) {
    return NULL;
}

bool livro_validar(Livro livro) {
    (void)livro;
    /* Stub: será implementado na Etapa 2 */
    return false;
}

bool bst_inserir(NoBST **raiz, Livro novo_livro) {
    (void)raiz;
    (void)novo_livro;
    /* Stub: será implementado na Etapa 2 */
    return false;
}

NoBST* bst_buscar(NoBST *raiz, int id) {
    (void)raiz;
    (void)id;
    /* Stub: será implementado na Etapa 2 */
    return NULL;
}

bool bst_remover(NoBST **raiz, int id, Livro *livro_removido) {
    (void)raiz;
    (void)id;
    (void)livro_removido;
    /* Stub: será implementado na Etapa 2 */
    return false;
}

void bst_percorrer_em_ordem(NoBST *raiz) {
    (void)raiz;
    /* Stub: será implementado na Etapa 2 */
}

void bst_percorrer_pre_ordem(NoBST *raiz) {
    (void)raiz;
    /* Stub: será implementado na Etapa 2 */
}

void bst_percorrer_pos_ordem(NoBST *raiz) {
    (void)raiz;
    /* Stub: será implementado na Etapa 2 */
}

int bst_contar_nos(NoBST *raiz) {
    (void)raiz;
    /* Stub: será implementado na Etapa 2 */
    return 0;
}

int bst_altura(NoBST *raiz) {
    (void)raiz;
    /* Stub: será implementado na Etapa 2 */
    return -1;
}

void bst_exportar_vetor(NoBST *raiz, Livro *vetor, int *indice) {
    (void)raiz;
    (void)vetor;
    (void)indice;
    /* Stub: será implementado na Etapa 2 */
}

void livro_exibir(const Livro *livro) {
    (void)livro;
    /* Stub: será implementado na Etapa 2 */
}

void bst_liberar(NoBST *raiz) {
    (void)raiz;
    /* Stub: será implementado na Etapa 2 */
}


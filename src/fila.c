/**
 * @file fila.c
 * @brief Implementação da Fila dinâmica encadeada própria (FIFO) para apoio à BFS (Requisito R05).
 */

#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

void fila_inicializar(Fila *fila) {
    if (fila != NULL) {
        fila->frente = NULL;
        fila->tras = NULL;
        fila->tamanho = 0;
    }
}

bool fila_vazia(const Fila *fila) {
    return (fila == NULL || fila->frente == NULL);
}

bool fila_enfileirar(Fila *fila, int id_livro) {
    NoFila *novo;

    if (fila == NULL) {
        return false;
    }

    novo = (NoFila *)malloc(sizeof(NoFila));
    if (novo == NULL) {
        return false;
    }

    novo->id_livro = id_livro;
    novo->prox = NULL;

    if (fila->frente == NULL) {
        fila->frente = novo;
        fila->tras = novo;
    } else {
        fila->tras->prox = novo;
        fila->tras = novo;
    }

    fila->tamanho++;
    return true;
}

bool fila_desenfileirar(Fila *fila, int *id_removido) {
    NoFila *removido;

    if (fila_vazia(fila)) {
        return false;
    }

    removido = fila->frente;
    if (id_removido != NULL) {
        *id_removido = removido->id_livro;
    }

    fila->frente = removido->prox;
    if (fila->frente == NULL) {
        fila->tras = NULL;
    }

    free(removido);
    fila->tamanho--;
    return true;
}

bool fila_frente(const Fila *fila, int *id_frente) {
    if (fila_vazia(fila)) {
        return false;
    }

    if (id_frente != NULL) {
        *id_frente = fila->frente->id_livro;
    }
    return true;
}

void fila_liberar(Fila *fila) {
    NoFila *atual;
    NoFila *prox;

    if (fila == NULL) {
        return;
    }

    atual = fila->frente;
    while (atual != NULL) {
        prox = atual->prox;
        free(atual);
        atual = prox;
    }

    fila->frente = NULL;
    fila->tras = NULL;
    fila->tamanho = 0;
}

/**
 * @file fila.c
 * @brief Implementação (stubs iniciais - Etapa 1) da Fila dinâmica encadeada.
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
    (void)fila;
    (void)id_livro;
    /* Stub: será implementado nas próximas etapas */
    return false;
}

bool fila_desenfileirar(Fila *fila, int *id_removido) {
    (void)fila;
    (void)id_removido;
    /* Stub: será implementado nas próximas etapas */
    return false;
}

bool fila_frente(const Fila *fila, int *id_frente) {
    (void)fila;
    (void)id_frente;
    /* Stub: será implementado nas próximas etapas */
    return false;
}

void fila_liberar(Fila *fila) {
    (void)fila;
    /* Stub: será implementado nas próximas etapas */
}


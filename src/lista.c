/**
 * @file lista.c
 * @brief Implementação (stubs iniciais - Etapa 1) da Lista simplesmente encadeada.
 */

#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

void lista_inicializar(Lista *lista) {
    if (lista != NULL) {
        lista->inicio = NULL;
        lista->fim = NULL;
        lista->tamanho = 0;
    }
}

bool lista_vazia(const Lista *lista) {
    return (lista == NULL || lista->inicio == NULL);
}

bool lista_inserir_fim(Lista *lista, int id_livro, int distancia) {
    (void)lista;
    (void)id_livro;
    (void)distancia;
    /* Stub: será implementado nas próximas etapas */
    return false;
}

bool lista_remover(Lista *lista, int id_livro) {
    (void)lista;
    (void)id_livro;
    /* Stub: será implementado nas próximas etapas */
    return false;
}

bool lista_contem(const Lista *lista, int id_livro) {
    (void)lista;
    (void)id_livro;
    /* Stub: será implementado nas próximas etapas */
    return false;
}

void lista_exibir(const Lista *lista) {
    (void)lista;
    /* Stub: será implementado nas próximas etapas */
}

void lista_liberar(Lista *lista) {
    (void)lista;
    /* Stub: será implementado nas próximas etapas */
}


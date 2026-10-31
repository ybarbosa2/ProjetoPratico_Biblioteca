/**
 * @file lista.c
 * @brief Implementação da Lista simplesmente encadeada própria com inserção no fim (Requisito R03).
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
    NoLista *novo;

    if (lista == NULL) {
        return false;
    }

    novo = (NoLista *)malloc(sizeof(NoLista));
    if (novo == NULL) {
        return false;
    }

    novo->id_livro = id_livro;
    novo->distancia = distancia;
    novo->prox = NULL;

    if (lista->inicio == NULL) {
        lista->inicio = novo;
        lista->fim = novo;
    } else {
        lista->fim->prox = novo;
        lista->fim = novo;
    }

    lista->tamanho++;
    return true;
}

bool lista_remover(Lista *lista, int id_livro) {
    NoLista *atual;
    NoLista *anterior = NULL;

    if (lista_vazia(lista)) {
        return false;
    }

    atual = lista->inicio;
    while (atual != NULL && atual->id_livro != id_livro) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual == NULL) {
        return false;
    }

    if (anterior == NULL) {
        lista->inicio = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }

    if (atual == lista->fim) {
        lista->fim = anterior;
    }

    free(atual);
    lista->tamanho--;
    return true;
}

bool lista_contem(const Lista *lista, int id_livro) {
    const NoLista *atual;

    if (lista_vazia(lista)) {
        return false;
    }

    atual = lista->inicio;
    while (atual != NULL) {
        if (atual->id_livro == id_livro) {
            return true;
        }
        atual = atual->prox;
    }
    return false;
}

void lista_exibir(const Lista *lista) {
    const NoLista *atual;
    int pos = 1;

    if (lista_vazia(lista)) {
        printf("  [Lista vazia]\n");
        return;
    }

    atual = lista->inicio;
    while (atual != NULL) {
        printf("  %d. Livro ID: %d (Nivel/Distancia: %d)\n",
               pos, atual->id_livro, atual->distancia);
        atual = atual->prox;
        pos++;
    }
}

void lista_liberar(Lista *lista) {
    NoLista *atual;
    NoLista *prox;

    if (lista == NULL) {
        return;
    }

    atual = lista->inicio;
    while (atual != NULL) {
        prox = atual->prox;
        free(atual);
        atual = prox;
    }

    lista->inicio = NULL;
    lista->fim = NULL;
    lista->tamanho = 0;
}

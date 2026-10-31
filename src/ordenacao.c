/**
 * @file ordenacao.c
 * @brief Implementação própria de Merge Sort e Quick Sort sobre cópias de Livro (Requisito R06).
 *
 * Não utiliza qsort da biblioteca padrão. Ordena de forma crescente pelo campo
 * "titulo" com strcmp() (sensível a maiúsculas/minúsculas) e desempata por ID crescente.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ordenacao.h"

/**
 * @brief Compara dois IDs inteiros de forma segura (sem risco de overflow).
 */
static int comparar_ids(int id_a, int id_b) {
    if (id_a < id_b) {
        return -1;
    }
    if (id_a > id_b) {
        return 1;
    }
    return 0;
}

int livro_comparar(const Livro *a, const Livro *b, CriterioOrdenacao criterio) {
    int cmp;

    if (a == NULL || b == NULL) {
        return 0;
    }

    switch (criterio) {
        case ORDENAR_POR_AUTOR:
            cmp = strcmp(a->autor, b->autor);
            if (cmp != 0) {
                return cmp;
            }
            return comparar_ids(a->id, b->id);

        case ORDENAR_POR_ANO:
            if (a->ano_publicacao != b->ano_publicacao) {
                return (a->ano_publicacao < b->ano_publicacao) ? -1 : 1;
            }
            return comparar_ids(a->id, b->id);

        case ORDENAR_POR_ID:
            return comparar_ids(a->id, b->id);

        case ORDENAR_POR_TITULO:
        default:
            /* Requisito R06: ordem crescente por titulo usando strcmp() e desempate por ID crescente */
            cmp = strcmp(a->titulo, b->titulo);
            if (cmp != 0) {
                return cmp;
            }
            return comparar_ids(a->id, b->id);
    }
}

/* ==========================================================================
 * MERGE SORT PRÓPRIO (Estável, O(n log n))
 * ========================================================================== */

/**
 * @brief Intercala dois subvetores ordenados [esq..meio] e [meio+1..dir].
 */
static void intercalar(Livro *vetor, Livro *aux, int esq, int meio, int dir, CriterioOrdenacao criterio) {
    int i = esq;
    int j = meio + 1;
    int k = esq;

    while (i <= meio && j <= dir) {
        if (livro_comparar(&vetor[i], &vetor[j], criterio) <= 0) {
            aux[k++] = vetor[i++];
        } else {
            aux[k++] = vetor[j++];
        }
    }

    while (i <= meio) {
        aux[k++] = vetor[i++];
    }

    while (j <= dir) {
        aux[k++] = vetor[j++];
    }

    for (i = esq; i <= dir; i++) {
        vetor[i] = aux[i];
    }
}

/**
 * @brief Função recursiva interna do Merge Sort.
 */
static void mergesort_recursivo(Livro *vetor, Livro *aux, int esq, int dir, CriterioOrdenacao criterio) {
    int meio;

    if (esq >= dir) {
        return;
    }

    meio = esq + (dir - esq) / 2;
    mergesort_recursivo(vetor, aux, esq, meio, criterio);
    mergesort_recursivo(vetor, aux, meio + 1, dir, criterio);
    intercalar(vetor, aux, esq, meio, dir, criterio);
}

void ordenar_livros_mergesort(Livro *vetor, int n, CriterioOrdenacao criterio) {
    Livro *aux;

    if (vetor == NULL || n <= 1) {
        return;
    }

    aux = (Livro *)malloc((size_t)n * sizeof(Livro));
    if (aux == NULL) {
        return;
    }

    mergesort_recursivo(vetor, aux, 0, n - 1, criterio);
    free(aux);
}

/* ==========================================================================
 * QUICK SORT PRÓPRIO (In-place com pivô central, O(n log n) médio)
 * ========================================================================== */

/**
 * @brief Troca dois registos Livro de posição num vetor.
 */
static void trocar_livros(Livro *a, Livro *b) {
    Livro temp = *a;
    *a = *b;
    *b = temp;
}

/**
 * @brief Particiona o subvetor [esq..dir] em torno de um pivô.
 */
static int particionar(Livro *vetor, int esq, int dir, CriterioOrdenacao criterio) {
    int meio = esq + (dir - esq) / 2;
    Livro pivo;
    int i;
    int indice_menores = esq;

    /* Coloca o elemento central no fim como pivô para evitar pior caso em dados já ordenados */
    trocar_livros(&vetor[meio], &vetor[dir]);
    pivo = vetor[dir];

    for (i = esq; i < dir; i++) {
        if (livro_comparar(&vetor[i], &pivo, criterio) <= 0) {
            trocar_livros(&vetor[i], &vetor[indice_menores]);
            indice_menores++;
        }
    }

    trocar_livros(&vetor[indice_menores], &vetor[dir]);
    return indice_menores;
}

/**
 * @brief Função recursiva interna do Quick Sort.
 */
static void quicksort_recursivo(Livro *vetor, int esq, int dir, CriterioOrdenacao criterio) {
    int pos_pivo;

    if (esq >= dir) {
        return;
    }

    pos_pivo = particionar(vetor, esq, dir, criterio);
    quicksort_recursivo(vetor, esq, pos_pivo - 1, criterio);
    quicksort_recursivo(vetor, pos_pivo + 1, dir, criterio);
}

void ordenar_livros_quicksort(Livro *vetor, int n, CriterioOrdenacao criterio) {
    if (vetor == NULL || n <= 1) {
        return;
    }

    quicksort_recursivo(vetor, 0, n - 1, criterio);
}

/* ==========================================================================
 * LISTAGEM ORDENADA SOBRE VETOR AUXILIAR DE CÓPIAS
 * ========================================================================== */

void bst_listar_ordenado(NoBST *raiz, CriterioOrdenacao criterio) {
    int total;
    int indice = 0;
    int i;
    Livro *vetor_copias;

    total = bst_contar_nos(raiz);
    if (total <= 0) {
        printf("\n[Catalogo] Nenhum livro registado na biblioteca.\n");
        return;
    }

    vetor_copias = (Livro *)malloc((size_t)total * sizeof(Livro));
    if (vetor_copias == NULL) {
        printf("\n[Erro] Falha ao alocar memoria para o vetor auxiliar de ordenacao.\n");
        return;
    }

    bst_exportar_vetor(raiz, vetor_copias, &indice);
    ordenar_livros_mergesort(vetor_copias, indice, criterio);

    printf("\n--- Catalogo Ordenado (%d livro(s)) ---\n", indice);
    for (i = 0; i < indice; i++) {
        livro_exibir(&vetor_copias[i]);
    }
    printf("---------------------------------------\n");

    free(vetor_copias);
}

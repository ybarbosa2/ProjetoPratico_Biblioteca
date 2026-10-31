/**
 * @file main.c
 * @brief Ponto de entrada e esqueleto do menu interativo do Sistema de Biblioteca (Etapa 1).
 */

#include <stdio.h>
#include <stdlib.h>
#include "bst.h"
#include "grafo.h"
#include "pilha.h"
#include "fila.h"
#include "lista.h"
#include "ordenacao.h"

/**
 * @brief Limpa o buffer de entrada padrão (stdin) até ao fim da linha.
 */
static void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta caracteres residuais */
    }
}

/**
 * @brief Exibe as opções do menu principal da Biblioteca.
 */
static void exibir_menu(void) {
    printf("\n========================================================\n");
    printf("       SISTEMA DE GESTAO DE BIBLIOTECA (TEMA C)         \n");
    printf("========================================================\n");
    printf(" [1] Cadastrar novo livro (BST + Grafo + Pilha)\n");
    printf(" [2] Buscar livro por ID (BST)\n");
    printf(" [3] Remover livro por ID (BST + Grafo + Pilha)\n");
    printf(" [4] Percursos da BST (Em-Ordem, Pre-Ordem, Pos-Ordem)\n");
    printf(" [5] Adicionar relacao entre livros (Grafo + Pilha)\n");
    printf(" [6] Remover relacao entre livros (Grafo + Pilha)\n");
    printf(" [7] Consultar livros relacionados / Busca em Largura (BFS)\n");
    printf(" [8] Exibir grafo de relacoes entre livros\n");
    printf(" [9] Listar catalogo ordenado (Titulo / Autor / Ano)\n");
    printf("[10] Consultar historico de alteracoes (Pilha)\n");
    printf(" [0] Sair\n");
    printf("--------------------------------------------------------\n");
    printf("Escolha uma opcao: ");
}

int main(void) {
    NoBST *catalogo_bst = bst_criar();
    Grafo grafo_relacoes;
    Pilha historico_acoes;
    int opcao = -1;

    grafo_inicializar(&grafo_relacoes);
    pilha_inicializar(&historico_acoes);

    do {
        exibir_menu();
        if (scanf("%d", &opcao) != 1) {
            printf("\n[!] Entrada invalida. Introduza um numero inteiro.\n");
            limpar_buffer();
            opcao = -1;
            continue;
        }
        limpar_buffer();

        switch (opcao) {
            case 1:
                printf("\n[Etapa 1 - Stub] Opcao 1 (Cadastrar livro) sera implementada nas proximas etapas.\n");
                break;
            case 2:
                printf("\n[Etapa 1 - Stub] Opcao 2 (Buscar livro por ID) sera implementada nas proximas etapas.\n");
                break;
            case 3:
                printf("\n[Etapa 1 - Stub] Opcao 3 (Remover livro por ID) sera implementada nas proximas etapas.\n");
                break;
            case 4:
                printf("\n[Etapa 1 - Stub] Opcao 4 (Percursos da BST) sera implementada nas proximas etapas.\n");
                break;
            case 5:
                printf("\n[Etapa 1 - Stub] Opcao 5 (Adicionar relacao no Grafo) sera implementada nas proximas etapas.\n");
                break;
            case 6:
                printf("\n[Etapa 1 - Stub] Opcao 6 (Remover relacao no Grafo) sera implementada nas proximas etapas.\n");
                break;
            case 7:
                printf("\n[Etapa 1 - Stub] Opcao 7 (Busca em Largura - BFS) sera implementada nas proximas etapas.\n");
                break;
            case 8:
                printf("\n[Etapa 1 - Stub] Opcao 8 (Exibir Grafo) sera implementada nas proximas etapas.\n");
                break;
            case 9:
                printf("\n[Etapa 1 - Stub] Opcao 9 (Ordenacao do catalogo) sera implementada nas proximas etapas.\n");
                break;
            case 10:
                printf("\n[Etapa 1 - Stub] Opcao 10 (Historico na Pilha) sera implementada nas proximas etapas.\n");
                break;
            case 0:
                printf("\nA encerrar o sistema e a libertar memoria...\n");
                break;
            default:
                printf("\n[!] Opcao inexistente. Escolha um valor entre 0 e 10.\n");
                break;
        }
    } while (opcao != 0);

    bst_liberar(catalogo_bst);
    grafo_liberar(&grafo_relacoes);
    pilha_liberar(&historico_acoes);

    return EXIT_SUCCESS;
}


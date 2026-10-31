/**
 * @file main.c
 * @brief Menu interativo do Sistema de Gestão de Biblioteca (Etapa 3).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst.h"
#include "grafo.h"
#include "pilha.h"
#include "fila.h"
#include "lista.h"
#include "ordenacao.h"
#include "io.h"

static void exibir_menu(void) {
    printf("\n========================================================\n");
    printf("       SISTEMA DE GESTAO DE BIBLIOTECA (TEMA C)         \n");
    printf("========================================================\n");
    printf(" [1] Cadastrar novo livro (BST + Grafo + Pilha)\n");
    printf(" [2] Buscar livro por ID (BST)\n");
    printf(" [3] Remover livro por ID (BST + Grafo + Pilha)\n");
    printf(" [4] Percursos e estatisticas da BST\n");
    printf(" [5] Adicionar relacao entre livros (Grafo + Pilha)\n");
    printf(" [6] Remover relacao entre livros (Grafo + Pilha)\n");
    printf(" [7] Consultar livros relacionados / Busca em Largura (BFS)\n");
    printf(" [8] Exibir grafo de relacoes entre livros\n");
    printf(" [9] Listar catalogo ordenado por Titulo (R06)\n");
    printf("[10] Consultar historico de alteracoes (Pilha - R04)\n");
    printf(" [0] Sair\n");
    printf("--------------------------------------------------------\n");
    printf("Escolha uma opcao: ");
}

static void cadastrar_livro_menu(NoBST **catalogo, Pilha *historico) {
    Livro novo;
    StatusIO st_io;
    StatusBST st_bst;

    printf("\n--- Cadastrar Novo Livro ---\n");

    printf("Digite o ID do livro (> 0): ");
    st_io = io_ler_id(&novo.id, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    if (bst_buscar(*catalogo, novo.id) != NULL) {
        printf("[!] ID duplicado: ja existe um livro cadastrado com o ID %d.\n", novo.id);
        return;
    }

    printf("Digite o Titulo (ate 79 caracteres uteis): ");
    st_io = io_ler_texto(novo.titulo, sizeof(novo.titulo), stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    printf("Digite o Autor (ate 79 caracteres uteis): ");
    st_io = io_ler_texto(novo.autor, sizeof(novo.autor), stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    printf("Digite o Ano de Publicacao (1 a 2026): ");
    st_io = io_ler_ano(&novo.ano_publicacao, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    st_bst = bst_inserir(catalogo, novo);
    if (st_bst == BST_SUCESSO) {
        printf("[OK] Livro cadastrado com sucesso no catalogo!\n");
        pilha_empilhar(historico, ACAO_INSERIR_LIVRO, novo.id, 0, &novo, "Cadastro de livro");
    } else {
        printf("[!] Falha ao inserir na BST: %s\n", bst_status_mensagem(st_bst));
    }
}

static void buscar_livro_menu(NoBST *catalogo) {
    int id;
    StatusIO st_io;
    NoBST *no;

    printf("\n--- Buscar Livro por ID ---\n");
    printf("Digite o ID do livro procurado: ");
    st_io = io_ler_id(&id, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    no = bst_buscar(catalogo, id);
    if (no != NULL) {
        printf("\n[Livro Localizado na BST]:\n");
        livro_exibir(&no->livro);
    } else {
        printf("[!] Nenhum livro com o ID %d foi encontrado na arvore.\n", id);
    }
}

static void remover_livro_menu(NoBST **catalogo, Pilha *historico) {
    int id;
    Livro removido;
    StatusIO st_io;
    StatusBST st_bst;

    printf("\n--- Remover Livro por ID ---\n");
    printf("Digite o ID do livro a remover: ");
    st_io = io_ler_id(&id, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    st_bst = bst_remover(catalogo, id, &removido);
    if (st_bst == BST_SUCESSO) {
        printf("[OK] Livro removido com sucesso da BST!\n");
        livro_exibir(&removido);
        pilha_empilhar(historico, ACAO_REMOVER_LIVRO, id, 0, &removido, "Remocao de livro");
    } else {
        printf("[!] Falha na remocao: %s\n", bst_status_mensagem(st_bst));
    }
}

static void percursos_bst_menu(NoBST *catalogo) {
    int menor_id = 0, maior_id = 0;
    int total = bst_contar_nos(catalogo);

    if (total == 0) {
        printf("\n[BST] A arvore esta vazia (nenhum livro cadastrado).\n");
        return;
    }

    bst_obter_menor_id(catalogo, &menor_id);
    bst_obter_maior_id(catalogo, &maior_id);

    printf("\n--- Estatisticas da BST ---\n");
    printf("  Total de livros: %d\n", total);
    printf("  Altura da arvore: %d\n", bst_altura(catalogo));
    printf("  Menor ID cadastrado: %d\n", menor_id);
    printf("  Maior ID cadastrado: %d\n", maior_id);

    printf("\n--- Listagem Em-Ordem (ID crescente) ---\n");
    bst_percorrer_em_ordem(catalogo);

    printf("\n--- Listagem Pre-Ordem (Raiz, Esq, Dir) ---\n");
    bst_percorrer_pre_ordem(catalogo);

    printf("\n--- Listagem Pos-Ordem (Esq, Dir, Raiz) ---\n");
    bst_percorrer_pos_ordem(catalogo);
    printf("-----------------------------------------\n");
}

int main(void) {
    NoBST *catalogo_bst = bst_criar();
    Grafo grafo_relacoes;
    Pilha historico_acoes;
    char buffer_opcao[32];
    int opcao = -1;

    grafo_inicializar(&grafo_relacoes);
    pilha_inicializar(&historico_acoes);

    do {
        exibir_menu();
        if (io_ler_linha(buffer_opcao, sizeof(buffer_opcao), stdin) != IO_SUCESSO) {
            printf("\n[!] Entrada invalida.\n");
            continue;
        }

        if (sscanf(buffer_opcao, "%d", &opcao) != 1) {
            printf("\n[!] Entrada invalida. Introduza um numero inteiro.\n");
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1:
                cadastrar_livro_menu(&catalogo_bst, &historico_acoes);
                break;
            case 2:
                buscar_livro_menu(catalogo_bst);
                break;
            case 3:
                remover_livro_menu(&catalogo_bst, &historico_acoes);
                break;
            case 4:
                percursos_bst_menu(catalogo_bst);
                break;
            case 5:
                printf("\n[Etapa 4] Adicionar relacao no Grafo sera ativado na Etapa 4.\n");
                break;
            case 6:
                printf("\n[Etapa 4] Remover relacao no Grafo sera ativado na Etapa 4.\n");
                break;
            case 7:
                printf("\n[Etapa 4] Busca em Largura (BFS) sera ativada na Etapa 4.\n");
                break;
            case 8:
                printf("\n[Etapa 4] Exibir Grafo sera ativado na Etapa 4.\n");
                break;
            case 9:
                bst_listar_ordenado(catalogo_bst, ORDENAR_POR_TITULO);
                break;
            case 10:
                pilha_exibir_historico(&historico_acoes);
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

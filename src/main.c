/**
 * @file main.c
 * @brief Menu interativo do Sistema de Gestão de Biblioteca (Etapa 4).
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
    printf(" [7] Consultar conexoes / Busca em Largura (BFS)\n");
    printf(" [8] Exibir grafo de relacoes entre livros\n");
    printf(" [9] Listar catalogo ordenado por Titulo (R06)\n");
    printf("[10] Consultar historico de alteracoes (Pilha - R04)\n");
    printf(" [0] Sair\n");
    printf("--------------------------------------------------------\n");
    printf("Escolha uma opcao: ");
}

static void cadastrar_livro_menu(NoBST **catalogo, Grafo *grafo, Pilha *historico) {
    Livro novo;
    StatusIO st_io;
    StatusGrafo st_sis;

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

    /* Inserção atômica no catálogo e no grafo com rollback garantido */
    st_sis = sistema_cadastrar_livro(catalogo, grafo, novo);
    if (st_sis == GRAFO_SUCESSO) {
        printf("[OK] Livro cadastrado com sucesso no catalogo e no grafo!\n");
        pilha_empilhar(historico, ACAO_INSERIR_LIVRO, novo.id, 0, &novo, "Cadastro de livro");
    } else {
        printf("[!] Falha no cadastro: %s\n", grafo_status_mensagem(st_sis));
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

static void remover_livro_menu(NoBST **catalogo, Grafo *grafo, Pilha *historico) {
    int id;
    Livro removido;
    StatusIO st_io;
    StatusGrafo st_sis;

    printf("\n--- Remover Livro por ID ---\n");
    printf("Digite o ID do livro a remover: ");
    st_io = io_ler_id(&id, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    /* Remoção atômica na BST e no Grafo */
    st_sis = sistema_remover_livro(catalogo, grafo, id, &removido);
    if (st_sis == GRAFO_SUCESSO) {
        printf("[OK] Livro e suas conexoes foram removidos com sucesso!\n");
        livro_exibir(&removido);
        pilha_empilhar(historico, ACAO_REMOVER_LIVRO, id, 0, &removido, "Remocao de livro");
    } else {
        printf("[!] Falha na remocao: %s\n", grafo_status_mensagem(st_sis));
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

static void adicionar_relacao_menu(Grafo *grafo, Pilha *historico) {
    int id1, id2;
    StatusIO st_io;
    StatusGrafo st_g;

    printf("\n--- Adicionar Relacao entre Livros ---\n");
    printf("Digite o ID do primeiro livro: ");
    st_io = io_ler_id(&id1, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    printf("Digite o ID do segundo livro: ");
    st_io = io_ler_id(&id2, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    st_g = grafo_adicionar_aresta(grafo, id1, id2);
    if (st_g == GRAFO_SUCESSO) {
        printf("[OK] Relacao adicionada com sucesso entre o livro %d e o livro %d!\n", id1, id2);
        pilha_empilhar(historico, ACAO_ADICIONAR_RELACAO, id1, id2, NULL, "Adicao de conexao");
    } else {
        printf("[!] Falha ao adicionar relacao: %s\n", grafo_status_mensagem(st_g));
    }
}

static void remover_relacao_menu(Grafo *grafo, Pilha *historico) {
    int id1, id2;
    StatusIO st_io;
    StatusGrafo st_g;

    printf("\n--- Remover Relacao entre Livros ---\n");
    printf("Digite o ID do primeiro livro: ");
    st_io = io_ler_id(&id1, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    printf("Digite o ID do segundo livro: ");
    st_io = io_ler_id(&id2, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    st_g = grafo_remover_aresta(grafo, id1, id2);
    if (st_g == GRAFO_SUCESSO) {
        printf("[OK] Relacao removida com sucesso entre o livro %d e o livro %d!\n", id1, id2);
        pilha_empilhar(historico, ACAO_REMOVER_RELACAO, id1, id2, NULL, "Remocao de conexao");
    } else {
        printf("[!] Falha ao remover relacao: %s\n", grafo_status_mensagem(st_g));
    }
}

static void consultar_bfs_menu(Grafo *grafo, NoBST *catalogo) {
    int id_origem;
    StatusIO st_io;
    StatusGrafo st_g;
    Lista visitados;
    NoLista *atual;
    int pos = 1;

    printf("\n--- Busca em Largura (BFS) / Livros Relacionados ---\n");
    printf("Digite o ID do livro de partida da busca: ");
    st_io = io_ler_id(&id_origem, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    lista_inicializar(&visitados);
    st_g = grafo_bfs(grafo, id_origem, &visitados);

    if (st_g != GRAFO_SUCESSO) {
        printf("[!] Falha na BFS: %s\n", grafo_status_mensagem(st_g));
        lista_liberar(&visitados);
        return;
    }

    printf("\n--- Ordem de Descoberta da BFS (Origem ID %d) ---\n", id_origem);
    atual = visitados.inicio;
    while (atual != NULL) {
        NoBST *no = bst_buscar(catalogo, atual->id_livro);
        if (no != NULL) {
            printf("  %d. [Nivel/Distancia: %d] ID %-4d | Titulo: %s\n",
                   pos, atual->distancia, atual->id_livro, no->livro.titulo);
        } else {
            printf("  %d. [Nivel/Distancia: %d] ID %-4d\n",
                   pos, atual->distancia, atual->id_livro);
        }
        atual = atual->prox;
        pos++;
    }
    printf("----------------------------------------------------\n");

    lista_liberar(&visitados);
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
                cadastrar_livro_menu(&catalogo_bst, &grafo_relacoes, &historico_acoes);
                break;
            case 2:
                buscar_livro_menu(catalogo_bst);
                break;
            case 3:
                remover_livro_menu(&catalogo_bst, &grafo_relacoes, &historico_acoes);
                break;
            case 4:
                percursos_bst_menu(catalogo_bst);
                break;
            case 5:
                adicionar_relacao_menu(&grafo_relacoes, &historico_acoes);
                break;
            case 6:
                remover_relacao_menu(&grafo_relacoes, &historico_acoes);
                break;
            case 7:
                consultar_bfs_menu(&grafo_relacoes, catalogo_bst);
                break;
            case 8:
                grafo_exibir(&grafo_relacoes, catalogo_bst);
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

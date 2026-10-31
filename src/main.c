/**
 * @file main.c
 * @brief Menu Interativo Completo do Sistema de Gestão de Biblioteca (Etapa 5).
 *
 * Implementa o menu de referência adaptado ao Tema C (Biblioteca) segundo a Seção 10,
 * integrando BST (R07), Grafo com BFS (R08), Pilha de Histórico (R04), Lista Encadeada (R03),
 * Fila (R05), Ordenação própria (R06) e Validação Segura de Entradas (R01 e Seção 9).
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

/**
 * @brief Exibe o menu principal com todas as 11 opções de referência mais o encerramento.
 */
static void exibir_menu(void) {
    printf("\n====================================================================\n");
    printf("         SISTEMA DE GESTAO DE BIBLIOTECA - ESTRUTURA DE DADOS       \n");
    printf("====================================================================\n");
    printf(" [1]  Cadastrar livro (BST + Grafo com Rollback)\n");
    printf(" [2]  Buscar livro por ID na BST (e consultar menor/maior ID)\n");
    printf(" [3]  Remover livro e respetivo vertice/arestas (BST + Grafo)\n");
    printf(" [4]  Listar livros em-ordem (BST - ID crescente)\n");
    printf(" [5]  Ordenar e exibir livros por titulo (Vetor auxiliar - R06)\n");
    printf(" [6]  Adicionar conexao/relacionamento entre dois livros (Grafo)\n");
    printf(" [7]  Remover conexao entre dois livros (Grafo)\n");
    printf(" [8]  Consultar conexao direta entre dois livros\n");
    printf(" [9]  Listar livros alcancaveis a partir de um ID (BFS + Lista)\n");
    printf("[10]  Verificar existencia de caminho entre dois IDs (Grafo)\n");
    printf("[11]  Consultar historico de alteracoes (Pilha - ate 5 mais recentes)\n");
    printf(" [0]  Encerrar o sistema e liberar memoria\n");
    printf("--------------------------------------------------------------------\n");
    printf("Escolha uma opcao: ");
}

/**
 * @brief Opção 1: Cadastrar novo livro (BST + Grafo com Rollback).
 *        REGRA DO HISTÓRICO: Registra na pilha se concluída com sucesso.
 */
static void opcao_cadastrar_livro(NoBST **catalogo, Grafo *grafo, Pilha *historico) {
    Livro novo;
    StatusIO st_io;
    StatusGrafo st_sis;

    printf("\n--- [Opcao 1] Cadastrar Novo Livro ---\n");

    printf("Digite o ID do livro (inteiro positivo > 0): ");
    st_io = io_ler_id(&novo.id, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    if (bst_buscar(*catalogo, novo.id) != NULL) {
        printf("[!] ID duplicado: ja existe um livro cadastrado com o ID %d.\n", novo.id);
        return;
    }

    printf("Digite o Titulo (ate 79 caracteres uteis, aceita espacos): ");
    st_io = io_ler_texto(novo.titulo, sizeof(novo.titulo), stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    printf("Digite o Autor (ate 79 caracteres uteis, aceita espacos): ");
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

    /* Operação atômica com rollback automático caso o grafo atinja 100 vértices */
    st_sis = sistema_cadastrar_livro(catalogo, grafo, novo);
    if (st_sis == GRAFO_SUCESSO) {
        printf("[OK] Livro ID %d cadastrado com sucesso no catalogo e no grafo!\n", novo.id);
        /* REGRA DO HISTÓRICO: Registra alteração de estado com sucesso */
        pilha_empilhar(historico, ACAO_INSERIR_LIVRO, novo.id, 0, &novo, "Cadastro de livro");
    } else {
        printf("[!] Falha no cadastro: %s\n", grafo_status_mensagem(st_sis));
    }
}

/**
 * @brief Opção 2: Buscar livro por ID na BST e exibir menor e maior ID (R07).
 *        NÃO entra no histórico (apenas consulta).
 */
static void opcao_buscar_livro(NoBST *catalogo) {
    int id;
    int menor_id, maior_id;
    StatusIO st_io;
    NoBST *no;

    printf("\n--- [Opcao 2] Buscar Livro por ID na BST ---\n");

    if (catalogo == NULL) {
        printf("[Catalogo] A arvore esta vazia (nenhum livro cadastrado).\n");
        return;
    }

    if (bst_obter_menor_id(catalogo, &menor_id) && bst_obter_maior_id(catalogo, &maior_id)) {
        printf("[Informacao da BST] Menor ID cadastrado: %d | Maior ID cadastrado: %d\n",
               menor_id, maior_id);
    }

    printf("Digite o ID do livro procurado: ");
    st_io = io_ler_id(&id, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    no = bst_buscar(catalogo, id);
    if (no != NULL) {
        printf("\n[Livro Localizado com Sucesso]:\n");
        livro_exibir(&no->livro);
    } else {
        printf("[!] Nenhum livro com o ID %d foi encontrado na arvore BST.\n", id);
    }
}

/**
 * @brief Opção 3: Remover livro e respetivo vértice/arestas (BST + Grafo).
 *        REGRA DO HISTÓRICO: Registra na pilha se concluída com sucesso.
 */
static void opcao_remover_livro(NoBST **catalogo, Grafo *grafo, Pilha *historico) {
    int id;
    Livro removido;
    StatusIO st_io;
    StatusGrafo st_sis;

    printf("\n--- [Opcao 3] Remover Livro e Conexoes ---\n");
    printf("Digite o ID do livro a remover: ");
    st_io = io_ler_id(&id, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    st_sis = sistema_remover_livro(catalogo, grafo, id, &removido);
    if (st_sis == GRAFO_SUCESSO) {
        printf("[OK] Livro ID %d e todas as suas relacoes foram removidos com sucesso!\n", id);
        livro_exibir(&removido);
        /* REGRA DO HISTÓRICO: Registra alteração de estado com sucesso */
        pilha_empilhar(historico, ACAO_REMOVER_LIVRO, id, 0, &removido, "Remocao de livro");
    } else {
        printf("[!] Falha na remocao: %s\n", grafo_status_mensagem(st_sis));
    }
}

/**
 * @brief Opção 4: Listar livros em-ordem (crescente de ID).
 *        NÃO entra no histórico (apenas consulta).
 */
static void opcao_listar_em_ordem(NoBST *catalogo) {
    int total = bst_contar_nos(catalogo);

    printf("\n--- [Opcao 4] Listagem Em-Ordem da BST (ID crescente) ---\n");
    if (total == 0) {
        printf("[Catalogo] Nenhum livro cadastrado na biblioteca.\n");
        return;
    }

    printf("Total de livros cadastrados: %d | Altura da BST: %d\n\n",
           total, bst_altura(catalogo));
    bst_percorrer_em_ordem(catalogo);
    printf("--------------------------------------------------------\n");
}

/**
 * @brief Opção 5: Ordenar e exibir livros por título (Merge Sort próprio sobre cópias).
 *        NÃO entra no histórico (apenas consulta).
 */
static void opcao_ordenar_por_titulo(NoBST *catalogo) {
    printf("\n--- [Opcao 5] Listagem Ordenada por Titulo (Merge Sort - R06) ---\n");
    bst_listar_ordenado(catalogo, ORDENAR_POR_TITULO);
}

/**
 * @brief Opção 6: Adicionar conexão/relacionamento entre dois livros (aresta).
 *        REGRA DO HISTÓRICO: Registra na pilha se concluída com sucesso.
 */
static void opcao_adicionar_conexao(Grafo *grafo, Pilha *historico) {
    int id1, id2;
    StatusIO st_io;
    StatusGrafo st_g;

    printf("\n--- [Opcao 6] Adicionar Conexao entre Dois Livros ---\n");
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
        printf("[OK] Conexao criada com sucesso entre o livro %d e o livro %d!\n", id1, id2);
        /* REGRA DO HISTÓRICO: Registra alteração de estado com sucesso */
        pilha_empilhar(historico, ACAO_ADICIONAR_RELACAO, id1, id2, NULL, "Adicao de conexao");
    } else {
        printf("[!] Falha ao adicionar conexao: %s\n", grafo_status_mensagem(st_g));
    }
}

/**
 * @brief Opção 7: Remover conexão entre dois livros (aresta).
 *        REGRA DO HISTÓRICO: Registra na pilha se concluída com sucesso.
 */
static void opcao_remover_conexao(Grafo *grafo, Pilha *historico) {
    int id1, id2;
    StatusIO st_io;
    StatusGrafo st_g;

    printf("\n--- [Opcao 7] Remover Conexao entre Dois Livros ---\n");
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
        printf("[OK] Conexao removida com sucesso entre o livro %d e o livro %d!\n", id1, id2);
        /* REGRA DO HISTÓRICO: Registra alteração de estado com sucesso */
        pilha_empilhar(historico, ACAO_REMOVER_RELACAO, id1, id2, NULL, "Remocao de conexao");
    } else {
        printf("[!] Falha ao remover conexao: %s\n", grafo_status_mensagem(st_g));
    }
}

/**
 * @brief Opção 8: Consultar conexão direta entre dois livros.
 *        NÃO entra no histórico (apenas consulta).
 */
static void opcao_consultar_conexao_direta(const Grafo *grafo) {
    int id1, id2;
    StatusIO st_io;

    printf("\n--- [Opcao 8] Consultar Conexao Direta ---\n");
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

    if (grafo_obter_indice(grafo, id1) == -1) {
        printf("[!] Livro com ID %d nao esta cadastrado no grafo.\n", id1);
        return;
    }
    if (grafo_obter_indice(grafo, id2) == -1) {
        printf("[!] Livro com ID %d nao esta cadastrado no grafo.\n", id2);
        return;
    }

    if (grafo_existe_aresta(grafo, id1, id2)) {
        printf("[Resultado] SIM! Existe conexao direta (aresta) entre o livro %d e o livro %d.\n",
               id1, id2);
    } else {
        printf("[Resultado] NAO. Nao existe conexao direta entre o livro %d e o livro %d.\n",
               id1, id2);
    }
}

/**
 * @brief Opção 9: Listar livros alcançáveis a partir de um ID (BFS + Lista).
 *        NÃO entra no histórico (apenas consulta).
 */
static void opcao_listar_alcancaveis_bfs(const Grafo *grafo, NoBST *catalogo) {
    int id_origem;
    StatusIO st_io;
    StatusGrafo st_g;
    Lista visitados;
    NoLista *atual;
    int pos = 1;

    printf("\n--- [Opcao 9] Livros Alcancaveis (Busca em Largura - BFS) ---\n");
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

    printf("\n--- Ordem de Descoberta da BFS (Origem ID %d | Total alcancados: %d) ---\n",
           id_origem, visitados.tamanho);
    atual = visitados.inicio;
    while (atual != NULL) {
        NoBST *no = bst_buscar(catalogo, atual->id_livro);
        if (no != NULL) {
            printf("  %2d. [Distancia/Nivel: %d] ID %-4d | \"%s\" (Autor: %s, %d)\n",
                   pos, atual->distancia, atual->id_livro,
                   no->livro.titulo, no->livro.autor, no->livro.ano_publicacao);
        } else {
            printf("  %2d. [Distancia/Nivel: %d] ID %-4d\n",
                   pos, atual->distancia, atual->id_livro);
        }
        atual = atual->prox;
        pos++;
    }
    printf("--------------------------------------------------------------------\n");

    lista_liberar(&visitados);
}

/**
 * @brief Opção 10: Verificar existência de caminho entre dois IDs (Grafo).
 *        NÃO entra no histórico (apenas consulta).
 */
static void opcao_verificar_caminho(const Grafo *grafo) {
    int id_origem, id_destino;
    StatusIO st_io;
    StatusGrafo st_g;
    bool existe = false;

    printf("\n--- [Opcao 10] Verificar Existencia de Caminho entre Dois Livros ---\n");
    printf("Digite o ID de origem: ");
    st_io = io_ler_id(&id_origem, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    printf("Digite o ID de destino: ");
    st_io = io_ler_id(&id_destino, stdin);
    if (st_io != IO_SUCESSO) {
        printf("[!] %s\n", io_mensagem_erro(st_io));
        return;
    }

    st_g = grafo_existe_caminho(grafo, id_origem, id_destino, &existe);
    if (st_g != GRAFO_SUCESSO) {
        printf("[!] Consulta invalida: %s\n", grafo_status_mensagem(st_g));
        return;
    }

    if (existe) {
        printf("[Resultado] SIM! Existe pelo menos um caminho no grafo ligando o livro %d ao livro %d.\n",
               id_origem, id_destino);
    } else {
        printf("[Resultado] NAO. Os livros %d e %d pertencem a componentes desconectados (sem caminho).\n",
               id_origem, id_destino);
    }
}

/**
 * @brief Opção 11: Consultar histórico de alterações (Pilha).
 *        NÃO entra no histórico (apenas consulta).
 */
static void opcao_consultar_historico(const Pilha *historico) {
    printf("\n--- [Opcao 11] Historico de Alteracoes Concluidas com Sucesso ---\n");
    pilha_exibir_historico(historico);
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
            printf("\n[!] Entrada invalida. Introduza um numero inteiro entre 0 e 11.\n");
            opcao = -1;
            continue;
        }

        switch (opcao) {
            case 1:
                opcao_cadastrar_livro(&catalogo_bst, &grafo_relacoes, &historico_acoes);
                break;
            case 2:
                opcao_buscar_livro(catalogo_bst);
                break;
            case 3:
                opcao_remover_livro(&catalogo_bst, &grafo_relacoes, &historico_acoes);
                break;
            case 4:
                opcao_listar_em_ordem(catalogo_bst);
                break;
            case 5:
                opcao_ordenar_por_titulo(catalogo_bst);
                break;
            case 6:
                opcao_adicionar_conexao(&grafo_relacoes, &historico_acoes);
                break;
            case 7:
                opcao_remover_conexao(&grafo_relacoes, &historico_acoes);
                break;
            case 8:
                opcao_consultar_conexao_direta(&grafo_relacoes);
                break;
            case 9:
                opcao_listar_alcancaveis_bfs(&grafo_relacoes, catalogo_bst);
                break;
            case 10:
                opcao_verificar_caminho(&grafo_relacoes);
                break;
            case 11:
                opcao_consultar_historico(&historico_acoes);
                break;
            case 0:
                printf("\nA encerrar o sistema e a liberar toda a memoria alocada...\n");
                break;
            default:
                printf("\n[!] Opcao inexistente. Escolha um valor entre 0 e 11.\n");
                break;
        }
    } while (opcao != 0);

    /* Liberação total de memória antes de sair (Requisito R02) */
    bst_liberar(catalogo_bst);
    grafo_liberar(&grafo_relacoes);
    pilha_liberar(&historico_acoes);

    printf("Memoria liberada com sucesso. Programa finalizado.\n");
    return EXIT_SUCCESS;
}

/**
 * @file test_etapa4.c
 * @brief Testes automatizados da Etapa 4 (Grafo - R08, BFS com vizinhos crescentes, Caminho e Rollback).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "grafo.h"
#include "bst.h"
#include "lista.h"

/* ==========================================================================
 * TESTE 1: Regras básicas de vértices e arestas no Grafo (R08 e Seção 5)
 * ========================================================================== */
static void testar_grafo_regras_basicas_r08(void) {
    Grafo g;
    printf("[Teste 1/5] A testar Regras de Vertices e Arestas do Grafo (R08)...\n");

    grafo_inicializar(&g);
    assert(g.num_vertices == 0);
    assert(g.num_arestas == 0);

    /* 1. Inserção de IDs arbitrários (não sequenciais) */
    assert(grafo_adicionar_vertice(&g, 5000) == GRAFO_SUCESSO);
    assert(grafo_adicionar_vertice(&g, 1200) == GRAFO_SUCESSO);
    assert(grafo_adicionar_vertice(&g, 350) == GRAFO_SUCESSO);
    assert(grafo_adicionar_vertice(&g, 80) == GRAFO_SUCESSO);
    assert(g.num_vertices == 4);

    /* Rejeição de duplicado e ID inválido */
    assert(grafo_adicionar_vertice(&g, 5000) == GRAFO_ERRO_VERTICE_DUPLICADO);
    assert(grafo_adicionar_vertice(&g, 0) == GRAFO_ERRO_ID_INVALIDO);
    assert(grafo_adicionar_vertice(&g, -10) == GRAFO_ERRO_ID_INVALIDO);

    /* 2. Adição de aresta (A,B) e rejeição de duplicata na ordem inversa (B,A) */
    assert(grafo_adicionar_aresta(&g, 5000, 1200) == GRAFO_SUCESSO);
    assert(g.num_arestas == 1);
    assert(grafo_existe_aresta(&g, 5000, 1200) == true);
    assert(grafo_existe_aresta(&g, 1200, 5000) == true); /* Simetria não direcionada */

    /* Rejeição de aresta duplicada (B, A) */
    assert(grafo_adicionar_aresta(&g, 1200, 5000) == GRAFO_ERRO_ARESTA_DUPLICADA);
    assert(grafo_adicionar_aresta(&g, 5000, 1200) == GRAFO_ERRO_ARESTA_DUPLICADA);
    assert(g.num_arestas == 1);

    /* 3. Rejeição de Laço (A, A) */
    assert(grafo_adicionar_aresta(&g, 5000, 5000) == GRAFO_ERRO_LACO);
    assert(g.num_arestas == 1);

    /* 4. Conexão envolvendo ID inexistente */
    assert(grafo_adicionar_aresta(&g, 5000, 9999) == GRAFO_ERRO_VERTICE_NAO_ENCONTRADO);
    assert(grafo_adicionar_aresta(&g, 8888, 5000) == GRAFO_ERRO_VERTICE_NAO_ENCONTRADO);

    /* 5. Remoção de aresta inexistente e com ID ausente */
    assert(grafo_remover_aresta(&g, 1200, 350) == GRAFO_ERRO_ARESTA_NAO_ENCONTRADA);
    assert(grafo_remover_aresta(&g, 1200, 9999) == GRAFO_ERRO_VERTICE_NAO_ENCONTRADO);

    /* Remoção com sucesso */
    assert(grafo_remover_aresta(&g, 5000, 1200) == GRAFO_SUCESSO);
    assert(g.num_arestas == 0);
    assert(grafo_existe_aresta(&g, 5000, 1200) == false);

    printf("  -> Grafo: IDs arbitrarios, arestas simetricas, rejeicao de duplicata/laco: OK!\n\n");
}

/* ==========================================================================
 * TESTE 2: Remoção de vértice com arestas incidentes e reutilização de slots
 * ========================================================================== */
static void testar_remocao_vertice_e_limpeza(void) {
    Grafo g;
    printf("[Teste 2/5] A testar Remocao de Vertice e Limpeza de Arestas Incidentes...\n");

    grafo_inicializar(&g);
    grafo_adicionar_vertice(&g, 100);
    grafo_adicionar_vertice(&g, 101);
    grafo_adicionar_vertice(&g, 102);
    grafo_adicionar_vertice(&g, 103);

    grafo_adicionar_aresta(&g, 100, 101);
    grafo_adicionar_aresta(&g, 100, 102);
    grafo_adicionar_aresta(&g, 100, 103);
    grafo_adicionar_aresta(&g, 101, 102);
    assert(g.num_arestas == 4);

    /* Remove o vértice central 100: deve decrementar 3 arestas incidentes */
    assert(grafo_remover_vertice(&g, 100) == GRAFO_SUCESSO);
    assert(g.num_vertices == 3);
    assert(g.num_arestas == 1); /* Apenas (101, 102) resta */
    assert(grafo_existe_aresta(&g, 101, 102) == true);
    assert(grafo_existe_aresta(&g, 100, 101) == false);

    /* Reutilização do slot liberado */
    assert(grafo_adicionar_vertice(&g, 999) == GRAFO_SUCESSO);
    assert(g.num_vertices == 4);
    assert(grafo_obter_indice(&g, 999) != -1);

    printf("  -> Remocao de vertice e limpeza de conexoes: OK!\n\n");
}

/* ==========================================================================
 * TESTE 3: BFS com visitação estritamente em ORDEM CRESCENTE DE ID (Seção 6)
 * ========================================================================== */
static void testar_bfs_ordem_crescente_de_id(void) {
    Grafo g;
    Lista visitados;
    NoLista *atual;
    int ordem_esperada[8] = { 10, 20, 30, 40, 50, 60, 70, 80 };
    int i = 0;

    printf("[Teste 3/5] A testar BFS com Vizinhos em Ordem Crescente de ID...\n");

    grafo_inicializar(&g);
    lista_inicializar(&visitados);

    /* Adiciona vértices fora de ordem */
    grafo_adicionar_vertice(&g, 10);
    grafo_adicionar_vertice(&g, 50);
    grafo_adicionar_vertice(&g, 20);
    grafo_adicionar_vertice(&g, 40);
    grafo_adicionar_vertice(&g, 30);
    grafo_adicionar_vertice(&g, 70);
    grafo_adicionar_vertice(&g, 60);
    grafo_adicionar_vertice(&g, 80);
    grafo_adicionar_vertice(&g, 9999); /* Vértice isolado (desconectado) */

    /* Conexões a partir de 10 inseridas desordenadamente */
    grafo_adicionar_aresta(&g, 10, 50);
    grafo_adicionar_aresta(&g, 10, 20);
    grafo_adicionar_aresta(&g, 10, 40);
    grafo_adicionar_aresta(&g, 10, 30);

    /* Conexões de 2º nível */
    grafo_adicionar_aresta(&g, 20, 70);
    grafo_adicionar_aresta(&g, 20, 60);
    grafo_adicionar_aresta(&g, 50, 80);

    /* Executa BFS com origem no vértice 10 */
    assert(grafo_bfs(&g, 10, &visitados) == GRAFO_SUCESSO);
    assert(visitados.tamanho == 8);

    /* Valida a ordem exata da BFS (nível 0: 10; nível 1 em ordem crescente: 20,30,40,50; nível 2: 60,70,80) */
    atual = visitados.inicio;
    while (atual != NULL && i < 8) {
        assert(atual->id_livro == ordem_esperada[i]);
        atual = atual->prox;
        i++;
    }
    assert(i == 8);

    /* Vértice desconectado 9999 não deve constar na BFS de 10 */
    assert(lista_contem(&visitados, 9999) == false);

    /* Teste de Origem Isolada: deve produzir na lista APENAS o seu próprio ID */
    lista_liberar(&visitados);
    assert(grafo_bfs(&g, 9999, &visitados) == GRAFO_SUCESSO);
    assert(visitados.tamanho == 1);
    assert(visitados.inicio->id_livro == 9999);

    /* Teste de BFS com ID inexistente */
    lista_liberar(&visitados);
    assert(grafo_bfs(&g, 1111, &visitados) == GRAFO_ERRO_VERTICE_NAO_ENCONTRADO);

    lista_liberar(&visitados);
    printf("  -> BFS: Vizinhos em ordem crescente de ID e origem isolada: OK!\n\n");
}

/* ==========================================================================
 * TESTE 4: Existência de Caminho (caminho válido, inexistente e ID ausente)
 * ========================================================================== */
static void testar_existencia_caminho(void) {
    Grafo g;
    bool existe = false;
    printf("[Teste 4/5] A testar Verificacao de Existencia de Caminho...\n");

    grafo_inicializar(&g);
    grafo_adicionar_vertice(&g, 1);
    grafo_adicionar_vertice(&g, 2);
    grafo_adicionar_vertice(&g, 3);
    grafo_adicionar_vertice(&g, 4);  /* Componente 1: 1 <-> 2 <-> 3 */
    grafo_adicionar_vertice(&g, 10); /* Componente 2: 10 <-> 20 */
    grafo_adicionar_vertice(&g, 20);

    grafo_adicionar_aresta(&g, 1, 2);
    grafo_adicionar_aresta(&g, 2, 3);
    grafo_adicionar_aresta(&g, 10, 20);

    /* 1. Caminho existente */
    assert(grafo_existe_caminho(&g, 1, 3, &existe) == GRAFO_SUCESSO && existe == true);
    assert(grafo_existe_caminho(&g, 1, 1, &existe) == GRAFO_SUCESSO && existe == true);

    /* 2. Caminho inexistente em componentes separados */
    assert(grafo_existe_caminho(&g, 1, 10, &existe) == GRAFO_SUCESSO && existe == false);
    assert(grafo_existe_caminho(&g, 4, 1, &existe) == GRAFO_SUCESSO && existe == false);

    /* 3. Rejeição de ID ausente (não retorna falso silencioso!) */
    assert(grafo_existe_caminho(&g, 1, 999, &existe) == GRAFO_ERRO_VERTICE_NAO_ENCONTRADO);
    assert(grafo_existe_caminho(&g, 888, 2, &existe) == GRAFO_ERRO_VERTICE_NAO_ENCONTRADO);

    printf("  -> Existencia de Caminho: conexao valida, separacao e rejeicao de ID ausente: OK!\n\n");
}

/* ==========================================================================
 * TESTE 5: Integração BST + Grafo e Rollback na Capacidade Máxima (Seção 4)
 * ========================================================================== */
static void testar_integracao_e_rollback(void) {
    NoBST *catalogo = bst_criar();
    Grafo g;
    Livro removido;
    int i;
    printf("[Teste 5/5] A testar Integracao BST + Grafo com Rollback na Capacidade...\n");

    grafo_inicializar(&g);

    /* 1. Preenche o sistema até atingir exatamente 100 livros */
    for (i = 1; i <= MAX_VERTICES; i++) {
        Livro l;
        l.id = i;
        snprintf(l.titulo, sizeof(l.titulo), "Livro Volume %d", i);
        snprintf(l.autor, sizeof(l.autor), "Autor %d", i);
        l.ano_publicacao = 2000 + (i % 26);

        assert(sistema_cadastrar_livro(&catalogo, &g, l) == GRAFO_SUCESSO);
    }

    assert(bst_contar_nos(catalogo) == MAX_VERTICES);
    assert(g.num_vertices == MAX_VERTICES);

    /* 2. Tenta cadastrar o 101º livro: o Grafo atinge capacidade máxima */
    {
        Livro l101 = { 101, "Livro Extra Transbordante", "Autor Excedente", 2024 };
        StatusGrafo st = sistema_cadastrar_livro(&catalogo, &g, l101);

        assert(st == GRAFO_ERRO_CAPACIDADE_MAXIMA);

        /* PROVA DE ROLLBACK: O livro 101 NÃO pode existir na BST nem no Grafo! */
        assert(bst_buscar(catalogo, 101) == NULL);
        assert(grafo_obter_indice(&g, 101) == -1);

        /* A BST e o Grafo permanecem intactos com 100 registros */
        assert(bst_contar_nos(catalogo) == MAX_VERTICES);
        assert(g.num_vertices == MAX_VERTICES);
    }

    /* 3. Remove o livro ID 50: libera vaga na BST e no Grafo */
    assert(sistema_remover_livro(&catalogo, &g, 50, &removido) == GRAFO_SUCESSO);
    assert(removido.id == 50);
    assert(bst_contar_nos(catalogo) == MAX_VERTICES - 1);
    assert(g.num_vertices == MAX_VERTICES - 1);

    /* 4. Agora o 101º livro pode ser cadastrado ocupando o slot vago */
    {
        Livro l101 = { 101, "Livro Extra Transbordante", "Autor Excedente", 2024 };
        assert(sistema_cadastrar_livro(&catalogo, &g, l101) == GRAFO_SUCESSO);

        assert(bst_buscar(catalogo, 101) != NULL);
        assert(grafo_obter_indice(&g, 101) != -1);
        assert(bst_contar_nos(catalogo) == MAX_VERTICES);
        assert(g.num_vertices == MAX_VERTICES);
    }

    bst_liberar(catalogo);
    grafo_liberar(&g);
    printf("  -> Integracao BST + Grafo e Rollback garantido: OK!\n\n");
}

int main(void) {
    printf("========================================================\n");
    printf("   EXECUCAO DOS TESTES AUTOMATIZADOS - ETAPA 4          \n");
    printf("========================================================\n\n");

    testar_grafo_regras_basicas_r08();
    testar_remocao_vertice_e_limpeza();
    testar_bfs_ordem_crescente_de_id();
    testar_existencia_caminho();
    testar_integracao_e_rollback();

    printf("========================================================\n");
    printf("   TODOS OS TESTES DA ETAPA 4 PASSARAM COM SUCESSO!     \n");
    printf("========================================================\n");
    return EXIT_SUCCESS;
}


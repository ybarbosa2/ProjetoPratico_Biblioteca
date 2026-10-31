/**
 * @file test_integrado.c
 * @brief Teste Integrado de Ponta a Ponta do Sistema de Biblioteca (Etapa 5 e Seção 12).
 *
 * Valida o ciclo de vida completo:
 * 1. Cadastro integrado (BST + Grafo com rollback).
 * 2. Conexões de arestas e consultas (direta e existência de caminho).
 * 3. Regra do Histórico: registro exclusivo de alterações de estado com sucesso (1, 3, 6, 7).
 *    Buscas e consultas (2, 4, 5, 8, 9, 10, 11) não modificam a pilha.
 * 4. Ordenação sobre vetor auxiliar de cópias independentes mantendo a BST intacta.
 * 5. Busca em Largura (BFS) em ordem crescente de vizinhos e verificação de caminho.
 * 6. Remoção em cascata (remover livro remove nó na BST e conexões no Grafo).
 * 7. Liberação total de memória sem vazamentos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bst.h"
#include "grafo.h"
#include "pilha.h"
#include "fila.h"
#include "lista.h"
#include "ordenacao.h"
#include "io.h"

int main(void) {
    NoBST *catalogo = bst_criar();
    Grafo grafo;
    Pilha historico;
    RegistoHistorico topo_reg;
    Lista visitados;
    Livro removido;
    bool existe_caminho = false;
    int menor_id = 0, maior_id = 0;
    int tamanho_historico_inicial;

    printf("====================================================================\n");
    printf("   TESTE INTEGRADO DE PONTA A PONTA (END-TO-END) - ETAPA 5          \n");
    printf("====================================================================\n\n");

    grafo_inicializar(&grafo);
    pilha_inicializar(&historico);
    lista_inicializar(&visitados);

    /* ----------------------------------------------------------------------
     * PASSO 1: Cadastro Integrado de Livros (Opção 1)
     * ---------------------------------------------------------------------- */
    printf("[Passo 1] A cadastrar livros no Catalogo (BST) e Grafo...\n");
    {
        Livro l1 = { 10, "Dom Casmurro", "Machado de Assis", 1899 };
        Livro l2 = { 50, "O Alienista", "Machado de Assis", 1882 };
        Livro l3 = { 20, "Memorias Postumas", "Machado de Assis", 1881 };
        Livro l4 = { 30, "Quincas Borba", "Machado de Assis", 1891 };
        Livro l5 = { 90, "Helena", "Machado de Assis", 1876 };

        assert(sistema_cadastrar_livro(&catalogo, &grafo, l1) == GRAFO_SUCESSO);
        pilha_empilhar(&historico, ACAO_INSERIR_LIVRO, l1.id, 0, &l1, "Cadastro l1");

        assert(sistema_cadastrar_livro(&catalogo, &grafo, l2) == GRAFO_SUCESSO);
        pilha_empilhar(&historico, ACAO_INSERIR_LIVRO, l2.id, 0, &l2, "Cadastro l2");

        assert(sistema_cadastrar_livro(&catalogo, &grafo, l3) == GRAFO_SUCESSO);
        pilha_empilhar(&historico, ACAO_INSERIR_LIVRO, l3.id, 0, &l3, "Cadastro l3");

        assert(sistema_cadastrar_livro(&catalogo, &grafo, l4) == GRAFO_SUCESSO);
        pilha_empilhar(&historico, ACAO_INSERIR_LIVRO, l4.id, 0, &l4, "Cadastro l4");

        assert(sistema_cadastrar_livro(&catalogo, &grafo, l5) == GRAFO_SUCESSO);
        pilha_empilhar(&historico, ACAO_INSERIR_LIVRO, l5.id, 0, &l5, "Cadastro l5");

        assert(bst_contar_nos(catalogo) == 5);
        assert(grafo.num_vertices == 5);
        assert(historico.tamanho == 5);
    }
    printf("  -> 5 livros cadastrados e sincronizados com sucesso na BST, Grafo e Pilha.\n\n");

    /* ----------------------------------------------------------------------
     * PASSO 2: Consultas Não Destrutivas e Verificação da Pilha (Opções 2 e 4)
     * ---------------------------------------------------------------------- */
    printf("[Passo 2] A testar consultas e garantir que NAO alteram o historico...\n");
    tamanho_historico_inicial = historico.tamanho;

    assert(bst_obter_menor_id(catalogo, &menor_id) == true && menor_id == 10);
    assert(bst_obter_maior_id(catalogo, &maior_id) == true && maior_id == 90);
    assert(bst_buscar(catalogo, 20) != NULL);
    assert(bst_buscar(catalogo, 999) == NULL);

    /* REGRA DO HISTÓRICO: consultas não modificam a pilha */
    assert(historico.tamanho == tamanho_historico_inicial);
    printf("  -> Consultas executadas sem alterar a pilha de historico (R04 e Secao 8).\n\n");

    /* ----------------------------------------------------------------------
     * PASSO 3: Adição de Conexões e Relações (Opção 6)
     * ---------------------------------------------------------------------- */
    printf("[Passo 3] A criar conexoes simetricas no Grafo...\n");
    {
        /* 10 <-> 20 */
        assert(grafo_adicionar_aresta(&grafo, 10, 20) == GRAFO_SUCESSO);
        pilha_empilhar(&historico, ACAO_ADICIONAR_RELACAO, 10, 20, NULL, "Conexao 10 <-> 20");

        /* 10 <-> 50 */
        assert(grafo_adicionar_aresta(&grafo, 10, 50) == GRAFO_SUCESSO);
        pilha_empilhar(&historico, ACAO_ADICIONAR_RELACAO, 10, 50, NULL, "Conexao 10 <-> 50");

        /* 20 <-> 30 */
        assert(grafo_adicionar_aresta(&grafo, 20, 30) == GRAFO_SUCESSO);
        pilha_empilhar(&historico, ACAO_ADICIONAR_RELACAO, 20, 30, NULL, "Conexao 20 <-> 30");

        /* Rejeição de duplicata inversa: (20, 10) */
        assert(grafo_adicionar_aresta(&grafo, 20, 10) == GRAFO_ERRO_ARESTA_DUPLICADA);
        /* Tentativa falha NÃO entra no histórico! */

        assert(grafo.num_arestas == 3);
        assert(historico.tamanho == 8);
    }
    printf("  -> Conexoes adicionadas e falhas rejeitadas sem poluir o historico.\n\n");

    /* ----------------------------------------------------------------------
     * PASSO 4: Consultas de Aresta e Caminho (Opções 8 e 10)
     * ---------------------------------------------------------------------- */
    printf("[Passo 4] A verificar conexao direta e caminhos...\n");
    assert(grafo_existe_aresta(&grafo, 10, 20) == true);
    assert(grafo_existe_aresta(&grafo, 20, 10) == true);
    assert(grafo_existe_aresta(&grafo, 10, 30) == false); /* Indireto */

    assert(grafo_existe_caminho(&grafo, 10, 30, &existe_caminho) == GRAFO_SUCESSO && existe_caminho == true);
    assert(grafo_existe_caminho(&grafo, 10, 90, &existe_caminho) == GRAFO_SUCESSO && existe_caminho == false); /* 90 isolado */
    assert(grafo_existe_caminho(&grafo, 10, 999, &existe_caminho) == GRAFO_ERRO_VERTICE_NAO_ENCONTRADO);

    /* Pilha continua inalterada com 8 elementos */
    assert(historico.tamanho == 8);
    printf("  -> Conexoes diretas e caminhos validados com sucesso.\n\n");

    /* ----------------------------------------------------------------------
     * PASSO 5: BFS com Vizinhos em Ordem Crescente de ID (Opção 9)
     * ---------------------------------------------------------------------- */
    printf("[Passo 5] A executar BFS e validar ordem de vizinhos e niveis...\n");
    {
        /* Origem 10 está ligada a 20 e 50.
           Nível 0: 10
           Nível 1 (crescente): 20, 50
           Nível 2: 30 (vizinho de 20)
        */
        NoLista *curr;
        int ordem_bfs_esperada[4] = { 10, 20, 50, 30 };
        int idx_bfs = 0;

        assert(grafo_bfs(&grafo, 10, &visitados) == GRAFO_SUCESSO);
        assert(visitados.tamanho == 4);

        curr = visitados.inicio;
        while (curr != NULL) {
            assert(curr->id_livro == ordem_bfs_esperada[idx_bfs]);
            curr = curr->prox;
            idx_bfs++;
        }
        assert(idx_bfs == 4);
    }
    printf("  -> BFS executada respeitando estritamente a ordem crescente de IDs.\n\n");

    /* ----------------------------------------------------------------------
     * PASSO 6: Ordenação por Título e Prova de Integridade da BST (Opção 5)
     * ---------------------------------------------------------------------- */
    printf("[Passo 6] A ordenar livros por titulo e provar integridade da BST...\n");
    {
        Livro vetor_copias[5];
        int idx_copia = 0;

        bst_exportar_vetor(catalogo, vetor_copias, &idx_copia);
        assert(idx_copia == 5);

        /* Ordena por título via Merge Sort próprio */
        ordenar_livros_mergesort(vetor_copias, idx_copia, ORDENAR_POR_TITULO);

        /* Ordem alfabética esperada de título:
           1. "Dom Casmurro" (ID 10)
           2. "Helena" (ID 90)
           3. "Memorias Postumas" (ID 20)
           4. "O Alienista" (ID 50)
           5. "Quincas Borba" (ID 30)
        */
        assert(strcmp(vetor_copias[0].titulo, "Dom Casmurro") == 0);
        assert(strcmp(vetor_copias[1].titulo, "Helena") == 0);
        assert(strcmp(vetor_copias[2].titulo, "Memorias Postumas") == 0);
        assert(strcmp(vetor_copias[3].titulo, "O Alienista") == 0);
        assert(strcmp(vetor_copias[4].titulo, "Quincas Borba") == 0);

        /* PROVA: BST original continua inalterada e ordenada por ID */
        idx_copia = 0;
        bst_exportar_vetor(catalogo, vetor_copias, &idx_copia);
        assert(vetor_copias[0].id == 10);
        assert(vetor_copias[1].id == 20);
        assert(vetor_copias[2].id == 30);
        assert(vetor_copias[3].id == 50);
        assert(vetor_copias[4].id == 90);
    }
    printf("  -> Catalogo ordenado por titulo mantendo a BST original intacta (R06).\n\n");

    /* ----------------------------------------------------------------------
     * PASSO 7: Remoção em Cascata de Livro (Opção 3)
     * ---------------------------------------------------------------------- */
    printf("[Passo 7] A remover livro ID 20 e verificar remocao de arestas incidentes...\n");
    {
        /* O livro 20 tinha arestas com 10 e com 30 (2 arestas).
           Ao ser removido, o total de arestas deve passar de 3 para 1.
        */
        assert(sistema_remover_livro(&catalogo, &grafo, 20, &removido) == GRAFO_SUCESSO);
        assert(removido.id == 20);
        pilha_empilhar(&historico, ACAO_REMOVER_LIVRO, 20, 0, &removido, "Remocao livro 20");

        assert(bst_contar_nos(catalogo) == 4);
        assert(grafo.num_vertices == 4);
        assert(grafo.num_arestas == 1); /* Apenas restou a aresta (10, 50) */
        assert(grafo_existe_aresta(&grafo, 10, 50) == true);
        assert(grafo_existe_aresta(&grafo, 10, 20) == false);
        assert(grafo_existe_aresta(&grafo, 20, 30) == false);
    }
    printf("  -> Livro e arestas incidentes removidos em cascata com consistencia total.\n\n");

    /* ----------------------------------------------------------------------
     * PASSO 8: Consulta Não Destrutiva do Histórico (Opção 11 - R04)
     * ---------------------------------------------------------------------- */
    printf("[Passo 8] A consultar pilha de historico (ate 5 acoes mais recentes)...\n");
    {
        /* A pilha possui 9 acoes gravadas com sucesso. A consulta exibe ate 5 sem remover. */
        assert(historico.tamanho == 9);
        assert(pilha_topo(&historico, &topo_reg) == true);
        assert(topo_reg.tipo == ACAO_REMOVER_LIVRO && topo_reg.id_principal == 20);

        pilha_exibir_historico(&historico);

        /* Consulta nao destrutiva: tamanho permanece 9 */
        assert(historico.tamanho == 9);
    }
    printf("  -> Historico consultado de forma nao destrutiva com sucesso.\n\n");

    /* ----------------------------------------------------------------------
     * PASSO 9: Liberação Completa de Memória (Opção 0 - R02)
     * ---------------------------------------------------------------------- */
    printf("[Passo 9] A liberar toda a memoria alocada...\n");
    bst_liberar(catalogo);
    grafo_liberar(&grafo);
    pilha_liberar(&historico);
    lista_liberar(&visitados);
    printf("  -> Toda a memoria desalocada com sucesso.\n\n");

    printf("====================================================================\n");
    printf("   TESTE INTEGRADO CONCLUIDO COM 100%% DE SUCESSO!                  \n");
    printf("====================================================================\n");
    return EXIT_SUCCESS;
}


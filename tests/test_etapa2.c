/**
 * @file test_etapa2.c
 * @brief Testes automatizados da Etapa 2 (Lista R03, Pilha R04, Fila R05 e Ordenação R06).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "lista.h"
#include "fila.h"
#include "pilha.h"
#include "ordenacao.h"

static void testar_lista_r03(void) {
    Lista l;
    printf("[Teste 1/4] A testar Lista Encadeada Propria (R03)...\n");

    lista_inicializar(&l);
    assert(lista_vazia(&l) == true);
    assert(l.tamanho == 0);

    /* Insercao no fim mantendo ordem de descoberta da BFS */
    assert(lista_inserir_fim(&l, 10, 0) == true);
    assert(lista_inserir_fim(&l, 25, 1) == true);
    assert(lista_inserir_fim(&l, 40, 1) == true);
    assert(lista_inserir_fim(&l, 55, 2) == true);

    assert(lista_vazia(&l) == false);
    assert(l.tamanho == 4);
    assert(l.inicio->id_livro == 10);
    assert(l.fim->id_livro == 55);

    assert(lista_contem(&l, 25) == true);
    assert(lista_contem(&l, 99) == false);

    lista_exibir(&l);

    /* Remocao do fim e do inicio */
    assert(lista_remover(&l, 55) == true);
    assert(l.fim->id_livro == 40);
    assert(lista_remover(&l, 10) == true);
    assert(l.inicio->id_livro == 25);
    assert(l.tamanho == 2);

    lista_liberar(&l);
    assert(lista_vazia(&l) == true);
    assert(l.tamanho == 0);
    printf("  -> Lista Encadeada (R03): OK!\n\n");
}

static void testar_fila_r05(void) {
    Fila f;
    int id = 0;
    printf("[Teste 2/4] A testar Fila Dinamica Encadeada Propria (R05)...\n");

    fila_inicializar(&f);
    assert(fila_vazia(&f) == true);

    assert(fila_enfileirar(&f, 101) == true);
    assert(fila_enfileirar(&f, 202) == true);
    assert(fila_enfileirar(&f, 303) == true);
    assert(f.tamanho == 3);

    assert(fila_frente(&f, &id) == true && id == 101);
    assert(fila_desenfileirar(&f, &id) == true && id == 101);
    assert(fila_desenfileirar(&f, &id) == true && id == 202);
    assert(fila_desenfileirar(&f, &id) == true && id == 303);
    assert(fila_vazia(&f) == true);
    assert(fila_desenfileirar(&f, &id) == false);

    assert(fila_enfileirar(&f, 404) == true);
    fila_liberar(&f);
    assert(fila_vazia(&f) == true);
    printf("  -> Fila Dinamica (R05): OK!\n\n");
}

static void testar_pilha_r04(void) {
    Pilha p;
    RegistoHistorico reg;
    printf("[Teste 3/4] A testar Pilha Dinamica de Historico (R04)...\n");

    pilha_inicializar(&p);
    assert(pilha_vazia(&p) == true);

    /* Caso 1: Pilha vazia */
    pilha_exibir_historico(&p);

    /* Caso 2: Menos de 5 acoes (3 acoes) */
    assert(pilha_empilhar(&p, ACAO_INSERIR_LIVRO, 10, 0, NULL, "Cadastro livro ID 10") == true);
    assert(pilha_empilhar(&p, ACAO_INSERIR_LIVRO, 20, 0, NULL, "Cadastro livro ID 20") == true);
    assert(pilha_empilhar(&p, ACAO_ADICIONAR_RELACAO, 10, 20, NULL, "Conexao 10 <-> 20") == true);
    assert(p.tamanho == 3);
    pilha_exibir_historico(&p);
    /* Garante que a consulta nao removeu elementos */
    assert(p.tamanho == 3);

    /* Caso 3: Mais de 5 acoes (7 acoes no total -> deve mostrar apenas as 5 mais recentes) */
    assert(pilha_empilhar(&p, ACAO_INSERIR_LIVRO, 30, 0, NULL, "Cadastro livro ID 30") == true);
    assert(pilha_empilhar(&p, ACAO_ADICIONAR_RELACAO, 20, 30, NULL, "Conexao 20 <-> 30") == true);
    assert(pilha_empilhar(&p, ACAO_REMOVER_RELACAO, 10, 20, NULL, "Remocao conexao 10 <-> 20") == true);
    assert(pilha_empilhar(&p, ACAO_REMOVER_LIVRO, 10, 0, NULL, "Remocao livro ID 10") == true);
    assert(p.tamanho == 7);

    pilha_exibir_historico(&p);
    /* Consulta nao destrutiva: tamanho permanece 7 e topo continua a ser a ultima acao */
    assert(p.tamanho == 7);
    assert(pilha_topo(&p, &reg) == true);
    assert(reg.tipo == ACAO_REMOVER_LIVRO && reg.id_principal == 10);

    assert(pilha_desempilhar(&p, &reg) == true);
    assert(reg.tipo == ACAO_REMOVER_LIVRO && reg.id_principal == 10);
    assert(p.tamanho == 6);

    pilha_liberar(&p);
    assert(pilha_vazia(&p) == true);
    assert(p.tamanho == 0);
    printf("  -> Pilha de Historico (R04): OK!\n\n");
}

static void testar_ordenacao_r06(void) {
    Livro dados_originais[6] = {
        { 50, "Estruturas de Dados", "Cormen", 2022 },
        { 12, "Algoritmos em C", "Sedgewick", 2015 },
        { 30, "Algoritmos em C", "Sedgewick", 2010 }, /* Mesmo titulo que ID 12: desempate por ID */
        { 5,  "Algoritmos em C", "Ziviani", 2018 },   /* Mesmo titulo que ID 12 e 30: menor ID (5) */
        { 40, "Compiladores", "Aho", 2008 },
        { 8,  "programacao em C", "Ritchie", 1988 }   /* Minuscula ('p' > 'E' na tabela ASCII via strcmp) */
    };
    Livro copia_merge[6];
    Livro copia_quick[6];
    int ids_esperados[6] = { 5, 12, 30, 40, 50, 8 };
    int i;

    printf("[Teste 4/4] A testar Algoritmos de Ordenacao Proprios (R06)...\n");

    memcpy(copia_merge, dados_originais, sizeof(dados_originais));
    memcpy(copia_quick, dados_originais, sizeof(dados_originais));

    ordenar_livros_mergesort(copia_merge, 6, ORDENAR_POR_TITULO);
    ordenar_livros_quicksort(copia_quick, 6, ORDENAR_POR_TITULO);

    for (i = 0; i < 6; i++) {
        livro_exibir(&copia_merge[i]);
        assert(copia_merge[i].id == ids_esperados[i]);
        assert(copia_quick[i].id == ids_esperados[i]);
    }

    printf("  -> Ordenacao Merge Sort e Quick Sort c/ strcmp e desempate por ID (R06): OK!\n\n");
}

int main(void) {
    printf("========================================================\n");
    printf("   EXECUCAO DOS TESTES AUTOMATIZADOS - ETAPA 2          \n");
    printf("========================================================\n\n");

    testar_lista_r03();
    testar_fila_r05();
    testar_pilha_r04();
    testar_ordenacao_r06();

    printf("========================================================\n");
    printf("   TODOS OS TESTES DA ETAPA 2 PASSARAM COM SUCESSO!     \n");
    printf("========================================================\n");
    return EXIT_SUCCESS;
}


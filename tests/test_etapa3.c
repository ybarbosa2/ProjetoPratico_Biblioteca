/**
 * @file test_etapa3.c
 * @brief Testes automatizados da Etapa 3 (BST - R07, Validação de Entradas - R01 e Integração c/ Ordenação - R06).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "bst.h"
#include "ordenacao.h"
#include "io.h"

/* ==========================================================================
 * TESTE A: Árvore Binária de Busca (BST) - Requisito R07
 * ========================================================================== */
static void testar_bst_r07(void) {
    NoBST *raiz = bst_criar();
    Livro l1 = { 50, "Engenharia de Software", "Pressman", 2014 };
    Livro l2 = { 30, "Codigo Limpo", "Robert Martin", 2009 };
    Livro l3 = { 70, "Padroes de Projetos", "GoF", 2000 };
    Livro l4 = { 20, "O Programador Pragmatico", "Hunt", 1999 };
    Livro l5 = { 40, "Refatoracao", "Fowler", 2012 };
    Livro l6 = { 60, "Algoritmos", "Sedgewick", 2011 };
    Livro l7 = { 80, "Sistemas Operacionais", "Tanenbaum", 2016 };

    Livro invalido_id_zero = { 0, "Livro Zero", "Autor", 2020 };
    Livro invalido_id_neg = { -5, "Livro Negativo", "Autor", 2020 };
    Livro duplicado_id_50 = { 50, "Engenharia Nova", "Outro", 2021 };

    Livro removido;
    int menor = 0, maior = 0;
    NoBST *busca = NULL;

    printf("[Teste 1/3] A testar Arvore Binaria de Busca (BST - R07)...\n");

    /* 1. Inserções válidas */
    assert(bst_inserir(&raiz, l1) == BST_SUCESSO);
    assert(bst_inserir(&raiz, l2) == BST_SUCESSO);
    assert(bst_inserir(&raiz, l3) == BST_SUCESSO);
    assert(bst_inserir(&raiz, l4) == BST_SUCESSO);
    assert(bst_inserir(&raiz, l5) == BST_SUCESSO);
    assert(bst_inserir(&raiz, l6) == BST_SUCESSO);
    assert(bst_inserir(&raiz, l7) == BST_SUCESSO);
    assert(bst_contar_nos(raiz) == 7);

    /* 2. Rejeição de IDs inválidos e duplicados com códigos claros */
    assert(bst_inserir(&raiz, invalido_id_zero) == BST_ERRO_ID_INVALIDO);
    assert(bst_inserir(&raiz, invalido_id_neg) == BST_ERRO_ID_INVALIDO);
    assert(bst_inserir(&raiz, duplicado_id_50) == BST_ERRO_DUPLICADO);
    assert(bst_contar_nos(raiz) == 7); /* Permanece com 7 nós */

    /* 3. Consultas de menor e maior ID */
    assert(bst_obter_menor_id(raiz, &menor) == true && menor == 20);
    assert(bst_obter_maior_id(raiz, &maior) == true && maior == 80);

    /* 4. Busca por ID */
    busca = bst_buscar(raiz, 40);
    assert(busca != NULL && busca->livro.id == 40 && strcmp(busca->livro.titulo, "Refatoracao") == 0);
    assert(bst_buscar(raiz, 999) == NULL);
    assert(bst_buscar(raiz, -1) == NULL);

    /* 5. Remoção de ID inexistente */
    assert(bst_remover(&raiz, 999, &removido) == BST_ERRO_NAO_ENCONTRADO);
    assert(bst_remover(&raiz, 0, &removido) == BST_ERRO_ID_INVALIDO);
    assert(bst_contar_nos(raiz) == 7);

    /* 6. Remoção - Caso 1: Nó folha (0 filhos) -> remover nó 20 */
    assert(bst_remover(&raiz, 20, &removido) == BST_SUCESSO);
    assert(removido.id == 20);
    assert(bst_buscar(raiz, 20) == NULL);
    assert(bst_contar_nos(raiz) == 6);
    assert(bst_obter_menor_id(raiz, &menor) == true && menor == 30);

    /* 7. Remoção - Caso 2: Nó com 1 filho -> nó 30 agora só tem o filho 40 */
    assert(bst_remover(&raiz, 30, &removido) == BST_SUCESSO);
    assert(removido.id == 30);
    assert(bst_buscar(raiz, 30) == NULL);
    assert(bst_buscar(raiz, 40) != NULL); /* 40 assumiu a subárvore */
    assert(bst_contar_nos(raiz) == 5);

    /* 8. Remoção - Caso 3: Nó com 2 filhos (incluindo a RAIZ 50!) */
    /* A raiz 50 tem subárvore esquerda (40) e direita (70 com filhos 60 e 80). */
    /* O sucessor em-ordem de 50 é 60. 60 deve assumir a nova raiz. */
    assert(bst_remover(&raiz, 50, &removido) == BST_SUCESSO);
    assert(removido.id == 50);
    assert(raiz->livro.id == 60); /* Sucessor virou a nova raiz */
    assert(bst_buscar(raiz, 50) == NULL);
    assert(bst_buscar(raiz, 60) != NULL);
    assert(bst_contar_nos(raiz) == 4);

    /* Valida menor e maior após as remoções */
    assert(bst_obter_menor_id(raiz, &menor) == true && menor == 40);
    assert(bst_obter_maior_id(raiz, &maior) == true && maior == 80);

    bst_liberar(raiz);
    printf("  -> BST (R07 - Insercao, Busca, 3 Casos de Remocao, Menor/Maior): OK!\n\n");
}

/* ==========================================================================
 * TESTE B: Integração BST + Ordenação (Seção 7 e R06)
 * ========================================================================== */
static void testar_integracao_bst_ordenacao(void) {
    NoBST *raiz = bst_criar();
    Livro livros[5] = {
        { 42, "Zadig", "Voltaire", 1747 },
        { 15, "A Arte da Guerra", "Sun Tzu", 500 },
        { 88, "Dom Casmurro", "Machado de Assis", 1899 },
        { 23, "Dom Casmurro", "Outra Edicao", 1950 }, /* Mesmo titulo, desempate por ID */
        { 10, "1984", "George Orwell", 1949 }
    };
    Livro vetor_aux[5];
    int indice = 0;
    int i;
    int total;

    printf("[Teste 2/3] A testar Integracao BST + Ordenacao Independente...\n");

    for (i = 0; i < 5; i++) {
        assert(bst_inserir(&raiz, livros[i]) == BST_SUCESSO);
    }

    total = bst_contar_nos(raiz);
    assert(total == 5);

    /* Exporta cópias para vetor auxiliar */
    bst_exportar_vetor(raiz, vetor_aux, &indice);
    assert(indice == 5);

    /* Vetor recém exportado em-ordem deve estar ordenado por ID crescente: 10, 15, 23, 42, 88 */
    assert(vetor_aux[0].id == 10);
    assert(vetor_aux[1].id == 15);
    assert(vetor_aux[2].id == 23);
    assert(vetor_aux[3].id == 42);
    assert(vetor_aux[4].id == 88);

    /* Ordena o vetor auxiliar por TITULO com desempate por ID */
    ordenar_livros_mergesort(vetor_aux, indice, ORDENAR_POR_TITULO);

    /* Ordem esperada por título:
       1. "1984" (ID 10)
       2. "A Arte da Guerra" (ID 15)
       3. "Dom Casmurro" (ID 23 - menor ID)
       4. "Dom Casmurro" (ID 88 - maior ID)
       5. "Zadig" (ID 42)
    */
    assert(vetor_aux[0].id == 10 && strcmp(vetor_aux[0].titulo, "1984") == 0);
    assert(vetor_aux[1].id == 15 && strcmp(vetor_aux[1].titulo, "A Arte da Guerra") == 0);
    assert(vetor_aux[2].id == 23 && strcmp(vetor_aux[2].titulo, "Dom Casmurro") == 0);
    assert(vetor_aux[3].id == 88 && strcmp(vetor_aux[3].titulo, "Dom Casmurro") == 0);
    assert(vetor_aux[4].id == 42 && strcmp(vetor_aux[4].titulo, "Zadig") == 0);

    /* PROVA DE INTEGRIDADE: A BST original permanece intacta e indexada por ID! */
    indice = 0;
    bst_exportar_vetor(raiz, vetor_aux, &indice);
    assert(vetor_aux[0].id == 10);
    assert(vetor_aux[1].id == 15);
    assert(vetor_aux[2].id == 23);
    assert(vetor_aux[3].id == 42);
    assert(vetor_aux[4].id == 88);

    bst_liberar(raiz);
    printf("  -> Integracao BST + Ordenacao (Copia independente e arvore intacta): OK!\n\n");
}

/* ==========================================================================
 * TESTE C: Validação Segura de Entradas (Requisito R01 e Seção 9)
 * ========================================================================== */
static void testar_validacao_entradas_r01(void) {
    char texto_longo[100];
    int id_out = 0;
    int ano_out = 0;
    long val_long = 0;

    printf("[Teste 3/3] A testar Validacao Segura de Entradas (R01 e Secao 9)...\n");

    /* 1. Validação de Textos */
    assert(io_validar_texto(NULL, 79) == IO_ERRO_ENTRADA_NULA);
    assert(io_validar_texto("", 79) == IO_ERRO_TEXTO_VAZIO);
    assert(io_validar_texto("   \t  \n  ", 79) == IO_ERRO_APENAS_ESPACOS);

    memset(texto_longo, 'A', 80);
    texto_longo[80] = '\0'; /* 80 caracteres úteis > 79 */
    assert(io_validar_texto(texto_longo, 79) == IO_ERRO_TAMANHO_EXCEDIDO);

    texto_longo[79] = '\0'; /* Exatamente 79 caracteres úteis */
    assert(io_validar_texto(texto_longo, 79) == IO_SUCESSO);

    assert(io_validar_texto("O Cortico", 79) == IO_SUCESSO);

    /* 2. Validação de Inteiros via strtol */
    assert(io_validar_inteiro(NULL, &val_long) == IO_ERRO_ENTRADA_NULA);
    assert(io_validar_inteiro("", &val_long) == IO_ERRO_TEXTO_VAZIO);
    assert(io_validar_inteiro("abc", &val_long) == IO_ERRO_NUMERO_INVALIDO);
    assert(io_validar_inteiro("123abc", &val_long) == IO_ERRO_CARACTERES_EXTRAS);
    assert(io_validar_inteiro("45 67", &val_long) == IO_ERRO_CARACTERES_EXTRAS);
    assert(io_validar_inteiro("999999999999999999999999999999", &val_long) == IO_ERRO_OVERFLOW);
    assert(io_validar_inteiro("  42  ", &val_long) == IO_SUCESSO && val_long == 42);

    /* 3. Validação de ID (> 0) */
    assert(io_validar_id("0", &id_out) == IO_ERRO_ID_INVALIDO);
    assert(io_validar_id("-10", &id_out) == IO_ERRO_ID_INVALIDO);
    assert(io_validar_id("abc", &id_out) == IO_ERRO_NUMERO_INVALIDO);
    assert(io_validar_id("150", &id_out) == IO_SUCESSO && id_out == 150);

    /* 4. Validação de Ano de Publicação (1 a 2026) */
    assert(io_validar_ano("0", &ano_out) == IO_ERRO_ANO_INVALIDO);
    assert(io_validar_ano("-50", &ano_out) == IO_ERRO_ANO_INVALIDO);
    assert(io_validar_ano("2027", &ano_out) == IO_ERRO_ANO_INVALIDO);
    assert(io_validar_ano("1", &ano_out) == IO_SUCESSO && ano_out == 1);
    assert(io_validar_ano("1995", &ano_out) == IO_SUCESSO && ano_out == 1995);
    assert(io_validar_ano("2026", &ano_out) == IO_SUCESSO && ano_out == 2026);

    printf("  -> Validacao Segura de Entradas (R01 e Secao 9): OK!\n\n");
}

int main(void) {
    printf("========================================================\n");
    printf("   EXECUCAO DOS TESTES AUTOMATIZADOS - ETAPA 3          \n");
    printf("========================================================\n\n");

    testar_bst_r07();
    testar_integracao_bst_ordenacao();
    testar_validacao_entradas_r01();

    printf("========================================================\n");
    printf("   TODOS OS TESTES DA ETAPA 3 PASSARAM COM SUCESSO!     \n");
    printf("========================================================\n");
    return EXIT_SUCCESS;
}


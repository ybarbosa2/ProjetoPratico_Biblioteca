/**
 * @file pilha.c
 * @brief Implementação (stubs iniciais - Etapa 1) da Pilha dinâmica de histórico.
 */

#include <stdio.h>
#include <stdlib.h>
#include "pilha.h"

void pilha_inicializar(Pilha *pilha) {
    if (pilha != NULL) {
        pilha->topo = NULL;
        pilha->tamanho = 0;
    }
}

bool pilha_vazia(const Pilha *pilha) {
    return (pilha == NULL || pilha->topo == NULL);
}

bool pilha_empilhar(Pilha *pilha, TipoAcao tipo, int id_principal, int id_secundario,
                    const Livro *livro, const char *descricao) {
    (void)pilha;
    (void)tipo;
    (void)id_principal;
    (void)id_secundario;
    (void)livro;
    (void)descricao;
    /* Stub: será implementado nas próximas etapas */
    return false;
}

bool pilha_desempilhar(Pilha *pilha, RegistoHistorico *registo_removido) {
    (void)pilha;
    (void)registo_removido;
    /* Stub: será implementado nas próximas etapas */
    return false;
}

bool pilha_topo(const Pilha *pilha, RegistoHistorico *registo_topo) {
    (void)pilha;
    (void)registo_topo;
    /* Stub: será implementado nas próximas etapas */
    return false;
}

void pilha_exibir_historico(const Pilha *pilha) {
    (void)pilha;
    /* Stub: será implementado nas próximas etapas */
}

const char* pilha_nome_acao(TipoAcao tipo) {
    (void)tipo;
    /* Stub: será implementado nas próximas etapas */
    return "DESCONHECIDA";
}

void pilha_liberar(Pilha *pilha) {
    (void)pilha;
    /* Stub: será implementado nas próximas etapas */
}


/**
 * @file pilha.c
 * @brief Implementação da Pilha dinâmica encadeada própria de histórico (Requisito R04).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

const char* pilha_nome_acao(TipoAcao tipo) {
    switch (tipo) {
        case ACAO_INSERIR_LIVRO:
            return "CADASTRO_LIVRO";
        case ACAO_REMOVER_LIVRO:
            return "REMOCAO_LIVRO";
        case ACAO_ADICIONAR_RELACAO:
            return "ADICAO_CONEXAO";
        case ACAO_REMOVER_RELACAO:
            return "REMOCAO_CONEXAO";
        default:
            return "ACAO_DESCONHECIDA";
    }
}

bool pilha_empilhar(Pilha *pilha, TipoAcao tipo, int id_principal, int id_secundario,
                    const Livro *livro, const char *descricao) {
    NoPilha *novo;

    if (pilha == NULL) {
        return false;
    }

    novo = (NoPilha *)malloc(sizeof(NoPilha));
    if (novo == NULL) {
        return false;
    }

    novo->registo.tipo = tipo;
    novo->registo.id_principal = id_principal;
    novo->registo.id_secundario = id_secundario;

    if (livro != NULL) {
        novo->registo.snapshot_livro = *livro;
    } else {
        memset(&novo->registo.snapshot_livro, 0, sizeof(Livro));
    }

    if (descricao != NULL) {
        strncpy(novo->registo.descricao, descricao, sizeof(novo->registo.descricao) - 1);
        novo->registo.descricao[sizeof(novo->registo.descricao) - 1] = '\0';
    } else {
        novo->registo.descricao[0] = '\0';
    }

    novo->prox = pilha->topo;
    pilha->topo = novo;
    pilha->tamanho++;
    return true;
}

bool pilha_desempilhar(Pilha *pilha, RegistoHistorico *registo_removido) {
    NoPilha *removido;

    if (pilha_vazia(pilha)) {
        return false;
    }

    removido = pilha->topo;
    if (registo_removido != NULL) {
        *registo_removido = removido->registo;
    }

    pilha->topo = removido->prox;
    free(removido);
    pilha->tamanho--;
    return true;
}

bool pilha_topo(const Pilha *pilha, RegistoHistorico *registo_topo) {
    if (pilha_vazia(pilha)) {
        return false;
    }

    if (registo_topo != NULL) {
        *registo_topo = pilha->topo->registo;
    }
    return true;
}

void pilha_exibir_historico(const Pilha *pilha) {
    const NoPilha *atual;
    int contador = 0;

    if (pilha_vazia(pilha)) {
        printf("\n[Historico] Nenhuma alteracao de estado registada ate ao momento (pilha vazia).\n");
        return;
    }

    printf("\n--- Historico de Alteracoes ( ate %d mais recentes | Total na pilha: %d ) ---\n",
           MAX_HISTORICO_EXIBICAO, pilha->tamanho);

    atual = pilha->topo;
    while (atual != NULL && contador < MAX_HISTORICO_EXIBICAO) {
        const RegistoHistorico *reg = &atual->registo;
        contador++;

        if (reg->tipo == ACAO_ADICIONAR_RELACAO || reg->tipo == ACAO_REMOVER_RELACAO) {
            printf(" [%d] Acao: %-16s | IDs: (%d <-> %d)",
                   contador, pilha_nome_acao(reg->tipo),
                   reg->id_principal, reg->id_secundario);
        } else {
            printf(" [%d] Acao: %-16s | ID: %d",
                   contador, pilha_nome_acao(reg->tipo),
                   reg->id_principal);
        }

        if (reg->descricao[0] != '\0') {
            printf(" | %s", reg->descricao);
        }
        printf("\n");

        atual = atual->prox;
    }
    printf("----------------------------------------------------------------------------\n");
}

void pilha_liberar(Pilha *pilha) {
    NoPilha *atual;
    NoPilha *prox;

    if (pilha == NULL) {
        return;
    }

    atual = pilha->topo;
    while (atual != NULL) {
        prox = atual->prox;
        free(atual);
        atual = prox;
    }

    pilha->topo = NULL;
    pilha->tamanho = 0;
}

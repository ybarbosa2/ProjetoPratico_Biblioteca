/**
 * @file bst.c
 * @brief Implementação completa da Árvore Binária de Busca (BST) (Requisito R07).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst.h"
#include "io.h"

NoBST* bst_criar(void) {
    return NULL;
}

const char* bst_status_mensagem(StatusBST status) {
    switch (status) {
        case BST_SUCESSO:
            return "Operacao realizada com sucesso.";
        case BST_ERRO_ID_INVALIDO:
            return "ID invalido: deve ser um numero inteiro positivo (> 0).";
        case BST_ERRO_DUPLICADO:
            return "ID duplicado: ja existe um livro cadastrado com este identificador.";
        case BST_ERRO_MEMORIA:
            return "Falha de memoria: nao foi possivel alocar memoria para o no da BST.";
        case BST_ERRO_NAO_ENCONTRADO:
            return "Livro nao encontrado: o ID informado nao existe na arvore.";
        case BST_ERRO_DADOS_INVALIDOS:
            return "Dados invalidos: campos do livro nao atendem as regras do dominio.";
        default:
            return "Erro desconhecido na BST.";
    }
}

bool livro_validar(Livro livro) {
    if (io_validar_id_valor(livro.id) != IO_SUCESSO) {
        return false;
    }
    if (io_validar_texto(livro.titulo, TAM_TEXTO - 1) != IO_SUCESSO) {
        return false;
    }
    if (io_validar_texto(livro.autor, TAM_TEXTO - 1) != IO_SUCESSO) {
        return false;
    }
    if (io_validar_ano_valor(livro.ano_publicacao) != IO_SUCESSO) {
        return false;
    }
    return true;
}

StatusBST bst_inserir(NoBST **raiz, Livro novo_livro) {
    if (raiz == NULL) {
        return BST_ERRO_DADOS_INVALIDOS;
    }

    if (novo_livro.id <= 0) {
        return BST_ERRO_ID_INVALIDO;
    }

    if (!livro_validar(novo_livro)) {
        return BST_ERRO_DADOS_INVALIDOS;
    }

    if (*raiz == NULL) {
        NoBST *novo = (NoBST *)malloc(sizeof(NoBST));
        if (novo == NULL) {
            return BST_ERRO_MEMORIA;
        }
        novo->livro = novo_livro;
        novo->esq = NULL;
        novo->dir = NULL;
        *raiz = novo;
        return BST_SUCESSO;
    }

    if (novo_livro.id == (*raiz)->livro.id) {
        return BST_ERRO_DUPLICADO;
    } else if (novo_livro.id < (*raiz)->livro.id) {
        return bst_inserir(&((*raiz)->esq), novo_livro);
    } else {
        return bst_inserir(&((*raiz)->dir), novo_livro);
    }
}

NoBST* bst_buscar(NoBST *raiz, int id) {
    if (raiz == NULL || id <= 0) {
        return NULL;
    }

    if (raiz->livro.id == id) {
        return raiz;
    }

    if (id < raiz->livro.id) {
        return bst_buscar(raiz->esq, id);
    } else {
        return bst_buscar(raiz->dir, id);
    }
}

NoBST* bst_minimo(NoBST *raiz) {
    if (raiz == NULL) {
        return NULL;
    }
    while (raiz->esq != NULL) {
        raiz = raiz->esq;
    }
    return raiz;
}

NoBST* bst_maximo(NoBST *raiz) {
    if (raiz == NULL) {
        return NULL;
    }
    while (raiz->dir != NULL) {
        raiz = raiz->dir;
    }
    return raiz;
}

bool bst_obter_menor_id(NoBST *raiz, int *menor_id) {
    NoBST *min_no = bst_minimo(raiz);
    if (min_no == NULL) {
        return false;
    }
    if (menor_id != NULL) {
        *menor_id = min_no->livro.id;
    }
    return true;
}

bool bst_obter_maior_id(NoBST *raiz, int *maior_id) {
    NoBST *max_no = bst_maximo(raiz);
    if (max_no == NULL) {
        return false;
    }
    if (maior_id != NULL) {
        *maior_id = max_no->livro.id;
    }
    return true;
}

StatusBST bst_remover(NoBST **raiz, int id, Livro *livro_removido) {
    NoBST *atual;

    if (raiz == NULL || *raiz == NULL) {
        return BST_ERRO_NAO_ENCONTRADO;
    }

    if (id <= 0) {
        return BST_ERRO_ID_INVALIDO;
    }

    if (id < (*raiz)->livro.id) {
        return bst_remover(&((*raiz)->esq), id, livro_removido);
    } else if (id > (*raiz)->livro.id) {
        return bst_remover(&((*raiz)->dir), id, livro_removido);
    }

    /* Nó com o ID procurado foi encontrado */
    atual = *raiz;
    if (livro_removido != NULL) {
        *livro_removido = atual->livro;
    }

    /* Caso 1: Nó folha (0 filhos) */
    if (atual->esq == NULL && atual->dir == NULL) {
        free(atual);
        *raiz = NULL;
        return BST_SUCESSO;
    }
    /* Caso 2a: Nó possui apenas filho à direita */
    else if (atual->esq == NULL) {
        *raiz = atual->dir;
        free(atual);
        return BST_SUCESSO;
    }
    /* Caso 2b: Nó possui apenas filho à esquerda */
    else if (atual->dir == NULL) {
        *raiz = atual->esq;
        free(atual);
        return BST_SUCESSO;
    }
    /* Caso 3: Nó possui 2 filhos */
    else {
        /* Localiza o sucessor em-ordem (menor elemento da subárvore direita) */
        NoBST *sucessor = bst_minimo(atual->dir);
        Livro dados_sucessor = sucessor->livro;

        /* Remove o nó sucessor da subárvore direita (cai no caso 1 ou 2) */
        bst_remover(&atual->dir, sucessor->livro.id, NULL);

        /* Substitui o conteúdo do nó atual pelos dados do sucessor */
        atual->livro = dados_sucessor;
        return BST_SUCESSO;
    }
}

void bst_percorrer_em_ordem(NoBST *raiz) {
    if (raiz == NULL) {
        return;
    }
    bst_percorrer_em_ordem(raiz->esq);
    livro_exibir(&raiz->livro);
    bst_percorrer_em_ordem(raiz->dir);
}

void bst_percorrer_pre_ordem(NoBST *raiz) {
    if (raiz == NULL) {
        return;
    }
    livro_exibir(&raiz->livro);
    bst_percorrer_pre_ordem(raiz->esq);
    bst_percorrer_pre_ordem(raiz->dir);
}

void bst_percorrer_pos_ordem(NoBST *raiz) {
    if (raiz == NULL) {
        return;
    }
    bst_percorrer_pos_ordem(raiz->esq);
    bst_percorrer_pos_ordem(raiz->dir);
    livro_exibir(&raiz->livro);
}

int bst_contar_nos(NoBST *raiz) {
    if (raiz == NULL) {
        return 0;
    }
    return 1 + bst_contar_nos(raiz->esq) + bst_contar_nos(raiz->dir);
}

int bst_altura(NoBST *raiz) {
    int alt_esq;
    int alt_dir;

    if (raiz == NULL) {
        return -1;
    }

    alt_esq = bst_altura(raiz->esq);
    alt_dir = bst_altura(raiz->dir);

    return 1 + (alt_esq > alt_dir ? alt_esq : alt_dir);
}

void bst_exportar_vetor(NoBST *raiz, Livro *vetor, int *indice) {
    if (raiz == NULL || vetor == NULL || indice == NULL) {
        return;
    }

    bst_exportar_vetor(raiz->esq, vetor, indice);
    vetor[*indice] = raiz->livro; /* Cópia independente por valor */
    (*indice)++;
    bst_exportar_vetor(raiz->dir, vetor, indice);
}

void livro_exibir(const Livro *livro) {
    if (livro == NULL) {
        return;
    }
    printf("  ID: %-4d | Titulo: %-30s | Autor: %-22s | Ano: %4d\n",
           livro->id, livro->titulo, livro->autor, livro->ano_publicacao);
}

void bst_liberar(NoBST *raiz) {
    if (raiz == NULL) {
        return;
    }
    bst_liberar(raiz->esq);
    bst_liberar(raiz->dir);
    free(raiz);
}

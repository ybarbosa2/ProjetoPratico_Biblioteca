/**
 * @file grafo.c
 * @brief Implementação completa do Grafo não direcionado, BFS e Integração c/ BST (Requisito R08).
 */

#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "fila.h"
#include "lista.h"

const char* grafo_status_mensagem(StatusGrafo status) {
    switch (status) {
        case GRAFO_SUCESSO:
            return "Operacao no grafo realizada com sucesso.";
        case GRAFO_ERRO_ID_INVALIDO:
            return "ID invalido: deve ser um numero inteiro positivo (> 0).";
        case GRAFO_ERRO_VERTICE_DUPLICADO:
            return "Vertice duplicado: ja existe um vertice com este ID no grafo.";
        case GRAFO_ERRO_VERTICE_NAO_ENCONTRADO:
            return "Vertice nao encontrado: o ID informado nao existe no grafo.";
        case GRAFO_ERRO_CAPACIDADE_MAXIMA:
            return "Capacidade maxima do grafo atingida (limite de 100 vertices simultaneos).";
        case GRAFO_ERRO_LACO:
            return "Operacao invalida: lacos (conectar um livro a si mesmo) nao sao permitidos.";
        case GRAFO_ERRO_ARESTA_DUPLICADA:
            return "Conexao duplicada: a relacao entre estes dois livros ja existe.";
        case GRAFO_ERRO_ARESTA_NAO_ENCONTRADA:
            return "Conexao nao encontrada: nao existe relacao direta entre os dois livros informados.";
        default:
            return "Erro desconhecido no grafo.";
    }
}

void grafo_inicializar(Grafo *grafo) {
    int i, j;
    if (grafo == NULL) {
        return;
    }

    for (i = 0; i < MAX_VERTICES; i++) {
        grafo->ids_vertices[i] = 0;
        grafo->ativo[i] = false;
        for (j = 0; j < MAX_VERTICES; j++) {
            grafo->adj[i][j] = 0;
        }
    }
    grafo->num_vertices = 0;
    grafo->num_arestas = 0;
}

int grafo_obter_indice(const Grafo *grafo, int id_livro) {
    int i;
    if (grafo == NULL || id_livro <= 0) {
        return -1;
    }

    for (i = 0; i < MAX_VERTICES; i++) {
        if (grafo->ativo[i] && grafo->ids_vertices[i] == id_livro) {
            return i;
        }
    }
    return -1;
}

StatusGrafo grafo_adicionar_vertice(Grafo *grafo, int id_livro) {
    int slot = -1;
    int i, j;

    if (grafo == NULL || id_livro <= 0) {
        return GRAFO_ERRO_ID_INVALIDO;
    }

    if (grafo_obter_indice(grafo, id_livro) != -1) {
        return GRAFO_ERRO_VERTICE_DUPLICADO;
    }

    if (grafo->num_vertices >= MAX_VERTICES) {
        return GRAFO_ERRO_CAPACIDADE_MAXIMA;
    }

    /* Procura o primeiro slot disponível / liberado */
    for (i = 0; i < MAX_VERTICES; i++) {
        if (!grafo->ativo[i]) {
            slot = i;
            break;
        }
    }

    if (slot == -1) {
        return GRAFO_ERRO_CAPACIDADE_MAXIMA;
    }

    grafo->ids_vertices[slot] = id_livro;
    grafo->ativo[slot] = true;

    /* Garante que o novo slot inicia sem conexões residuais */
    for (j = 0; j < MAX_VERTICES; j++) {
        grafo->adj[slot][j] = 0;
        grafo->adj[j][slot] = 0;
    }

    grafo->num_vertices++;
    return GRAFO_SUCESSO;
}

StatusGrafo grafo_remover_vertice(Grafo *grafo, int id_livro) {
    int idx;
    int j;

    if (grafo == NULL || id_livro <= 0) {
        return GRAFO_ERRO_ID_INVALIDO;
    }

    idx = grafo_obter_indice(grafo, id_livro);
    if (idx == -1) {
        return GRAFO_ERRO_VERTICE_NAO_ENCONTRADO;
    }

    /* Remove todas as arestas incidentes ao vértice */
    for (j = 0; j < MAX_VERTICES; j++) {
        if (grafo->ativo[j] && grafo->adj[idx][j] == 1) {
            grafo->adj[idx][j] = 0;
            grafo->adj[j][idx] = 0;
            grafo->num_arestas--;
        }
    }

    /* Liberta o slot para permitir novos cadastros até a capacidade de 100 */
    grafo->ativo[idx] = false;
    grafo->ids_vertices[idx] = 0;
    grafo->num_vertices--;

    return GRAFO_SUCESSO;
}

StatusGrafo grafo_adicionar_aresta(Grafo *grafo, int id_origem, int id_destino) {
    int u, v;

    if (grafo == NULL || id_origem <= 0 || id_destino <= 0) {
        return GRAFO_ERRO_ID_INVALIDO;
    }

    /* Rejeição de Laço (A, A) */
    if (id_origem == id_destino) {
        return GRAFO_ERRO_LACO;
    }

    u = grafo_obter_indice(grafo, id_origem);
    v = grafo_obter_indice(grafo, id_destino);

    /* Rejeição de conexão com ID inexistente no grafo */
    if (u == -1 || v == -1) {
        return GRAFO_ERRO_VERTICE_NAO_ENCONTRADO;
    }

    /* Rejeição de aresta duplicada (A,B) ou (B,A) */
    if (grafo->adj[u][v] == 1) {
        return GRAFO_ERRO_ARESTA_DUPLICADA;
    }

    /* Grafo não direcionado: (u,v) e (v,u) */
    grafo->adj[u][v] = 1;
    grafo->adj[v][u] = 1;
    grafo->num_arestas++;

    return GRAFO_SUCESSO;
}

StatusGrafo grafo_remover_aresta(Grafo *grafo, int id_origem, int id_destino) {
    int u, v;

    if (grafo == NULL || id_origem <= 0 || id_destino <= 0) {
        return GRAFO_ERRO_ID_INVALIDO;
    }

    if (id_origem == id_destino) {
        return GRAFO_ERRO_LACO;
    }

    u = grafo_obter_indice(grafo, id_origem);
    v = grafo_obter_indice(grafo, id_destino);

    if (u == -1 || v == -1) {
        return GRAFO_ERRO_VERTICE_NAO_ENCONTRADO;
    }

    /* Rejeição de remoção de aresta inexistente */
    if (grafo->adj[u][v] == 0) {
        return GRAFO_ERRO_ARESTA_NAO_ENCONTRADA;
    }

    grafo->adj[u][v] = 0;
    grafo->adj[v][u] = 0;
    grafo->num_arestas--;

    return GRAFO_SUCESSO;
}

bool grafo_existe_aresta(const Grafo *grafo, int id_origem, int id_destino) {
    int u, v;

    if (grafo == NULL || id_origem <= 0 || id_destino <= 0 || id_origem == id_destino) {
        return false;
    }

    u = grafo_obter_indice(grafo, id_origem);
    v = grafo_obter_indice(grafo, id_destino);

    if (u == -1 || v == -1) {
        return false;
    }

    return (grafo->adj[u][v] == 1);
}

bool grafo_obter_vizinhos(const Grafo *grafo, int id_livro, Lista *vizinhos) {
    int u;
    int candidatos[MAX_VERTICES];
    int num_candidatos = 0;
    int j, a, b;

    if (grafo == NULL || vizinhos == NULL || id_livro <= 0) {
        return false;
    }

    u = grafo_obter_indice(grafo, id_livro);
    if (u == -1) {
        return false;
    }

    for (j = 0; j < MAX_VERTICES; j++) {
        if (grafo->ativo[j] && grafo->adj[u][j] == 1) {
            candidatos[num_candidatos++] = grafo->ids_vertices[j];
        }
    }

    /* Ordena os vizinhos em ordem crescente de ID */
    for (a = 0; a < num_candidatos - 1; a++) {
        for (b = 0; b < num_candidatos - 1 - a; b++) {
            if (candidatos[b] > candidatos[b + 1]) {
                int temp = candidatos[b];
                candidatos[b] = candidatos[b + 1];
                candidatos[b + 1] = temp;
            }
        }
    }

    for (j = 0; j < num_candidatos; j++) {
        lista_inserir_fim(vizinhos, candidatos[j], 1);
    }

    return true;
}

StatusGrafo grafo_bfs(const Grafo *grafo, int id_origem, Lista *visitados_ordem) {
    int idx_origem;
    Fila fila;
    bool visitado[MAX_VERTICES];
    int distancias[MAX_VERTICES];
    int id_atual;
    int i;

    if (grafo == NULL || visitados_ordem == NULL || id_origem <= 0) {
        return GRAFO_ERRO_ID_INVALIDO;
    }

    idx_origem = grafo_obter_indice(grafo, id_origem);
    if (idx_origem == -1) {
        return GRAFO_ERRO_VERTICE_NAO_ENCONTRADO;
    }

    for (i = 0; i < MAX_VERTICES; i++) {
        visitado[i] = false;
        distancias[i] = 0;
    }

    fila_inicializar(&fila);

    /* REGRA CRÍTICA: Marcar cada vértice como visitado no momento em que ele ENTRA na fila */
    visitado[idx_origem] = true;
    distancias[idx_origem] = 0;
    fila_enfileirar(&fila, id_origem);

    while (!fila_vazia(&fila)) {
        int u;
        int vizinhos_candidatos[MAX_VERTICES];
        int num_candidatos = 0;
        int a, b;

        fila_desenfileirar(&fila, &id_atual);
        u = grafo_obter_indice(grafo, id_atual);

        /* Insere o nó na lista na ordem exata de descoberta */
        lista_inserir_fim(visitados_ordem, id_atual, distancias[u]);

        /* Coleta vizinhos não visitados */
        for (i = 0; i < MAX_VERTICES; i++) {
            if (grafo->ativo[i] && grafo->adj[u][i] == 1 && !visitado[i]) {
                vizinhos_candidatos[num_candidatos++] = grafo->ids_vertices[i];
            }
        }

        /* REGRA CRÍTICA: Ordena vizinhos estritamente em ORDEM CRESCENTE DE ID */
        for (a = 0; a < num_candidatos - 1; a++) {
            for (b = 0; b < num_candidatos - 1 - a; b++) {
                if (vizinhos_candidatos[b] > vizinhos_candidatos[b + 1]) {
                    int temp = vizinhos_candidatos[b];
                    vizinhos_candidatos[b] = vizinhos_candidatos[b + 1];
                    vizinhos_candidatos[b + 1] = temp;
                }
            }
        }

        /* Enfileira e marca como visitado imediatamente no enqueue */
        for (a = 0; a < num_candidatos; a++) {
            int id_viz = vizinhos_candidatos[a];
            int idx_viz = grafo_obter_indice(grafo, id_viz);

            visitado[idx_viz] = true; /* Marcar ao entrar na fila */
            distancias[idx_viz] = distancias[u] + 1;
            fila_enfileirar(&fila, id_viz);
        }
    }

    fila_liberar(&fila);
    return GRAFO_SUCESSO;
}

StatusGrafo grafo_existe_caminho(const Grafo *grafo, int id_origem, int id_destino, bool *existe) {
    int idx_origem, idx_destino;
    Fila fila;
    bool visitado[MAX_VERTICES];
    int id_atual;
    int i;
    bool caminho_encontrado = false;

    if (grafo == NULL || existe == NULL || id_origem <= 0 || id_destino <= 0) {
        return GRAFO_ERRO_ID_INVALIDO;
    }

    idx_origem = grafo_obter_indice(grafo, id_origem);
    idx_destino = grafo_obter_indice(grafo, id_destino);

    /* Se qualquer um dos IDs não existir no grafo, rejeitar com código de ID ausente */
    if (idx_origem == -1 || idx_destino == -1) {
        return GRAFO_ERRO_VERTICE_NAO_ENCONTRADO;
    }

    if (id_origem == id_destino) {
        *existe = true;
        return GRAFO_SUCESSO;
    }

    for (i = 0; i < MAX_VERTICES; i++) {
        visitado[i] = false;
    }

    fila_inicializar(&fila);
    visitado[idx_origem] = true;
    fila_enfileirar(&fila, id_origem);

    while (!fila_vazia(&fila)) {
        int u;
        fila_desenfileirar(&fila, &id_atual);
        u = grafo_obter_indice(grafo, id_atual);

        if (id_atual == id_destino) {
            caminho_encontrado = true;
            break;
        }

        for (i = 0; i < MAX_VERTICES; i++) {
            if (grafo->ativo[i] && grafo->adj[u][i] == 1 && !visitado[i]) {
                if (i == idx_destino) {
                    caminho_encontrado = true;
                    break;
                }
                visitado[i] = true;
                fila_enfileirar(&fila, grafo->ids_vertices[i]);
            }
        }

        if (caminho_encontrado) {
            break;
        }
    }

    fila_liberar(&fila);
    *existe = caminho_encontrado;
    return GRAFO_SUCESSO;
}

StatusGrafo sistema_cadastrar_livro(NoBST **catalogo, Grafo *grafo, Livro novo_livro) {
    StatusBST st_bst;
    StatusGrafo st_grafo;

    if (catalogo == NULL || grafo == NULL) {
        return GRAFO_ERRO_ID_INVALIDO;
    }

    /* Verifica capacidade do grafo antes de alterar o catálogo */
    if (grafo->num_vertices >= MAX_VERTICES) {
        return GRAFO_ERRO_CAPACIDADE_MAXIMA;
    }

    /* 1. Insere o livro na BST */
    st_bst = bst_inserir(catalogo, novo_livro);
    if (st_bst != BST_SUCESSO) {
        if (st_bst == BST_ERRO_ID_INVALIDO) {
            return GRAFO_ERRO_ID_INVALIDO;
        }
        if (st_bst == BST_ERRO_DUPLICADO) {
            return GRAFO_ERRO_VERTICE_DUPLICADO;
        }
        return GRAFO_ERRO_ID_INVALIDO;
    }

    /* 2. Cria o vértice de mesmo ID no grafo */
    st_grafo = grafo_adicionar_vertice(grafo, novo_livro.id);
    if (st_grafo != GRAFO_SUCESSO) {
        /* ROLLBACK: desfaz imediatamente a inserção na BST mantendo a consistência */
        bst_remover(catalogo, novo_livro.id, NULL);
        return st_grafo;
    }

    return GRAFO_SUCESSO;
}

StatusGrafo sistema_remover_livro(NoBST **catalogo, Grafo *grafo, int id, Livro *livro_removido) {
    StatusBST st_bst;
    StatusGrafo st_grafo;

    if (catalogo == NULL || grafo == NULL || id <= 0) {
        return GRAFO_ERRO_ID_INVALIDO;
    }

    /* Rejeita se o ID não existir no grafo ou na BST sem alterar nada */
    if (grafo_obter_indice(grafo, id) == -1 || bst_buscar(*catalogo, id) == NULL) {
        return GRAFO_ERRO_VERTICE_NAO_ENCONTRADO;
    }

    /* 1. Remove da BST */
    st_bst = bst_remover(catalogo, id, livro_removido);
    if (st_bst != BST_SUCESSO) {
        return GRAFO_ERRO_VERTICE_NAO_ENCONTRADO;
    }

    /* 2. Remove o vértice e todas as suas arestas do Grafo */
    st_grafo = grafo_remover_vertice(grafo, id);
    return st_grafo;
}

void grafo_exibir(const Grafo *grafo, NoBST *raiz_bst) {
    int i, j, a, b;

    if (grafo == NULL || grafo->num_vertices == 0) {
        printf("\n[Grafo] O grafo de relacoes esta vazio (nenhum vertice cadastrado).\n");
        return;
    }

    printf("\n--- Grafo de Relacoes (%d vertices, %d relacoes) ---\n",
           grafo->num_vertices, grafo->num_arestas);

    for (i = 0; i < MAX_VERTICES; i++) {
        int id_origem;
        NoBST *no;
        int vizinhos[MAX_VERTICES];
        int num_v = 0;

        if (!grafo->ativo[i]) {
            continue;
        }

        id_origem = grafo->ids_vertices[i];
        no = (raiz_bst != NULL) ? bst_buscar(raiz_bst, id_origem) : NULL;

        if (no != NULL) {
            printf("  Livro [%d: \"%s\"] -> ", id_origem, no->livro.titulo);
        } else {
            printf("  Livro [%d] -> ", id_origem);
        }

        for (j = 0; j < MAX_VERTICES; j++) {
            if (grafo->ativo[j] && grafo->adj[i][j] == 1) {
                vizinhos[num_v++] = grafo->ids_vertices[j];
            }
        }

        /* Ordena vizinhos em ordem crescente de ID para exibição uniforme */
        for (a = 0; a < num_v - 1; a++) {
            for (b = 0; b < num_v - 1 - a; b++) {
                if (vizinhos[b] > vizinhos[b + 1]) {
                    int t = vizinhos[b];
                    vizinhos[b] = vizinhos[b + 1];
                    vizinhos[b + 1] = t;
                }
            }
        }

        if (num_v == 0) {
            printf("(Sem conexoes / Isolado)\n");
        } else {
            for (a = 0; a < num_v; a++) {
                printf("%d%s", vizinhos[a], (a < num_v - 1) ? ", " : "\n");
            }
        }
    }
    printf("----------------------------------------------------\n");
}

void grafo_liberar(Grafo *grafo) {
    grafo_inicializar(grafo);
}

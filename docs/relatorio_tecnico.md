# RELATÓRIO TÉCNICO DE PROJETO PRÁTICO

## TEMA C — SISTEMA DE GESTÃO DE BIBLIOTECA
### Disciplina: Estrutura de Dados
**Docente:** Prof. Me. Heber Castro  
**Discente:** Yuri  
**Ano Letivo / Semestre:** 2026 / 2  

---

## SUMÁRIO

1. [Introdução e Objetivo do Sistema](#1-introdução-e-objetivo-do-sistema)
2. [Modelagem dos Dados, Structs e Campos](#2-modelagem-dos-dados-structs-e-campos)
3. [Justificativa Técnica da Escolha das Estruturas de Dados](#3-justificativa-técnica-da-escolha-das-estruturas-de-dados)
4. [Árvore Binária de Busca (BST) e Análise de Casos de Remoção](#4-árvore-binária-de-busca-bst-e-análise-de-casos-de-remoção)
5. [Grafo, Mapeamento Compacto, Rollback e Busca em Largura (BFS)](#5-grafo-mapeamento-compacto-rollback-e-busca-em-largura-bfs)
6. [Estruturas Auxiliares, Histórico e Algoritmo Próprio de Ordenação](#6-estruturas-auxiliares-histórico-e-algoritmo-próprio-de-ordenação)
7. [Tabelas de Análise de Complexidade Temporal e Espacial (Big-O)](#7-tabelas-de-análise-de-complexidade-temporal-e-espacial-big-o)
8. [Relatório Completo dos Testes Reproduzíveis](#8-relatório-completo-dos-testes-reproduzíveis)
9. [Conclusão e Limitações Conhecidas](#9-conclusão-e-limitações-conhecidas)
10. [Referências Bibliográficas](#10-referências-bibliográficas)
11. [Declaração Transparente do Uso de Ferramentas de Inteligência Artificial](#11-declaração-transparente-do-uso-de-ferramentas-de-inteligência-artificial)

---

## 1. INTRODUÇÃO E OBJETIVO DO SISTEMA

O gerenciamento de acervos bibliográficos modernos exige o processamento coordenado de múltiplos relacionamentos entre registros textuais e numéricos. No domínio de uma biblioteca (Tema C), uma aplicação de catálogo eficiente precisa atender simultaneamente a três requisitos fundamentais:
1. **Recuperação rápida e determinística** de obras por chave primária de identificação única (código de tombamento / ID);
2. **Mapeamento explícito de relações temáticas** ou de co-leitura entre livros (recomendações de obras correlatas, trilhas de estudo e análise de proximidade em rede);
3. **Auditabilidade estrita** de alterações de estado do acervo com garantia de consistência relacional e controle de concorrência/capacidade.

O objetivo deste projeto prático individual é projetar, implementar e validar formalmente um sistema completo de gestão de biblioteca em linguagem C (padrão C99 estrito), utilizando estruturas de dados fundamentais sem o emprego de facilitadores de alto nível ou bibliotecas de terceiros.

O sistema integra:
- Uma **Árvore Binária de Busca (BST)** encarregada da indexação primária dos livros ordenados por ID;
- Um **Grafo Não Direcionado e Não Ponderado** com matriz de adjacência e tabela de dispersão/mapeamento de slots para modelar afinidades entre livros por seus identificadores;
- Um mecanismo de **Busca em Largura (BFS)** apoiado por uma **Fila Dinâmica FIFO** e uma **Lista Simplesmente Encadeada** com inserção no fim em $\mathcal{O}(1)$, garantindo percurso de níveis em ordem estritamente crescente de ID;
- Uma **Pilha Dinâmica Encadeada** para histórico LIFO não volátil de até 5 ações recentes de alteração de estado;
- Algoritmos próprios de **Merge Sort** e **Quick Sort** aplicados sobre vetores auxiliares de cópias independentes dos registros, possibilitando a reordenação do catálogo por título e autor sem afetar a topologia da BST;
- Camada de **Entrada Segura de Dados (I/O)** com prevenção sistemática contra estouro de buffer (*buffer overflow*) e contaminação de fluxo de entrada (`stdin`).

---

## 2. MODELAGEM DOS DADOS, STRUCTS E CAMPOS

A modelagem dos dados atende rigorosamente aos limites estabelecidos no regulamento do Tema C, definindo restrições explícitas de tipos primitivos, faixas numéricas e tamanhos de vetores de caracteres com terminador nulo (`\0`).

### 2.1. O Registro do Livro (`Livro`)

Definido no arquivo [`bst.h`](file:///c:/Users/yuri2/Desktop/ProjetoPratico_Biblioteca/include/bst.h), o registro `Livro` possui a seguinte estrutura:

```c
#define TAM_TEXTO 80
#define ANO_MINIMO 1
#define ANO_MAXIMO 2026

typedef struct {
    int id;                         /* Identificador único e positivo (> 0) */
    char titulo[TAM_TEXTO];         /* Título da obra (até 79 caracteres úteis + '\0') */
    char autor[TAM_TEXTO];          /* Nome do autor (até 79 caracteres úteis + '\0') */
    int ano_publicacao;             /* Ano de publicação (inteiro entre 1 e 2026) */
} Livro;
```

**Regras de Validação Aplicadas:**
- `id`: Deve ser estritamente maior que zero (`id > 0`). Valores nulos ou negativos são rejeitados de imediato com o código `IO_ERRO_ID_INVALIDO` ou `BST_ERRO_ID_INVALIDO`.
- `titulo` e `autor`: Vetores de caracteres estáticos de tamanho 80. São permitidos até 79 caracteres imprimíveis mais o caractere nulo `\0`. Rejeitam-se cadeias vazias (`""`), strings contendo apenas espaços em branco (`isspace()`) ou entradas truncadas por estouro de buffer.
- `ano_publicacao`: Restrito aos limites $1 \le \text{ano} \le 2026$. Qualquer valor externo é rejeitado com o erro `IO_ERRO_ANO_INVALIDO`.

### 2.2. Nó da Árvore Binária de Busca (`NoBST`)

```c
typedef struct NoBST {
    Livro livro;                    /* Registro de dados completo (cópia por valor) */
    struct NoBST *esq;              /* Ponteiro para a subárvore esquerda (chaves menores) */
    struct NoBST *dir;              /* Ponteiro para a subárvore direita (chaves maiores) */
} NoBST;
```

### 2.3. Grafo com Mapeamento de Slots (`Grafo`)

A representação do grafo adota matriz de adjacência estática com capacidade nominal de 100 vértices simultâneos (`MAX_VERTICES = 100`). Como os IDs dos livros são arbitrários (por exemplo, ID 5000 ou 1200), adota-se um vetor bidirecional de controle de slots ativos:

```c
#define MAX_VERTICES 100

typedef struct {
    int ids_vertices[MAX_VERTICES];         /* Mapeia slot [0..99] -> ID arbitrário do livro */
    bool ativo[MAX_VERTICES];               /* Booleano indicando ocupação ativa do slot */
    int adj[MAX_VERTICES][MAX_VERTICES];    /* Matriz de adjacência (0 = sem aresta, 1 = conectado) */
    int num_vertices;                       /* Quantidade de vértices atualmente ativos */
    int num_arestas;                        /* Quantidade total de arestas não direcionadas */
} Grafo;
```

### 2.4. Estruturas Auxiliares Encadeadas (`Lista`, `Fila`, `Pilha`)

- **Nó e Descritor da Lista Encadeada (`lista.h`):**
  ```c
  typedef struct NoLista {
      int id_livro;
      int distancia;                  /* Grau de separação/camada calculada na BFS */
      struct NoLista *prox;
  } NoLista;

  typedef struct {
      NoLista *inicio;
      NoLista *fim;                   /* Ponteiro para o fim permitindo inserção O(1) */
      int tamanho;
  } Lista;
  ```
- **Nó e Descritor da Fila Dinâmica (`fila.h`):**
  ```c
  typedef struct NoFila {
      int id_livro;
      struct NoFila *prox;
  } NoFila;

  typedef struct {
      NoFila *frente;                 /* Ponto de desenfileiramento (dequeue) */
      NoFila *tras;                   /* Ponto de enfileiramento (enqueue) */
      int tamanho;
  } Fila;
  ```
- **Registro de Histórico e Pilha Dinâmica (`pilha.h`):**
  ```c
  typedef enum {
      ACAO_INSERIR_LIVRO = 1,
      ACAO_REMOVER_LIVRO = 2,
      ACAO_ADICIONAR_RELACAO = 3,
      ACAO_REMOVER_RELACAO = 4
  } TipoAcao;

  typedef struct {
      TipoAcao tipo;
      int id_principal;
      int id_secundario;
      Livro snapshot_livro;
      char descricao[120];
  } RegistoHistorico;

  typedef struct NoPilha {
      RegistoHistorico registo;
      struct NoPilha *prox;
  } NoPilha;

  typedef struct {
      NoPilha *topo;
      int tamanho;
  } Pilha;
  ```

---

## 3. JUSTIFICATIVA TÉCNICA DA ESCOLHA DAS ESTRUTURAS DE DADOS

A escolha arquitetural de cada estrutura de dados responde diretamente aos compromissos de eficiência assintótica e semântica operacional do problema:

1. **Árvore Binária de Busca (BST) para Catálogo Primário:**
   A busca por chave em tabelas sequenciais não ordenadas impõe custo temporal $\mathcal{O}(n)$, inviável para acervos crescentes. O uso de uma BST oferece busca, inserção e remoção em tempo médio $\mathcal{O}(\log n)$ com custo espacial linear $\mathcal{O}(n)$, além de permitir o percurso ordenado natural por ID através de travessia em-ordem (*in-order traversal*) sem alocação adicional.
2. **Grafo com Matriz de Adjacência para Relações Temáticas:**
   O edital estipula uma capacidade de 100 nós simultâneos. Para grafos densos ou de porte delimitado ($V \le 100$), a matriz de adjacência oferece verificação de adjacência direta `(u, v)` em tempo constante estrito $\mathcal{O}(1)$, sem a sobrecarga de percurso de listas encadeadas de adjacência. O consumo espacial de $100 \times 100 \times 4 \text{ bytes} \approx 40 \text{ KB}$ é insignificante para as arquiteturas computacionais contemporâneas.
3. **Pilha Dinâmica Encadeada para Histórico de Ações (LIFO):**
   A semântica de auditoria e operações de desfazimento/inspeção de eventos recentes requer acesso prioritário às últimas mutações efetuadas no estado do sistema. Uma pilha encadeada aloca memória estritamente sob demanda e permite operações de *push* e *pop* em tempo $\mathcal{O}(1)$, eliminando restrições artificiais de tamanho estático.
4. **Fila Dinâmica FIFO para Busca em Largura (BFS):**
   O algoritmo de exploração em largura processa vértices em ordem estrita de distância radial da origem. Uma fila encadeada com ponteiros de início (`frente`) e fim (`tras`) garante enfileiramento e desenfileiramento em $\mathcal{O}(1)$.
5. **Lista Encadeada com Ponteiro de Fim para Armazenamento da BFS:**
   A preservação da sequência exata de descoberta dos nós na BFS demanda uma coleção ordenada sequencialmente. Ao manter o ponteiro `fim` na struct `Lista`, cada nova inserção é realizada em tempo ótimo $\mathcal{O}(1)$.
6. **Merge Sort Próprio para Ordenação sobre Vetor Auxiliar:**
   Diferente de algoritmos quadráticos ($\mathcal{O}(n^2)$) ou instáveis, o Merge Sort implementado manualmente garante complexidade de pior caso estritamente delimitada em $\mathcal{O}(n \log n)$, assegurando estabilidade na ordenação por título e desempate determinístico por ID.

---

## 4. ÁRVORE BINÁRIA DE BUSCA (BST) E ANÁLISE DE CASOS DE REMOÇÃO

A BST mantém o invariante fundamental: para qualquer nó $X$, todas as chaves da subárvore esquerda são estritamente menores que a chave de $X$, e todas as chaves da subárvore direita são estritamente maiores que a chave de $X$.

### 4.1. Operação de Remoção e os Três Casos Clássicos

A remoção de um nó em uma BST é uma das operações mais críticas da estrutura, exigindo a preservação invariante da árvore após a eliminação da chave. O algoritmo implementado em [`bst_remover`](file:///c:/Users/yuri2/Desktop/ProjetoPratico_Biblioteca/src/bst.c#L143-L203) trata exaustivamente os três casos:

```text
    Caso 1: Nó Folha           Caso 2: Um Filho           Caso 3: Dois Filhos
        (P)                        (P)                            (P)
         |                          |                              |
        [X]                        [X]                            [X]
       /   \                      /   \                          /   \
     NULL  NULL                 (F)   NULL                     (E)   (D)
                                                                      |
                                                                   [Sucessor]
```

1. **Caso 1: Nó Folha (0 Filhos):**
   O nó a ser removido não possui filhos (`esq == NULL && dir == NULL`). A memória do nó é liberada imediatamente via `free()` e o ponteiro que apontava para ele no nó pai é atualizado para `NULL`.
2. **Caso 2: Nó com Apenas Um Filho:**
   O nó a ser removido possui apenas o filho esquerdo ou apenas o filho direito. O ponteiro do pai é redirecionado diretamente para esse único filho, e o nó intermediário é liberado via `free()`.
3. **Caso 3: Nó com Dois Filhos:**
   O nó possui ambas as subárvores não nulas. O nó não pode ser simplesmente desconectado sob pena de fragmentar a árvore. O procedimento adotado consiste em:
   - Localizar o **sucessor em-ordem** do nó (ou seja, o nó com a menor chave presente na subárvore direita, obtido por descida contínua à esquerda via `bst_minimo(atual->dir)`);
   - Salvar os dados do sucessor em uma variável local temporária;
   - Chamar recursivamente `bst_remover(&atual->dir, sucessor->livro.id, NULL)` para remover o nó sucessor da subárvore direita (que se enquadrará obrigatoriamente no Caso 1 ou no Caso 2, pois um nó mínimo à esquerda nunca possui filho esquerdo);
   - Copiar os dados salvos do sucessor para o nó original.

Essa abordagem preserva as referências de ponteiros da árvore e suporta com segurança a remoção da própria raiz com dois filhos.

### 4.2. Análise da Dependência da Altura da Árvore e Degeneração

O desempenho de todas as operações fundamentais da BST (busca, inserção e remoção) é proporcional à sua altura $h$:

$$T(n) = \mathcal{O}(h)$$

- **Árvore Balanceada (Melhor Caso e Caso Médio):**
  Quando as inserções de chaves ocorrem em distribuição aleatória ou quase uniforme, a árvore mantém uma altura balanceada próxima a:
  $$h \approx \lfloor \log_2 n \rfloor \implies \text{Complexidade } \mathcal{O}(\log n)$$
- **Árvore Degenerada (Pior Caso):**
  Se as chaves forem inseridas em ordem estritamente crescente (ex: IDs 10, 20, 30, 40...) ou estritamente decrescente, a árvore degenera em uma lista simplesmente encadeada orientada à direita ou à esquerda. Nesse cenário:
  $$h = n - 1 \implies \text{Complexidade } \mathcal{O}(n)$$

No presente projeto, o catálogo adota verificação de integridade e medição de altura em tempo real através da função [`bst_altura()`](file:///c:/Users/yuri2/Desktop/ProjetoPratico_Biblioteca/src/bst.c#L239-L252), permitindo a auditoria contínua da eficiência estrutural da árvore.

---

## 5. GRAFO, MAPEAMENTO COMPACTO, ROLLBACK E BUSCA EM LARGURA (BFS)

### 5.1. Mapeamento de ID Arbitrário para Índice Compacto

Para viabilizar o uso de matriz de adjacência de tamanho fixo sem impor que os IDs dos livros sejam restritos a índices de 0 a 99, implementou-se uma tabela de mapeamento bidirecional:
- `ids_vertices[i]`: Armazena o ID arbitrário associado ao slot $i \in [0, 99]$.
- `ativo[i]`: Flag booleana que indica se o slot $i$ está ocupado por um vértice vivo.

Ao remover um vértice pelo ID, a função [`grafo_remover_vertice()`](file:///c:/Users/yuri2/Desktop/ProjetoPratico_Biblioteca/src/grafo.c#L105-L135) zera todas as arestas incidentes em sua linha e coluna na matriz, marca `ativo[i] = false` e decrementa o contador `num_vertices`. Dessa forma, o slot liberado é imediatamente reciclado na próxima chamada de inserção, garantindo a sustentação de até 100 vértices ativos ao longo de todo o ciclo de vida da aplicação.

### 5.2. Estratégia de Transações e Rollback (Consistência BST + Grafo)

A Seção 4 do edital exige consistência atômica entre o catálogo de livros e o grafo de relacionamentos. No fluxo integrado de cadastro:

```text
[Início: Cadastrar Livro]
          |
  Verifica capacidade do Grafo (num_vertices < 100?)
          |-- NÃO --> Retorna GRAFO_ERRO_CAPACIDADE_MAXIMA
         SIM
          v
  Insere Livro na BST (bst_inserir)
          |-- FALHA --> Retorna Erro da BST
       SUCESSO
          v
  Tenta Adicionar Vértice no Grafo (grafo_adicionar_vertice)
          |
          +-- SUCESSO --> [Transação Concluída com Sucesso]
          |
         FALHA (Ex: Grafo Cheio)
          v
  ROLLBACK IMEDIATO: bst_remover(catalogo, livro.id)
          v
  Retorna Código de Erro (BST e Grafo permanecem idênticos)
```

Essa lógica transacional foi consolidada na função [`sistema_cadastrar_livro()`](file:///c:/Users/yuri2/Desktop/ProjetoPratico_Biblioteca/src/grafo.c#L382-L417). Da mesma forma, [`sistema_remover_livro()`](file:///c:/Users/yuri2/Desktop/ProjetoPratico_Biblioteca/src/grafo.c#L419-L442) assegura que a exclusão de um livro remova simultaneamente o nó da BST e o vértice correspondente no Grafo junto com todas as suas conexões incidentes.

### 5.3. Busca em Largura (BFS) com Ordem Crescente de ID

A Busca em Largura explora o grafo camada por camada (níveis radiais de distância a partir de um vértice de partida). O edital estabelece duas regras críticas de implementação:
1. **Marcação Antecipada de Visita:** Cada vértice é marcado como visitado no momento exato em que é colocado na fila (`enqueue`), impedindo que múltiplos nós de um mesmo nível enfileirem o mesmo vizinho concorrentemente.
2. **Ordenação Crescente dos Vizinhos:** Independentemente da ordem física em que os slots foram alocados na matriz, os vizinhos adjacentes a cada nó são coletados, ordenados em ordem crescente de ID e enfileirados nessa sequência.

O algoritmo em [`grafo_bfs()`](file:///c:/Users/yuri2/Desktop/ProjetoPratico_Biblioteca/src/grafo.c#L261-L327) procede da seguinte forma:
- Valida a existência do vértice inicial e aloca o vetor de distâncias (`distancias[u] = 0`);
- Marca a origem como visitada e enfileira na Fila dinâmica própria;
- Em cada iteração, desenfileira o nó atual $u$ e o insere no final da Lista encadeada;
- Coleta todos os vértices vizinhos $v$ tais que `adj[u][v] == 1` e `!visitado[v]`;
- Ordena o vetor de candidatos a vizinhos por ID crescente;
- Para cada vizinho ordenado: marca `visitado[v] = true`, atualiza `distancias[v] = distancias[u] + 1` e enfileira na Fila.

**Custo Computacional da BFS na Matriz de Adjacência:**
Para cada um dos $V$ vértices visitados, o algoritmo varre uma linha completa de tamanho $V$ da matriz para detectar adjacências. Portanto, a complexidade temporal da BFS com matriz de adjacência é estritamente quadrática no número de vértices:
$$\mathcal{O}(V^2)$$
Embora em listas de adjacência a BFS custe $\mathcal{O}(V + E)$, para a escala exigida ($V = 100$) o custo de $\approx 10.000$ operações elementares é executado em microssegundos, com a vantagem da simplicidade de implementação e ausência de ponteiros fragmentados.

---

## 6. ESTRUTURAS AUXILIARES, HISTÓRICO E ALGORITMO PRÓPRIO DE ORDENAÇÃO

### 6.1. Regra do Histórico na Pilha Dinâmica (Seção 8)

A Pilha dinâmica encadeada implementada em [`pilha.c`](file:///c:/Users/yuri2/Desktop/ProjetoPratico_Biblioteca/src/pilha.c) atua como registrador transacional auditável. Conforme a regra de negócio do projeto:
- **Operações que ENTRAM na Pilha:** Exclusivamente alterações de estado concluídas com sucesso:
  - Cadastro de livro (opção 1);
  - Remoção de livro (opção 3);
  - Adição de conexão entre livros (opção 6);
  - Remoção de conexão entre livros (opção 7).
- **Operações que NÃO ENTRAM na Pilha:** Operações de consulta ou leituras estéreis (busca por ID, listagem em-ordem, ordenação por título, verificação de conexão, BFS, existência de caminho e exibição do histórico).
- **Exibição Não Destrutiva:** A função [`pilha_exibir_historico()`](file:///c:/Users/yuri2/Desktop/ProjetoPratico_Biblioteca/src/pilha.c#L101-L133) percorre os nós a partir do topo até o limite de 5 ações mais recentes, imprimindo o tipo de ação, IDs envolvidos e descrição, mantendo a integridade de todos os nós empilhados.

### 6.2. Algoritmo de Ordenação Próprio sobre Vetor Auxiliar (Merge Sort e Quick Sort)

O edital veda terminantemente a utilização da função `qsort()` da biblioteca padrão (`stdlib.h`). No módulo [`ordenacao.c`](file:///c:/Users/yuri2/Desktop/ProjetoPratico_Biblioteca/src/ordenacao.c), foram implementados de forma manual tanto o **Merge Sort** quanto o **Quick Sort**.

#### Algoritmo Adotado: Merge Sort Estável
Para a listagem ordenada do catálogo (opção 5), adotou-se o Merge Sort:
1. Uma função precursora conta a quantidade total de nós da BST via `bst_contar_nos(raiz)` e aloca dinamicamente um vetor de registros `Livro` com o tamanho exato;
2. `bst_exportar_vetor(raiz, vetor, &indice)` copia os dados de cada livro por valor para o vetor auxiliar, sem expor ponteiros da árvore;
3. O algoritmo divide o vetor recursivamente até subarrays unitários e intercala as partições com base na função comparadora:
   ```c
   int livro_comparar(const Livro *a, const Livro *b, CriterioOrdenacao criterio);
   ```
4. **Critério de Ordenação e Desempate (Requisito R06):**
   - Compara o campo `titulo` de forma sensível a maiúsculas e minúsculas via `strcmp(a->titulo, b->titulo)`;
   - Se os títulos forem textualmente idênticos (`strcmp == 0`), realiza o desempate pelo campo `id` de forma crescente (`a->id < b->id`).

#### Análise de Complexidade do Merge Sort:
- **Melhor Caso:** $\mathcal{O}(n \log n)$ — A divisão em metades e a intercalação linear ocorrem independentemente da ordem inicial.
- **Caso Médio:** $\mathcal{O}(n \log n)$.
- **Pior Caso:** $\mathcal{O}(n \log n)$ — O Merge Sort não sofre da degeneração quadrática $\mathcal{O}(n^2)$ que pode ocorrer no Quick Sort quando o pivô é mal escolhido.
- **Complexidade Espacial Auxiliar:** $\mathcal{O}(n)$ — Necessária para o vetor de intercalação temporário.

Após a exibição formatada dos registros na tela, o vetor auxiliar é liberado via `free()`, mantendo a BST original intacta e inalterada.

---

## 7. TABELAS DE ANÁLISE DE COMPLEXIDADE TEMPORAL E ESPACIAL (BIG-O)

A tabela a seguir consolida o comportamento assintótico de todas as estruturas de dados e operações implementadas no sistema:

| Módulo / Estrutura | Operação | Melhor Caso (Tempo) | Caso Médio (Tempo) | Pior Caso (Tempo) | Complexidade Espacial |
|---|---|---|---|---|---|
| **BST** | Inserção (`bst_inserir`) | $\mathcal{O}(1)$ (raiz vazia) | $\mathcal{O}(\log n)$ | $\mathcal{O}(n)$ (degenerada) | $\mathcal{O}(1)$ por nó |
| **BST** | Busca por ID (`bst_buscar`) | $\mathcal{O}(1)$ (na raiz) | $\mathcal{O}(\log n)$ | $\mathcal{O}(n)$ (degenerada) | $\mathcal{O}(1)$ auxiliar |
| **BST** | Remoção (`bst_remover`) | $\mathcal{O}(1)$ (folha raiz) | $\mathcal{O}(\log n)$ | $\mathcal{O}(n)$ (degenerada) | $\mathcal{O}(1)$ auxiliar |
| **BST** | Percurso Em-Ordem | $\mathcal{O}(n)$ | $\mathcal{O}(n)$ | $\mathcal{O}(n)$ | $\mathcal{O}(h)$ pilha de chamada |
| **BST** | Menor/Maior ID | $\mathcal{O}(1)$ | $\mathcal{O}(\log n)$ | $\mathcal{O}(n)$ | $\mathcal{O}(1)$ |
| **BST** | Exportação para Vetor | $\mathcal{O}(n)$ | $\mathcal{O}(n)$ | $\mathcal{O}(n)$ | $\mathcal{O}(n)$ vetor cópia |
| **Grafo** | Inserção de Vértice | $\mathcal{O}(1)$ | $\mathcal{O}(V)$ (busca slot) | $\mathcal{O}(V)$ | $\mathcal{O}(V^2)$ matriz estática |
| **Grafo** | Remoção de Vértice | $\mathcal{O}(V)$ | $\mathcal{O}(V)$ (limpa arestas) | $\mathcal{O}(V)$ | $\mathcal{O}(1)$ |
| **Grafo** | Adição de Aresta | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| **Grafo** | Remoção de Aresta | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| **Grafo** | Consulta de Aresta | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| **Grafo** | Busca em Largura (BFS) | $\mathcal{O}(V^2)$ | $\mathcal{O}(V^2)$ | $\mathcal{O}(V^2)$ | $\mathcal{O}(V)$ fila + lista |
| **Grafo** | Existência de Caminho | $\mathcal{O}(1)$ (mesmo nó) | $\mathcal{O}(V^2)$ | $\mathcal{O}(V^2)$ | $\mathcal{O}(V)$ visitados |
| **Pilha** | Empilhar (`push`) | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ por nó |
| **Pilha** | Desempilhar (`pop`) | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| **Pilha** | Consulta Topo (`peek`) | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| **Pilha** | Exibir Histórico (até 5) | $\mathcal{O}(1)$ (constante $\le 5$) | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| **Fila** | Enfileirar (`enqueue`) | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ por nó |
| **Fila** | Desenfileirar (`dequeue`) | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ |
| **Lista** | Inserção no Fim | $\mathcal{O}(1)$ (com ponteiro fim) | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ | $\mathcal{O}(1)$ por nó |
| **Ordenação** | Merge Sort Próprio | $\mathcal{O}(n \log n)$ | $\mathcal{O}(n \log n)$ | $\mathcal{O}(n \log n)$ | $\mathcal{O}(n)$ auxiliar |
| **Ordenação** | Quick Sort Próprio | $\mathcal{O}(n \log n)$ | $\mathcal{O}(n \log n)$ | $\mathcal{O}(n^2)$ (pior pivô) | $\mathcal{O}(\log n)$ recursão |

---

## 8. RELATÓRIO COMPLETO DOS TESTES REPRODUZÍVEIS

Para assegurar a confiabilidade do código em conformidade com a Seção 12 do edital, foram desenvolvidas suítes de testes unitários e de integração automatizadas, executadas com `mingw32-make test`. A tabela abaixo documenta as entradas, operações e os resultados obtidos em cada cenário:

| # | Teste Unitário | Dados de Entrada | Operação Testada | Resultado Esperado | Resultado Obtido | Status |
|---|---|---|---|---|---|---|
| **1** | `testar_bst_r07` | Livros IDs 50, 30, 70, 20, 40, 60, 80 | Inserção válida na BST | Inserção dos 7 nós mantendo invariante BST | 7 nós inseridos com sucesso | **APROVADO** |
| **2** | `testar_bst_r07` | Livro ID 50 | Inserção de ID duplicado | Retorno `BST_ERRO_DUPLICADO` | Rejeição sem alteração na árvore | **APROVADO** |
| **3** | `testar_bst_r07` | Livros IDs 0 e -5 | Inserção de ID inválido | Retorno `BST_ERRO_ID_INVALIDO` | Rejeição imediata | **APROVADO** |
| **4** | `testar_bst_r07` | BST povoada | Consultar Menor e Maior ID | Menor = 20, Maior = 80 | Identificados 20 e 80 | **APROVADO** |
| **5** | `testar_bst_r07` | ID 999 | Remoção de ID inexistente | Retorno `BST_ERRO_NAO_ENCONTRADO` | Árvore permanece inalterada | **APROVADO** |
| **6** | `testar_bst_r07` | ID 20 (folha) | Remoção Caso 1 (0 filhos) | Nó liberado; ponteiro vira NULL | Nó 20 removido; contagem = 6 | **APROVADO** |
| **7** | `testar_bst_r07` | ID 30 (1 filho: 40) | Remoção Caso 2 (1 filho) | Filho 40 promovido diretamente | Nó 30 removido; contagem = 5 | **APROVADO** |
| **8** | `testar_bst_r07` | ID 50 (raiz c/ 2 filhos) | Remoção Caso 3 (2 filhos) | Sucessor 60 assume a nova raiz | Raiz atualizada para 60; contagem = 4 | **APROVADO** |
| **9** | `testar_integracao_bst_ordenacao` | 5 livros com títulos diversos e IDs 10, 15, 23, 42, 88 | Exportação + Merge Sort por título com desempate | Vetor ordenado por título; BST original intacta | Vetor ordenado alfabeticamente; BST inalterada | **APROVADO** |
| **10**| `testar_validacao_entradas_r01` | `""`, `"   "`, `> 79 chars` | Validação de texto (título/autor) | Rejeição de vazios, espaços e excesso | Rejeitado com códigos descritivos de I/O | **APROVADO** |
| **11**| `testar_validacao_entradas_r01` | `"123abc"`, overflow, ano `2027` | Validação de inteiros e limites | Rejeição de caracteres extras e limites | Rejeitado com `IO_ERRO_CARACTERES_EXTRAS` e `ANO_INVALIDO` | **APROVADO** |
| **12**| `testar_grafo_regras_basicas_r08`| IDs 5000, 1200, 350, 80 | Vértices com IDs arbitrários | Slots alocados em posições de 0 a 99 | Vértices alocados e mapeados | **APROVADO** |
| **13**| `testar_grafo_regras_basicas_r08`| Aresta `(5000, 1200)` | Criação de aresta simétrica | Aresta bidirecional ativa na matriz | Conexão confirmada em ambos os sentidos | **APROVADO** |
| **14**| `testar_grafo_regras_basicas_r08`| Aresta `(1200, 5000)` | Aresta duplicada na ordem inversa | Retorno `GRAFO_ERRO_ARESTA_DUPLICADA` | Rejeição sem duplicação de aresta | **APROVADO** |
| **15**| `testar_grafo_regras_basicas_r08`| Aresta `(5000, 5000)` | Tentativa de laço reflexivo | Retorno `GRAFO_ERRO_LACO` | Rejeição sem alteração no grafo | **APROVADO** |
| **16**| `testar_remocao_vertice_e_limpeza`| Vértice 100 com 3 arestas | Remoção de vértice | Limpeza em cascata das 3 arestas | Arestas zeradas; slot liberado | **APROVADO** |
| **17**| `testar_bfs_ordem_crescente_de_id`| Vértices desordenados ligados a 10 | BFS com vizinhos crescentes | Lista ordenada: 10, 20, 30, 40, 50, 60, 70, 80 | Descoberta exatamente em ordem crescente | **APROVADO** |
| **18**| `testar_bfs_ordem_crescente_de_id`| Vértice isolado 9999 | BFS com origem isolada | Lista contendo apenas `[9999]` dist 0 | Lista com 1 elemento exclusivo | **APROVADO** |
| **19**| `testar_existencia_caminho` | Nós 1 e 3 conexos; 1 e 10 desconexos | Verificação de caminho | `true` para conexos; `false` para desconexos | Detecção correta de caminhos | **APROVADO** |
| **20**| `testar_existencia_caminho` | IDs 1 e 999 (ausente) | Verificação com ID inexistente | Retorno `GRAFO_ERRO_VERTICE_NAO_ENCONTRADO` | Erro reportado sem falso silencioso | **APROVADO** |
| **21**| `testar_integracao_e_rollback` | Inserção de 100 livros seguidos do 101º | Teste de Rollback na lotação do grafo | 101º rejeitado; BST revertida com 100 nós | BST e Grafo consistentes com 100 nós | **APROVADO** |
| **22**| `test_integrado.c` | Fluxo completo do sistema | Auditoria da Pilha de Histórico | Apenas sucessos de alteração na pilha | 9 ações registradas; consultas não poluem pilha | **APROVADO** |

---

## 9. CONCLUSÃO E LIMITAÇÕES CONHECIDAS

### 9.1. Conclusão

O desenvolvimento deste projeto prático permitiu vivenciar, de forma concreta e aprofundada, os principais desafios de engenharia de software no gerenciamento de estruturas de dados em baixo nível. Destacam-se as seguintes conquistas arquiteturais:
- **Modularidade Estrita:** Separação clara entre contratos (`include/*.h`), implementações (`src/*.c`), testes automatizados (`tests/*.c`) e documentação (`docs/`).
- **Segurança de Memória:** Todo bloco de memória alocado dinamicamente (`malloc`) possui rotina explícita de liberação recursiva pós-ordem ou sequencial (`free`), sem vazamentos de memória (*memory leaks*) ao término da execução.
- **Robustez de Entrada:** Eliminação definitiva de vulnerabilidades clássicas de estouro de buffer associadas a `gets` ou `scanf("%s")`, substituídas por `fgets` monitorado e conversão com `strtol`.
- **Integridade Transacional:** O mecanismo de rollback garantiu que falhas de capacidade no grafo nunca deixassem registros órfãos na árvore de catálogo.

### 9.2. Limitações Conhecidas e Trabalhos Futuros

1. **Degeneração da BST:** Como a árvore implementada é uma BST clássica não balanceada, sequências estritamente ordenadas de inserção podem degradar a altura para $\mathcal{O}(n)$. Como trabalho futuro, propõe-se a evolução da estrutura para uma **Árvore AVL** ou **Árvore Rubro-Negra** (*Red-Black Tree*).
2. **Capacidade Estática do Grafo:** A matriz de adjacência adota limite nominal de 100 vértices simultâneos. Para acervos de milhares de livros, recomenda-se a substituição por uma representação em **Listas de Adjacência Dinâmicas** baseadas em tabelas hash para mapeamento de IDs.
3. **Persistência em Disco:** O sistema mantém o estado em memória primária (RAM). A implementação de persistência em arquivos binários ou texto formatado (JSON/CSV) permitiria reiniciar a aplicação preservando o estado do catálogo.

---

## 10. REFERÊNCIAS BIBLIOGRÁFICAS

1. **CORMEN, Thomas H.; LEISERSON, Charles E.; RIVEST, Ronald L.; STEIN, Clifford.** *Algoritmos: Teoria e Prática*. 3. ed. Rio de Janeiro: Elsevier, 2012.
2. **SEDGEWICK, Robert; WAYNE, Kevin.** *Algorithms*. 4th ed. Boston: Addison-Wesley Professional, 2011.
3. **ZIVIANI, Nivio.** *Projeto de Algoritmos: Com Implementações em Pascal e C*. 3. ed. São Paulo: Cengage Learning, 2011.
4. **KERNIGHAN, Brian W.; RITCHIE, Dennis M.** *C: A Linguagem de Programação Padrão ANSI*. Rio de Janeiro: Campus, 1990.
5. **CASTRO, Heber.** *Especificação do Projeto Prático Individual de Estrutura de Dados — Tema C (Biblioteca)*. 2026.

---

## 11. DECLARAÇÃO TRANSPARENTE DO USO DE FERRAMENTAS DE INTELIGÊNCIA ARTIFICIAL

Em estrito cumprimento aos preceitos éticos acadêmicos e às orientações do Professor Me. Heber Castro:

- **Declaração:** O autor declara que utilizou ferramentas de Inteligência Artificial Generativa (Google DeepMind / Antigravity com modelos Gemini) durante o desenvolvimento deste projeto.
- **Escopo de Utilização:**
  - Auxílio na estruturação inicial de diretórios e Makefile multiplataforma;
  - Sugestão de casos de teste de borda para validação de entradas incorretas e overflow numérico;
  - Apoio na padronização da documentação Doxygen nos cabeçalhos;
  - Revisão ortográfica e formatação do relatório técnico em Markdown.
- **Responsabilidade do Autor:** Toda a lógica de ponteiros, algoritmos de percurso, manipulação da matriz de adjacência, BFS com vizinhos ordenados, política de rollback e testes integrados foram revisados, depurados, compilados e validados pelo discente, assegurando a compreensão integral e a autoria do trabalho apresentado.


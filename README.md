# Sistema de Gestão de Biblioteca — Estrutura de Dados em C (Tema C)

Projeto prático individual de **Estrutura de Dados em C** focado no domínio de uma **Biblioteca** (Tema C). O sistema integra de forma atômica e consistente estruturas de dados clássicas: **Árvore Binária de Busca (BST)**, **Grafo não direcionado com Busca em Largura (BFS)**, **Pilha Dinâmica Encadeada de Histórico**, **Lista Encadeada**, **Fila Dinâmica FIFO** e **Algoritmos Próprios de Ordenação (Merge Sort / Quick Sort)**, além de rotinas estritas de validação segura de entradas (`fgets` e `strtol`).

---

## Sumário

- [Arquitetura do Projeto](#arquitetura-do-projeto)
- [Regras de Domínio e Modelagem](#regras-de-domínio-e-modelagem)
- [Funcionalidades e Menu Interativo](#funcionalidades-e-menu-interativo)
- [Como Compilar e Executar](#como-compilar-e-executar)
- [Execução dos Testes Automatizados](#execução-dos-testes-automatizados)
- [Tabela de Cenários de Teste](#tabela-de-cenários-de-teste)
- [Autoria e Declaração de IA](#autoria-e-declaração-de-ia)

---

## Arquitetura do Projeto

A organização de diretórios e arquivos segue estritamente o padrão profissional em C:

```text
ProjetoPratico_Biblioteca/
├── include/                 # Arquivos de cabeçalho (.h) com structs, enums e protótipos
│   ├── bst.h                # Árvore Binária de Busca (indexação por ID e percursos)
│   ├── grafo.h              # Grafo não direcionado (relações entre livros, BFS e rollback)
│   ├── pilha.h              # Pilha dinâmica encadeada (histórico de alterações de estado)
│   ├── fila.h               # Fila dinâmica encadeada FIFO (suporte à BFS)
│   ├── lista.h              # Lista simplesmente encadeada com inserção no fim
│   ├── ordenacao.h          # Merge Sort e Quick Sort próprios sobre cópias de livros
│   └── io.h                 # Validação estrita de strings, inteiros e limites
├── src/                     # Implementações dos módulos (.c) e ponto de entrada
│   ├── main.c               # Menu interativo principal (opções 0 a 11)
│   ├── bst.c                # Implementação das operações da BST e percursos
│   ├── grafo.c              # Implementação do grafo, matriz, BFS e transações
│   ├── pilha.c              # Implementação da pilha de histórico (LIFO)
│   ├── fila.c               # Implementação da fila dinâmica (FIFO)
│   ├── lista.c              # Implementação da lista encadeada (O(1) no fim)
│   ├── ordenacao.c          # Implementação de Merge Sort e Quick Sort próprios
│   └── io.c                 # Rotinas de leitura segura sem contaminação de buffer
├── tests/                   # Baterias de testes automatizados reproduzíveis
│   ├── test_etapa2.c        # Testes de Lista (R03), Pilha (R04), Fila (R05) e Ordenação (R06)
│   ├── test_etapa3.c        # Testes de BST (R07), Validação Segura (R01) e Integridade
│   ├── test_etapa4.c        # Testes de Grafo (R08), BFS em ordem crescente e Rollback
│   └── test_integrado.c     # Teste Integrado de ponta a ponta (End-to-End da Seção 12)
├── docs/                    # Documentação técnica e relatórios acadêmicos
│   ├── relatorio_tecnico.md # Relatório técnico completo detalhado (Seção 13)
│   └── relatorio_tecnico.pdf# Versão compilada em PDF do relatório
├── Makefile                 # Script de compilação compatível com make e mingw32-make
├── .gitignore               # Exclusão de binários e diretórios temporários de build
└── README.md                # Este documento descritivo
```

---

## Regras de Domínio e Modelagem

1. **Registo do Livro (`Livro`)**:
   - `id`: Inteiro positivo obrigatório (`> 0`), único no catálogo.
   - `titulo`: Texto de até 79 caracteres úteis (`char[80]`, aceita espaços). Rejeita strings vazias ou compostas apenas por espaços.
   - `autor`: Texto de até 79 caracteres úteis (`char[80]`, aceita espaços).
   - `ano_publicacao`: Inteiro restrito ao intervalo de `1` a `2026`.
2. **Árvore Binária de Busca (`BST`)**:
   - Indexa os registros pelo campo `id`.
   - Remoção com tratamento dos 3 casos clássicos (folha, 1 filho, 2 filhos via sucessor em-ordem).
   - Exportação independente para vetor auxiliar sem expor ponteiros internos da árvore.
3. **Grafo Não Direcionado e Não Ponderado (`Grafo`)**:
   - Suporte simultâneo a **100 vértices ativos** (`MAX_VERTICES = 100`).
   - Mapeamento bidirecional `ID arbitrário <-> índice compacto [0..99]`.
   - Rejeição estrita de laços $(A, A)$, arestas duplicadas $(A, B) \equiv (B, A)$ e IDs inexistentes.
   - Remoção em cascata: exclusão de vértice remove todas as arestas incidentes e libera o slot para novos cadastros.
4. **Busca em Largura (`BFS`)**:
   - Marca cada vértice como visitado no momento do `enqueue` (evitando duplicação na fila).
   - Percorre e enfileira vizinhos estritamente em **ordem crescente de ID**.
   - Origem isolada gera lista contendo apenas a si mesma com distância 0.
5. **Transações e Rollback (Consistência BST + Grafo)**:
   - Se o cadastro na BST ocorrer mas o Grafo atingir capacidade máxima (100 vértices), a inserção na BST é revertida (*rollback*) imediatamente.
6. **Regra do Histórico (Pilha)**:
   - Registra **exclusivamente** operações de alteração de estado com sucesso:
     - `Opção 1`: Cadastro de livro;
     - `Opção 3`: Remoção de livro;
     - `Opção 6`: Adição de conexão;
     - `Opção 7`: Remoção de conexão.
   - Consultas, buscas, listagens e ordenações não entram na pilha.
   - Exibição não destrutiva das até 5 ações mais recentes (topo para a base).

---

## Funcionalidades e Menu Interativo

O menu principal em `src/main.c` implementa as 11 opções de referência mais o encerramento:

| Opção | Ação | Estruturas Envolvidas | Registra Histórico? |
|---|---|---|---|
| **[1]** | Cadastrar livro | BST + Grafo (com Rollback) | **SIM** (`CADASTRO_LIVRO`) |
| **[2]** | Buscar livro por ID (e consultar menor/maior ID) | BST | NÃO (consulta) |
| **[3]** | Remover livro e respetivo vértice/arestas | BST + Grafo | **SIM** (`REMOCAO_LIVRO`) |
| **[4]** | Listar livros em-ordem (ID crescente) | BST | NÃO (consulta) |
| **[5]** | Ordenar e exibir livros por título | Vetor auxiliar + Merge Sort | NÃO (consulta) |
| **[6]** | Adicionar conexão entre dois livros | Grafo | **SIM** (`ADICAO_CONEXAO`) |
| **[7]** | Remover conexão entre dois livros | Grafo | **SIM** (`REMOCAO_CONEXAO`) |
| **[8]** | Consultar conexão direta entre dois livros | Grafo | NÃO (consulta) |
| **[9]** | Listar livros alcançáveis a partir de ID (BFS) | Grafo + Fila + Lista | NÃO (consulta) |
| **[10]**| Verificar existência de caminho entre dois IDs | Grafo + BFS | NÃO (consulta) |
| **[11]**| Consultar histórico de alterações recentes | Pilha (até 5 ações) | NÃO (consulta) |
| **[0]** | Encerrar o sistema e liberar memória | BST + Grafo + Pilha | Desalocação total |

---

## Como Compilar e Executar

O projeto foi desenvolvido em C99 estrito e compila sem avisos com as flags `-Wall -Wextra -pedantic -std=c99 -Iinclude -g`.

### No Windows (PowerShell / CMD via `mingw32-make`)

```powershell
# Compilação completa do executável
mingw32-make all

# Executar o sistema interativo
mingw32-make run
# ou diretamente:
.\biblioteca.exe

# Limpar binários e objetos
mingw32-make clean
```

### No Linux / macOS / MSYS2 (via `make`)

```bash
make all
make run
make clean
```

---

## Execução dos Testes Automatizados

O repositório possui quatro suítes de testes unitários e integrados:

```powershell
# Executar todas as suítes de teste (Etapas 2, 3, 4 e Integrado Final):
mingw32-make test

# Executar testes individualmente:
mingw32-make test2   # Lista, Fila, Pilha e Ordenação
mingw32-make test3   # BST, 3 Casos de Remoção e Validação Segura
mingw32-make test4   # Grafo, BFS Crescente, Caminho e Rollback
mingw32-make test5   # Teste Integrado End-to-End da Seção 12
```

---

## Tabela de Cenários de Teste

| ID Cenário | Entrada / Parâmetros | Operação Realizada | Resultado Esperado | Resultado Obtido | Status |
|---|---|---|---|---|---|
| **C01** | `ID = 10, Titulo = "Dom Casmurro", Ano = 1899` | Inserção na BST e Grafo | Sucesso em ambas as estruturas; nó adicionado | Sucesso; nó indexado e slot ativado | **PASSOU** |
| **C02** | `ID = 10` (já existente) | Inserção duplicada | Rejeição com código `BST_ERRO_DUPLICADO` | Rejeição sem alteração na árvore | **PASSOU** |
| **C03** | `ID = 0` e `ID = -5` | Inserção com ID inválido | Rejeição com código `BST_ERRO_ID_INVALIDO` | Rejeição sem alteração na árvore | **PASSOU** |
| **C04** | `ID = 999` (inexistente) | Remoção na BST | Rejeição com `BST_ERRO_NAO_ENCONTRADO` | Árvore permanece idêntica | **PASSOU** |
| **C05** | Nó folha (0 filhos) | Remoção na BST | Desalocação do nó folha; ponteiro pai vira NULL | Nó folha removido perfeitamente | **PASSOU** |
| **C06** | Nó com 1 filho | Remoção na BST | Conexão direta do pai ao filho do nó removido | Subárvore promovida com sucesso | **PASSOU** |
| **C07** | Nó com 2 filhos (incluindo Raiz) | Remoção na BST | Substituição pelo sucessor em-ordem | Sucessor promovido; raiz rebalanceada | **PASSOU** |
| **C08** | Vetor de cópias independentes | Merge Sort por título com desempate por ID | Vetor ordenado por título; BST original intacta | Títulos ordenados; BST inalterada por ID | **PASSOU** |
| **C09** | Aresta `(5000, 1200)` | Adição de aresta no Grafo | Criação simétrica em `adj[u][v]` e `adj[v][u]` | Aresta criada com sucesso | **PASSOU** |
| **C10** | Aresta `(1200, 5000)` após C09 | Adição duplicada inversa | Rejeição com `GRAFO_ERRO_ARESTA_DUPLICADA` | Rejeitado sem duplicar aresta | **PASSOU** |
| **C11** | Aresta `(5000, 5000)` | Tentativa de criar laço | Rejeição com `GRAFO_ERRO_LACO` | Rejeitado sem alterar matriz | **PASSOU** |
| **C12** | Vértice com 3 conexões incidentes | Remoção de vértice no Grafo | Limpeza de todas as 3 arestas e slot liberado | Arestas zeradas; slot reutilizável | **PASSOU** |
| **C13** | Vizinhos desordenados `(50, 20, 40, 30)` | BFS a partir da raiz | Exploração estrita em ordem crescente `20,30,40,50` | Ordem exata crescente na lista | **PASSOU** |
| **C14** | Vértice desconectado isolado | BFS a partir do isolado | Lista gerada contendo apenas o próprio nó | Lista com 1 elemento e distância 0 | **PASSOU** |
| **C15** | Componentes desconectados | Verificação de caminho | Retorno de `existe = false` | Confirmada ausência de caminho | **PASSOU** |
| **C16** | ID ausente no grafo | Verificação de caminho | Rejeição com `GRAFO_ERRO_VERTICE_NAO_ENCONTRADO` | Erro reportado sem falso silencioso | **PASSOU** |
| **C17** | 100 livros cadastrados (limite) | Tentativa de inserir 101º livro | Falha no Grafo e Rollback imediato na BST | BST e Grafo intactos com 100 nós | **PASSOU** |
| **C18** | Texto vazio `""` ou só espaços `"   "` | Validação de entrada textual | Rejeição com código de erro correspondente | Rejeitado sem registrar | **PASSOU** |
| **C19** | Texto com mais de 79 caracteres | Leitura com buffer de 80 bytes | Descarte do excesso no stream e rejeição | Limpeza sem contaminação do buffer | **PASSOU** |
| **C20** | Ano = 0, Ano = -10, Ano = 2027 | Validação de ano de publicação | Rejeição fora do intervalo `[1, 2026]` | Rejeitado com código de erro | **PASSOU** |
| **C21** | 9 ações de alteração de estado | Consulta da Pilha de Histórico | Exibição das 5 mais recentes sem desempilhar | 5 exibidas; tamanho manteve-se 9 | **PASSOU** |
| **C22** | Buscas, listagens e consultas | Verificação de estado da Pilha | Pilha inalterada após operações de leitura | Pilha permaneceu com tamanho exato | **PASSOU** |

---

## Autoria e Declaração de IA

* **Autor**: Yuri (Aluno do curso de Ciência da Computação / Engenharia de Software)
* **Disciplina**: Estrutura de Dados
* **Professor**: Me. Heber Castro
* **Tema**: Tema C — Sistema de Gestão de Biblioteca
* **Declaração de Uso de Ferramentas de IA**: Em total transparência e conformidade com as diretrizes da disciplina, declara-se a utilização de ferramentas de Inteligência Artificial Generativa (Antigravity / Gemini) como suporte ao planejamento arquitetural, assistência de pair-programming, revisão de casos de teste de borda e formatação de documentação técnica. Toda a lógica algorítmica, verificação de conformidade de tipos e testes foram inspecionados, compilados e validados pelo autor.

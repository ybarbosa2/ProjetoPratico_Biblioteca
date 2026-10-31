# Sistema de Gestão de Biblioteca — Estrutura de Dados em C (Tema C)

Projeto prático individual de **Estrutura de Dados em C** aplicado ao domínio de uma **Biblioteca**, integrando múltiplas estruturas de dados clássicas num sistema coeso com validação de integridade referencial.

---

## Arquitetura do Projeto

```text
ProjetoPratico_Biblioteca/
├── include/            # Ficheiros de cabeçalho (.h) com structs e protótipos documentados
│   ├── bst.h           # Árvore Binária de Busca (indexação de livros por ID)
│   ├── grafo.h         # Grafo não direcionado e não ponderado (relações entre livros por ID)
│   ├── lista.h         # Lista simplesmente encadeada (inserção no fim, apoio à BFS)
│   ├── pilha.h         # Pilha dinâmica encadeada (histórico de alterações de estado)
│   ├── fila.h          # Fila dinâmica encadeada (apoio à Busca em Largura - BFS)
│   └── ordenacao.h     # Algoritmos de ordenação sobre o catálogo exportado
├── src/                # Implementações (.c) e ponto de entrada (main.c)
│   ├── main.c
│   ├── bst.c
│   ├── grafo.c
│   ├── lista.c
│   ├── pilha.c
│   ├── fila.c
│   └── ordenacao.c
├── tests/              # Casos de teste automatizados e scripts de verificação
├── docs/               # Documentação técnica e relatórios do projeto
├── Makefile            # Script de compilação (compatível com make e mingw32-make)
├── .gitignore          # Exclusão de objetos e binários compilados
└── README.md           # Documentação geral do repositório
```

---

## Regras de Modelagem (Tema C — Biblioteca)

1. **Registo de Livro (`Livro`)**:
   - `id`: inteiro positivo (`> 0`) e único.
   - `titulo`: `char[80]` (até 79 caracteres úteis, aceita espaços).
   - `autor`: `char[80]` (até 79 caracteres úteis, aceita espaços).
   - `ano_publicacao`: inteiro no intervalo `[1, 2026]`.
2. **Árvore Binária de Busca (`BST`)**:
   - Indexa todos os livros pelo campo `id`.
   - Suporta inserção, busca, remoção (0, 1 ou 2 filhos) e percursos (em-ordem, pré-ordem e pós-ordem).
3. **Grafo de Relações (`Grafo`)**:
   - Não direcionado e não ponderado, com capacidade mínima para **100 vértices simultâneos** (`MAX_VERTICES = 100`).
   - Relaciona entidades exclusivamente pelo `id` através de um mapeamento compacto `ID -> índice`, sem armazenar ponteiros para nós da BST.
4. **Pilha Dinâmica Encadeada (`Pilha`)**:
   - Regista o histórico cronológico inverso (LIFO) das alterações de estado (tipo de ação e IDs envolvidos).
5. **Fila Dinâmica (`Fila`) e Lista Encadeada (`Lista`)**:
   - A `Fila` controla a fronteira de exploração da **Busca em Largura (BFS)** no grafo.
   - A `Lista` (com inserção no fim) armazena a ordem de visitação e os vizinhos obtidos na BFS.
6. **Ordenação (`ordenacao`)**:
   - Exporta os elementos da BST para um vetor auxiliar e ordena por diferentes critérios (Título, Autor, Ano de Publicação, ID).

---

## Como Compilar e Executar

### Usando `make` (Linux, macOS, Git Bash / MSYS2 no Windows)

```bash
make
make run
```

### Usando `mingw32-make` (Windows PowerShell / CMD)

```powershell
mingw32-make
mingw32-make run
```

### Limpar ficheiros compilados

```bash
make clean
# ou no Windows:
mingw32-make clean
```


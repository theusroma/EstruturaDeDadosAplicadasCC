#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definindo os limites máximos
#define MAX_VERTICES 20000  // Número máximo de vértices
#define MAX_ID 500000       // Número máximo para IDs únicos
#define MAX_LINHA 100       // Tamanho máximo de uma linha do CSV

// ==========================
// Estruturas Globais
// ==========================

// Matriz de adjacência para representar o grafo
int grafo[MAX_VERTICES][MAX_VERTICES];

// Vetores de controle para busca
int visitado[MAX_VERTICES];
int antecessor[MAX_VERTICES];

// Mapeamento de IDs reais (ex.: 101001) para índices internos (0, 1, 2, ...)
int idParaIndice[MAX_ID];        // Converte ID real para índice
int indiceParaId[MAX_VERTICES];  // Converte índice interno para ID real

// Contador de vértices únicos no grafo
int numVertices = 0;

// ==========================
// Implementação da Fila (BFS)
// ==========================

// Estrutura da fila
typedef struct {
    int itens[MAX_VERTICES];
    int frente, tras;
} Fila;

// Inicializa a fila
void inicializaFila(Fila *f) {
    f->frente = -1;
    f->tras = -1;
}

// Verifica se a fila está vazia
int filaVazia(Fila *f) {
    return f->frente == -1;
}

// Enfileira um elemento
void enfileira(Fila *f, int valor) {
    if (f->tras == MAX_VERTICES - 1) return;  // Fila cheia
    if (f->frente == -1) f->frente = 0;
    f->tras++;
    f->itens[f->tras] = valor;
}

// Desenfileira um elemento
int desenfileira(Fila *f) {
    if (filaVazia(f)) return -1;
    int valor = f->itens[f->frente];
    if (f->frente >= f->tras) {
        f->frente = -1;
        f->tras = -1;
    } else {
        f->frente++;
    }
    return valor;
}

// ==========================
// Função de Mapeamento de IDs
// ==========================

// Converte um ID real (ex.: 101001) para um índice interno
int obterIndice(int id) {
    if (idParaIndice[id] == -1) {
        idParaIndice[id] = numVertices;
        indiceParaId[numVertices] = id;
        numVertices++;
    }
    return idParaIndice[id];
}

// ==========================
// Leitura do Arquivo CSV
// ==========================

void lerCSV(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "r");

    if (!arquivo) {
        printf("Erro ao abrir o arquivo %s\n", nomeArquivo);
        exit(1);
    }

    char linha[MAX_LINHA];

    // Lê e ignora a primeira linha (cabeçalho)
    fgets(linha, sizeof(linha), arquivo);

    // Lê cada linha do arquivo CSV
    while (fgets(linha, sizeof(linha), arquivo)) {
        int origemId, destinoId;

        // Faz o parsing da linha, buscando dois inteiros separados por vírgula
        if (sscanf(linha, "%d,%d", &origemId, &destinoId) == 2) {
            // Mapeia IDs para índices internos
            int origem = obterIndice(origemId);
            int destino = obterIndice(destinoId);

            // Preenche a matriz de adjacência (grafo não direcionado)
            grafo[origem][destino] = 1;
            grafo[destino][origem] = 1;
        }
    }

    fclose(arquivo);
}

// ==========================
// Algoritmo de Busca em Largura (BFS)
// ==========================

void bfs(int origemId, int destinoId) {
    // Verifica se os IDs estão no grafo
    if (idParaIndice[origemId] == -1 || idParaIndice[destinoId] == -1) {
        printf("Origem ou destino não encontrados no grafo.\n");
        return;
    }

    int origem = idParaIndice[origemId];
    int destino = idParaIndice[destinoId];

    // Inicializa a fila
    Fila f;
    inicializaFila(&f);

    // Reseta os vetores de controle
    memset(visitado, 0, sizeof(visitado));
    memset(antecessor, -1, sizeof(antecessor));

    // Começa a busca a partir da origem
    visitado[origem] = 1;
    enfileira(&f, origem);

    while (!filaVazia(&f)) {
        int atual = desenfileira(&f);

        // Se chegou no destino, para
        if (atual == destino) break;

        // Percorre os vizinhos
        for (int i = 0; i < numVertices; i++) {
            if (grafo[atual][i] && !visitado[i]) {
                visitado[i] = 1;
                antecessor[i] = atual;
                enfileira(&f, i);
            }
        }
    }

    // Verifica se encontrou o destino
    if (!visitado[destino]) {
        printf("Caminho não encontrado.\n");
        return;
    }

    // Reconstrói o caminho
    int caminho[MAX_VERTICES];
    int tamanho = 0;
    int atual = destino;

    while (atual != -1) {
        caminho[tamanho++] = atual;
        atual = antecessor[atual];
    }

    // Exibe o menor caminho encontrado
    printf("\nMenor caminho:\n");
    for (int i = tamanho - 1; i >= 0; i--) {
        printf("%d", indiceParaId[caminho[i]]);
        if (i != 0) printf(" -> ");
    }
    printf("\n");
}

// ==========================
// Função Principal
// ==========================

int main() {
    // Inicializa o mapeamento de IDs para -1 (não mapeado)
    for (int i = 0; i < MAX_ID; i++) {
        idParaIndice[i] = -1;
    }

    // Zera a matriz do grafo
    memset(grafo, 0, sizeof(grafo));

    // Caminho do arquivo CSV
    char nomeArquivo[200] = "conexoes.csv";

    // Leitura do arquivo
    printf("Lendo conexões do arquivo %s...\n", nomeArquivo);
    lerCSV(nomeArquivo);

    // Recebe os IDs de origem e destino do usuário=
    int origemId, destinoId;
    printf("\nInforme o ponto de origem (ID): ");
    scanf("%d", &origemId);

    printf("Informe o ponto de destino (ID): ");
    scanf("%d", &destinoId);

    // Executa a busca
    bfs(origemId, destinoId);

    return 0;
}


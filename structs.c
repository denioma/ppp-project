// Ficheiro fonte autónomo de implementação de estutura de dados para o programa principal
// Rodrigo Machado (2019218299) - PPP - 2020 

#define QUEUESIZE 100

#define CATSIZE 50

static int root;

// Estrutura da dados
static struct data {
    char category[CATSIZE]; // Categoria do orçamento
    float budget;           // Orçamento
    float spent;            // Gasto
    int link;               // Implementação da lista ligada
} queue[QUEUESIZE];         // Matriz de elementos da lista ligada

// Inicialização da lista ligada
void root_init(void) {
    root = -1;
    for (int i = 0; i < QUEUESIZE; i++) {
        queue[i].budget = 0.00;
        queue[i].spent = 0.00;
        queue[i].link = -2;
    }
}

#include <string.h>

// Colocação de categorias na lista de forma alfabética, independente da ordem de entrada
int place(char *cat, float *budget) {
    int i;  // Indíce na lista
    int prev, next; // Usados na colocação alfabetizada das categorias
    for (i = 0; i < QUEUESIZE && queue[i].link != -2; i++); // Encontra a primeira posição livre
    if (i >= QUEUESIZE) return 1; // Lista cheia

    // Colocação da categoria na lista de forma alfabética
    strncpy(queue[i].category, cat, CATSIZE);
    queue[i].budget = *budget;
    if (root < 0) {
        queue[i].link = -1;
        root = i;
    } else if (strcmp(queue[root].category, cat) < 0) {
        queue[i].link = root;
        root = i;
    } else {
        prev = root;
        next = queue[root].link;
        while (next >= 0 && strcmp(queue[next].category, cat) >= 0) {
            prev = next;
            next = queue[next].link;
        }
        if (next < 0) {
            queue[prev].link = i;
            queue[i].link = -1;
        } else {
            queue[prev].link = i;
            queue[i].link = next;
        }
    }
    return 0; // Colocação bem sucedida
}

// Retira o primeiro dado da lista, 
int get(char *cat, int *cat_length, float *budget, float *spent) {
    if (root < 0) return 1; // Lista vazia

    int i, j = -1;

    // i aponta para o último a sair
    for (i = root; queue[i].link != -1; i = queue[i].link) j = i; // j aponta para o anterior

    if (j == -1) root = -1;     // Fila vai ser esvaziada
    else queue[j].link = -1;    // Fila continua com itens, j é agora o último elemento 
    
    queue[i].link = -2; // i fica livre para inserções
    // Copia os dados retirados para a função chamante
    strncpy(cat, queue[i].category, *cat_length);   
    *budget = queue[i].budget;                      
    *spent = queue[i].spent;

    if (j == -1) root_init();   // Re-inicializa a lista se ficar vazia

    return 0; // Item da lista retirado com sucesso
}

// Atualiza os gasto numa categoria existente na lista
int update(char *cat, float *spent) {
    int i, cond;
    // Procura pela categoria
    for (i = root; (cond = strcmp(queue[i].category, cat)) != 0 && i != -1; i = queue[i].link);
    if (cond != 0) return 1;    // Se a categoria não for encontrada na lista, retorna 1
    queue[i].spent += *spent;   // Atualiza os gastos da categoria 
    return 0;                   // Retorna 0 quando bem sucedido
}

/* Debug function to test result of list placement */
/*
#include <stdio.h>

void view_list(void) {
    for (int i = root; i != -1; i = queue[i].link) {
        printf("Category: %s\nBudget: %.2f\nSpent: %.2f\n", queue[i].category, queue[i].budget, queue[i].spent);
    }
} 
*/
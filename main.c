// Programa de tratamento de dados de orçamento mensal
// Rodrigo Machado (2019218299) - PPP - 2020

/*
    O programa lê dois ficheiros de entrada:
        1 - Categorias de orçamento
        2 - Despesas mensais

    O programa escreve os dados tratados num ficheiro binário com nome num ficheiro de texto:
        "config.txt"

    Estrutura do ficheiro "config.txt":
        Linha 1 - Nome do ficheiro binário de saída

    Códigos de erro:
        Código 1 - Número de argumentos de shell diferente de 2
        Código 2 - Erro em fopen() 
        Código 3 - config.txt vazio
*/

#include <stdio.h>

#define DEBUG 0

extern void root_init();

extern void view_list();

int set_budget(FILE *fp);

void import_data(FILE *fp);

void save_data(FILE *fp, int *catnum);

int main(int argc, char **argv) {
    /* Verifica que foram passados o número exato de argumentos necessários (2), caso não sejam o 
       programa termina com código 1 */
    if (argc != 3) {
        fprintf(stderr, "Expected 2 arguments (Files: Budget Allowances, Spendings).\n");
        return 1;
    }

    // Abertura do ficheiro de entrada 1, caso falhe o programa termina com código 2
    FILE *read =  NULL;
    if ((read = fopen(*(argv+1), "r")) == NULL) {
        fprintf(stderr, "Fatal error: Failed to open file \"%s\".\n", argv[1]);
        return 2;
    } 

    int size = 0;   // Contém o número de categorias do orçamento, usado para guardar os dados
    root_init();    // Inicializa a estrutura de dados de suporte ao programa
    size = set_budget(read);    // Leitura dos dados de orçamento mensal (ficheiro de entrada 1)
    fclose(read);
    read = NULL;
    
    // Abertura do ficheiro de entrada 2, caso falhe o programa termina com código 2 
    if ((read = fopen(*(argv+2), "r")) == NULL) {
        fprintf(stderr, "Fatal error: Failed to open file \"%s\".\n", argv[1]);
        return 2;
    }
    import_data(read);  // Leitura dos gastos mensais (ficheiro de entrada 2)
    fclose(read);

    #if DEBUG
    printf("\nCategories: %d\n", size);
    view_list(); 
    #endif
    
    // Abertura do ficheiro de configuração, caso falhe o programa termina com o código 2
    FILE *config = NULL;
    if ((config = fopen("config.txt", "r")) == NULL) {
        fprintf(stderr, "Fatal error: Failed to open config file.\n");
        return 2;
    }

    /* Leitura do nome de ficheiro binário de saída, se config.txt estiver vazio o programa termina 
    com o código 3 */
    char savename[50] = "";
    if (fscanf(config, "%s", savename) == 0) {
        fprintf(stderr, "\"config.txt\" is empty.\n");
        return 3;
    } 

    #if DEBUG
    printf("\nSave file name: %s\n", savename);
    #endif
    
    fclose(config);

    // Abertura do ficheiro binário de saída, caso falhe o programa termina com o código 2
    FILE *save = NULL;
    if ((save = fopen(savename, "w+b")) == NULL) {
        fprintf(stderr, "Fatal error: Failed to open file to write.\n");
        return 2;
    }
    
    save_data(save, &size); // Tratar e guardar os dados no ficheiro binário de saída
    fclose(save);
    return 0;
}

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SIZE 50

int getword(FILE *fp, char *word, int size, int *newline);

int verify_num(char *str);

extern int place(char *cat, float *budget);

// Função de leitura de dados de orçamento mensal
int set_budget(FILE *fp) {
    char c, category[SIZE] = "", convertnum[SIZE] = "";
    float budget;
    int count = 0;
    int newline;

    #if DEBUG 
    printf("Budget allowances:\n");
    #endif
    while (!feof(fp)) {
        getword(fp, category, SIZE, &newline);
        if (newline) {
            if ((c = fgetc(fp)) != EOF) ungetc(c, fp);
            else break;
            fprintf(stderr, "Entidade em falta, linha ignorada\n\n");
        }
        else {
            getword(fp, convertnum, SIZE, &newline);
            if (!newline) {
                fprintf(stderr, "Mais do que 2 entidades encontradas, entidades extra são ignoradas\n");
                while (fgetc(fp) != '\n');
            }
            if (verify_num(convertnum)) fprintf(stderr, "Valor inválido, categoria ignorada\n");
            else {
                budget = atof(convertnum);
                #if DEBUG 
                printf("%s: %.2f\n", category, budget);
                #endif
                if (place(category, &budget)) fprintf(stderr, "Falha na inserção da categoria\n\n");
                else printf("Categoria inserida com sucesso\n\n"); 
                count++;
            } 
        }
    }
    return count;
}

extern int update(char *cat, float *spent);

// Função de leitura dos dados de gastos mensais
void import_data(FILE *fp) {
    char c, str[SIZE] = "";
    float budget;
    int newline = 0;

    #if DEBUG 
    printf("\nExpenses:\n");
    #endif

    while(!feof(fp)) {
        getword(fp, str, SIZE, &newline);
        if (newline) {
            if ((c = fgetc(fp)) != EOF) ungetc(c, fp);
            else break;
            fprintf(stderr, "Entidades em falta, linha ignorada\n\n");
        } 
        else {
            #if DEBUG
            printf("Description: %s\n", str);
            #endif
            getword(fp, str, SIZE, &newline);
            if (newline) fprintf(stderr, "Entidade em falta, linha ignorada\n\n");
            else {
                if (verify_num(str)) {
                    fprintf(stderr, "Valor inválido, despesa ignorada\n\n");
                    while(fgetc(fp) != '\n');
                } else {
                    budget = atof(str);
                    #if DEBUG 
                    printf("Spent: %.2f\n", budget);
                    #endif
                    getword(fp, str, SIZE, &newline);
                    if (!newline) {
                        fprintf(stderr, "Mais do que 2 entidades encontradas, entidades extra são ignoradas\n");
                        while (fgetc(fp) != '\n');
                    }
                    #if DEBUG
                    printf("Category: %s\n", str);
                    #endif
                    if (update(str, &budget)) fprintf(stderr, "Categoria não encontrada\n");
                    else printf("Despesa adicionada\n\n");
                }
            }
        }
    }
}

extern int get(char *cat, int *cat_length, float *budget, float *spent); // definedo em structs.c

/* Função para escrever os dados obtidos num ficheiro binário, num formato que permita a leitura
   e conversão para um ficheiro de texto legível com um programa autónomo */
void save_data(FILE *fp, int *catnum) {
    int i = 0, macro = SIZE;
    char overbudget[100][SIZE]; // Matriz de categorias com desvio do orçamento maior ou igual a 10% 
    
    fwrite(&macro, sizeof(macro), 1, fp); // Tamanho das strings 
    fwrite(catnum, sizeof(*catnum), 1, fp); // Número de categorias total

    char cat[SIZE] = "";
    float budget = 0, spent = 0, budget_sum = 0, spent_sum = 0;

    // Gasto mensais totais discriminados por categorias
    while (get(cat, &macro, &budget, &spent) == 0) {
        budget_sum += budget;   // Soma total do orçamento
        spent_sum += spent; // Soma total dos gastos
        // Guarda as categorias com desvio para escrita mais tarde
        if (spent/budget <= 0.9 || spent/budget >= 1.1) strncpy(overbudget[i++], cat, macro);
        fwrite(cat, sizeof(char), SIZE, fp);
        fwrite(&spent, sizeof(spent), 1, fp);
    }

    fwrite(&i, sizeof(i), 1, fp); // Número de categorias em overbudget
    for (int j = 0; j < i; j++) fwrite(overbudget[j], sizeof(char), SIZE, fp);
    float balance = spent_sum - budget_sum;
    fwrite(&balance, sizeof(balance), 1, fp);
}

/* Função para ler entidades de um pointeiro para ficheiro para uma string, e com verificações
   de newline (para permitir a validação dos dados de entrada) */
int getword(FILE *fp, char *word, int size, int *newline) {
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != EOF) {
        if (i < size && c != '\t' && c != '\n' && c != '\r') {
            *(word+(i++)) = c;
        } else {
            if (c == '\r' && (c = fgetc(fp)) != '\n') ungetc(c, fp);
            if (c == '\n' && newline != NULL) *newline = 1;
            else *newline = 0;
            *(word+i) = '\0';
            /* Se uma palavra for maior do que o tamanho máximo da string,
               descarta o resto até uma nova entidade */
            if (c != '\t' && c != '\n') while((c = fgetc(fp)) != '\t' || c != '\n');
            return 0;
        } 
    }
    *(word+i) = '\0';
    return 1;
}

/* Função para verificar um número por caracteres inválidos, recupera uma vírgula para um ponto
   para compatibilidade com atof() */
int verify_num(char *str) {
    unsigned int i = 0;
    while (*(str+i) != '\0') {
        if (*(str+i) == ',') *(str+i) = '.';
        else if (!isdigit(*(str+i)) && *(str+i) != '.') return 1;
        i++;
    }
    return 0;
}

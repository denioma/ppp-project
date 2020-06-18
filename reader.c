// Programa de tradução de ficheiro binário para ficheiro de texto legível
// Rodrigo Machado (2019218299) - PPP - 2020

/* 
   O programa recebe dois argumentos de shell:
    1 - Ficheiro binário de entrada, criado no programa principal
    2 - Ficheiro de texto de saída legível 
   
   Códigos de saída:
    0 - Sem erros
    1 - Argumentos em falta
    2 - Erro a abrir ficheiro
*/

#include <stdio.h>

#define DEBUG 0

int main(int argc, char **argv) {
    
    // Verificação de passagem de argumentos
    if (argc != 3) {
        fprintf(stderr, "Expected 2 arguments:\n\tArg1: Binary file from budget program\n\tArg2: Final filename\n");
        return 1;
    }

    FILE *fp, *out; 
    // fp -> Ficheiro binário
    // out -> Ficheiro de texto
    if ((fp = fopen(*(argv+1), "rb")) == NULL) {
        fprintf(stderr, "Failed to open file: \"%s\"\n", *(argv+1));
        return 2;
    } if ((out = fopen(*(argv+2), "wb")) == NULL) {
        fprintf(stderr, "Failed to open file: \"%s\"\n", *(argv+2));
        return 2;
    }

    // Tamanhos para uso na leitura dos dados binários
    int strsize = 0, catnum = 0;
    fread(&strsize, sizeof(strsize), 1, fp);
    fread(&catnum, sizeof(catnum), 1, fp);
    #if DEBUG 
    printf("String size - %d\n", strsize);
    printf("No. of categories - %d\n", catnum);
    #endif
 
    char read[strsize];
    for (int i = 0; i < strsize; i++) *(read+i) = '\0'; // Inicialização da string read
    float spent = 0.0;

    // Gastos mensais totais
    #if DEBUG
    printf("Gastos Mensais:");
    #endif
    fprintf(out, "Gastos Mensais:\n");
    for (int i = 0; i < catnum; i++) {
        fread(read, 1, strsize, fp);
        fread(&spent, sizeof(spent), 1, fp);
        #if DEBUG 
        printf("%s - %.2f\n", read, spent);
        #endif
        fprintf(out, "\t%s: %.2f\n", read, spent);
    }

    // Número de categorias para os desvios
    catnum = 0;
    fread(&catnum, sizeof(int), 1, fp);
    // Desvios individuais maiores ou iguais a 10% do valor orçamentado
    if (catnum > 0) {
        fprintf(out, "\nCategorias com desvio maior ou igual a 10%% do orçamento:\n");
        for (int i = 0; i < catnum; i++) {
            fread(read, sizeof(char), strsize, fp);
            fprintf(out, "\t%s\n", read);
        }
    } else fprintf(out, "\nNão houve nenhum desvio maior ou igual a 10%% do orçamento.\n");
    // Desvio total do orçamento mensal
    fread(&spent, sizeof(spent), 1, fp);
    fprintf(out, "\nDesvio global do orçamento mensal: %.2f", spent);
   
    fclose(fp);   // Fechar o ficheiro binário
    fclose(out);  // Fechar o ficheiro de texto
    return 0;
}
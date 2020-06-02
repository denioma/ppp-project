#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Expected 2 arguments:\n\tArg1: Binary file from budget program\n\tArg2: Final filename.\n");
        return 1;
    }

    FILE *fp, *out;
    fp = fopen(*(argv+1), "rb");
    out = fopen(*(argv+2), "wb");

    int strsize, catnum = 0;
    fread(&strsize, sizeof(strsize), 1, fp);
    printf("String size - %d\n", strsize); /* Debug */
    fread(&catnum, sizeof(catnum), 1, fp);
    printf("No. of categories - %d\n", catnum); /* Debug */
    
    char *read;
    read = malloc(strsize * sizeof(char));
    float spent;

    fprintf(out, "Gastos Mensais:\n");
    for (int i = 0; i < catnum; i++) {
        fread(read, 1, strsize, fp);
        fread(&spent, sizeof(spent), 1, fp);
        printf("%s - %.2f\n", read, spent);
        fprintf(out, "\t%s: %.2f\n", read, spent);
    }

    catnum = 0;
    fread(&catnum, sizeof(int), 1, fp);

    if (catnum > 0) {
        fprintf(out, "\nCategorias com desvio maior ou igual a 10%% do orçamento:\n");
        for (int i = 0; i < catnum; i++) {
            fread(read, sizeof(char), strsize, fp);
            fprintf(out, "\t%s\n", read);
        }
    } else fprintf(out, "\nNão houve nenhum desvio maior ou igual a 10%% do orçamento.\n");

    fread(&spent, sizeof(spent), 1, fp);
    fprintf(out, "\nDesvio global do orçamento mensal: %.2f", spent);
    free(read);
    fclose(fp);
    fclose(out);
    return 0;
}
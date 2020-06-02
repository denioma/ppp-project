/*
    Ficheiro de entrada começa com duas linha sem dados, depois uma categoria e o seu orçamento por linha,
    duas linhas sem dados, e as despesas (descrição - valor - categoria)
*/

/*
    config.txt
    Line 1: Save file name
*/

/*
    Code 1: Shell arguments error
    Code 2: fopen() error
    Code 3: config.txt empty
*/

#include <stdio.h>

extern void root_init();

extern void view_list();

int set_budget(FILE *);

void import_data(FILE *);

void save_data(FILE *, int *);

int main(int argc, char **argv) {
    /* Check for shell argument, if less or more than one argument is provided 
    then program will return with return code 1 */
    if (argc != 3) {
        fprintf(stderr, "Expected 2 arguments (Files: Budget Allowances, Spendings).\n");
        return 1;
    }

    /* Input file open with check, if it fails to open then program returns with code 2 */
    FILE *read =  NULL;
    if ((read = fopen(*(argv+1), "r")) == NULL) {
        fprintf(stderr, "Failed to open file \"%s\".\n", argv[1]);
        return 2;
    } 

    /* Get all budget data from input data file */
    int size = 0;
    root_init();
    size = set_budget(read);
    fclose(read);
    
     /* Input file open with check, if it fails to open then program returns with code 2 */
    if ((read = fopen(*(argv+2), "r")) == NULL) {
        fprintf(stderr, "Failed to open file \"%s\".\n", argv[1]);
        return 2;
    }
    /* Gets all costs and store in struct */
    import_data(read);
    fclose(read);

    // puts(""); /* Debug */
    printf("Categories: %d\n", size); /* Debug */
    // view_list(); /* Debug */

    /* Open save file with check, if it fails to open then the program returns with code 2 */
    FILE *config = NULL;
    if ((config = fopen("config.txt", "r")) == NULL) {
        fprintf(stderr, "Failed to open config file.\n");
        return 2;
    }

    /* Get binary save file name from config.txt */
    char savename[50] = "";
    if (fscanf(config, "%s", savename) == 0) {
        fprintf(stderr, "\"config.txt\" is empty.\n");
        return 3;
    } 
    // printf("\nSave file name: %s\n", savename); /* Debug */
    fclose(config);


    FILE *save = NULL;
    if ((save = fopen(savename, "w+b")) == NULL) {
        fprintf(stderr, "Failed to open file to write.\n");
        return 2;
    }

    save_data(save, &size);
    fclose(save);
    return 0;
}

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define SIZE 50

int getword(FILE *, char *, int);

int verify_num(char *);

extern int place(char *, float *); // place(char* category, float budget); defined in structs.c

int set_budget(FILE *fp) {
    char category[SIZE] = "", convertnum[SIZE] = "";
    float budget;
    int count = 0;

    printf("Budget allowances:\n"); /* Debug */
    while (!feof(fp)) {
        getword(fp, category, SIZE);
        if (strcmp(category, "") == 0) break;
        getword(fp, convertnum, SIZE);
        if (verify_num(convertnum)) fprintf(stderr, "Invalid budget value!\n");
        else {
            budget = atof(convertnum);
            printf("%s: %.2f\n", category, budget); /* Debug */
            if (place(category, &budget)) fprintf(stderr, "Failure on placement.\n");
            else printf("Placement successful.\n"); 
            count++;
        } 
    }
    return count;
}

extern int update(char *, float *); // update(char *category, float spent)

void import_data(FILE *fp) {
    char category[SIZE] = "", convertnum[SIZE] = "";
    float budget;

    printf("\nExpenses:\n"); /* Debug */
    while(!feof(fp)) {
        getword(fp, category, SIZE);
        if (strcmp(category, "") == 0) break;
        printf("Description: %s\n", category); /* Debug */
        getword(fp, convertnum, SIZE);
        if (verify_num(convertnum)) {
            fprintf(stderr, "Invalid number! Skipping expense...\n\n");
            while(fgetc(fp) != '\n');
        } else {
            budget = atof(convertnum);
            printf("Spent: %.2f\n", budget); /* Debug */
            getword(fp, category, SIZE);
            printf("Category: %s\n", category); /* Debug */
            if (update(category, &budget)) fprintf(stderr, "Category not found.\n");
            else printf("Expense added.\n\n");
        }
    }
}

extern int get(char *cat, int *cat_length, float *budget, float *spent); // defined in structs.c

void save_data(FILE *fp, int *catnum) {
    int i = 0, macro = SIZE;
    char overbudget[100][SIZE];
    
    fwrite(&macro, sizeof(macro), 1, fp); // String size variable 
    fwrite(catnum, sizeof(*catnum), 1, fp); // Total num of categories

    char cat[SIZE] = "";
    float budget = 0, spent = 0, budget_sum = 0, spent_sum = 0;
    while (get(cat, &macro, &budget, &spent) == 0) {
        budget_sum += budget;
        spent_sum += spent;
        if (spent/budget <= 0.9 || spent/budget >= 1.1) strncpy(overbudget[i++], cat, macro);
        fwrite(cat, sizeof(char), SIZE, fp);
        fwrite(&spent, sizeof(spent), 1, fp);
    }
    fwrite(&i, sizeof(i), 1, fp); // Total num of overspent categories
    for (int j = 0; j < i; j++) fwrite(overbudget[j], sizeof(char), SIZE, fp);
    float balance = spent_sum - budget_sum;
    fwrite(&balance, sizeof(balance), 1, fp);
}

int getword(FILE *fp, char *word, int size) {
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != EOF) {
        if (i < size && c != '\t' && c != '\n') {
            *(word+(i++)) = c;
        } else {
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
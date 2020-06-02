#define QUEUESIZE 100

#define CATSIZE 50

static int root;

static struct data {
    char category[CATSIZE];
    float budget;
    float spent;
    int link;
} queue[QUEUESIZE];

void root_init(void) {
    root = -1;
    for (int i = 0; i < QUEUESIZE; i++) {
        queue[i].spent = 0.00;
        queue[i].link = -2;
    }
}

#include <string.h>

/* place: not sure if supposed to place categories alphabetically */

int place(char *cat, float *budget) {
    int i;  // Holds the vector position
    int prev, next; // For alphabetical placement
    for (i = 0; i < QUEUESIZE && queue[i].link != -2; i++); // Find first available index
    if (i >= QUEUESIZE) return 1; // Queue is full

    // Place budget data in queue
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
    return 0; // Placement successful
}

/* get: returns 0 on success, 1 on empty queue */

int get(char *cat, int *cat_length, float *budget, float *spent) {
    if (root < 0) return 1; // Empty queue

    int i, j = -1;

    // i holds the last queue item
    for (i = root; queue[i].link != -1; i = queue[i].link) {
        j = i; // j holds the preceding item
    }

    if (j == -1) root = -1; // Queue will be emptied
    else {
        queue[j].link = -1;
    }

    queue[i].link = -2;
    strncpy(cat, queue[i].category, *cat_length);
    *budget = queue[i].budget;
    *spent = queue[i].spent;

    if (j == -1) root_init();

    return 0; // Queue item successfully removed
}

/* Update budget category with expenses, returns 1 if category not in list, else return 0 */

int update(char *cat, float *spent) {
    int i, cond;
    for (i = root; (cond = strcmp(queue[i].category, cat)) != 0 && i != -1; i = queue[i].link);
    if (cond != 0) return 1;
    queue[i].spent += *spent;
    return 0;
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
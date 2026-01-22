#include "pile.h"
#include <stdlib.h>

typedef struct Element {
    Coord coord;
    struct Element *next;
} Element;

static Element *top = NULL;

void initpile(void) {
    top = NULL;
}

void empiler(int i, int j) {
    Element *nouv = (Element*)malloc(sizeof(Element));
    nouv->coord.i = i;
    nouv->coord.j = j;
    nouv->next = top;
    top = nouv;
}

Coord depiler(void) {
    Coord coord = {-1, -1};
    if(top != NULL) {
        Element *tmp = top;
        coord = top->coord;
        top = top->next;
        free(tmp);
    }
    return coord;
}

int pilevide(void) {
    return top == NULL;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "card.h"

#define VALUES_LEN 13
#define SUITS_LEN 4 

char values[VALUES_LEN][3] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
char suits[SUITS_LEN] = {'♥', '♠', '♦', '♣'}; // must change

struct Card* createCard(char suit, char* value) {
    struct Card *c = (struct Card*) malloc(sizeof(struct Card));
    strcpy(c->value, value);
    c->suit = suit;
    return c;
}

void printCard(struct Card *c) {
    printf("%c%c\n", c->suit, c->value);
}

int cardInternalValue(struct Card *c) {
    if (strcmp(c->value, "J") == 0)
        return 11;
    else if (strcmp(c->value, "Q") == 0)
        return 12;
    else if (strcmp(c->value, "K") == 0)
        return 13;
    else if (strcmp(c->value, "A") == 0)
        return 13;
    else if (strcmp(c->value, "10") == 0)
        return 10;
    else 
        return c->value[0] - 48;
}


int getBJValue(struct Card *c, int total) {
    if (strcmp(c->value, "10") == 0)
        return 10;
    else if (strcmp(c->value, "J") == 0)
        return 10;
    else if (strcmp(c->value, "Q") == 0)
        return 10;
    else if (strcmp(c->value, "K") == 0)
        return 10;
    else if (strcmp(c->value, "A") == 0) {
        if (total < 11) 
            return 11;
        else 
            return 1;
    }
    else 
        return c->value[0] - 48;
}


void freeCard(struct Card* c) {
    free(c->value);
    free(c);
}
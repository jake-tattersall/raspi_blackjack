#ifndef CARD_H
#define CARD_H

#define VALUES_LEN 13
#define SUITS_LEN 4 

extern char *values[VALUES_LEN];
extern char *suits[SUITS_LEN];

struct Card {
    char *value; // 2,3,4...9,10,J,Q,K,A
    char *suit; // (H)eart, (S)pade, (D)iamond, (C)lub
};

struct Card* createCard(char *suit, char* value);

void printCard(struct Card *c);
int cardInternalValue(struct Card *c);
int getBJValue(struct Card *c, int total, int isLast);
void freeCard(struct Card *c);

#endif
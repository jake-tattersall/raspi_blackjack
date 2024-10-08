#ifndef CARD_H
#define CARD_H

#define VALUES_LEN 13
#define SUITS_LEN 4 

extern char values[VALUES_LEN][3];
extern char suits[SUITS_LEN];

struct Card {
    char *value;
    char suit;
};

struct Card* createCard(char suit, char* value);

void printCard(struct Card *c);
int cardInternalValue(struct Card *c);
int getBJValue(struct Card *c, int total);
void freeCard(struct Card *c);

#endif
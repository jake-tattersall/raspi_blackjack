#ifndef HAND_H
#define HAND_H

#include "card.h"

struct Hand {
    struct Card** cards; // Cards in this hand
    int numCards; // Number of cards in this hand
};

struct Hand* createHand();
void addCard(struct Hand* h, struct Card* c);
struct Card* removeLastCard(struct Hand* h);
int getHandValue(struct Hand* h);
struct Card* peekHand(struct Hand* h);
void printHand(struct Hand* h);
int canSplit(struct Hand* h);
int canDouble(struct Hand* h);
void freeHand(struct Hand* h);

#endif
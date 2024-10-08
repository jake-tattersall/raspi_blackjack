#ifndef DECK_H
#define DECK_H

#include "card.h"

#define MAXDECKS 4
#define MAXCARDS 208 // 52 * 4 decks

struct Deck {
    struct Card** deck;
    int len;
};


struct Deck* createDeck();
struct Card* draw(struct Deck* d);
void freeDeck(struct Deck* d);

#endif
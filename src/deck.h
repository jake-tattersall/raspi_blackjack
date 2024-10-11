#ifndef DECK_H
#define DECK_H

#include "card.h"

struct Deck {
    struct Card** deck; // The list of cards in the deck
    int len; // Number of cards in the deck
};


struct Deck* createDeck(int numDecks);
struct Card* draw(struct Deck* d);
void shuffle(struct Deck* d);
void freeDeck(struct Deck* d);

#endif
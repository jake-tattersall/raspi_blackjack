#include <stdlib.h>

#include "deck.h"
#include "card.h"

#define MAXDECKS 4
#define MAXCARDS 208 // 52 * 4 decks

struct Deck* createDeck() {
    struct Deck* d = (struct Deck*) malloc(sizeof(struct Deck*));
    d->len = 0;
    for (int i = 0; i < MAXDECKS; i++) {
        for (int j = 0; j < SUITS_LEN; j++) {
            for (int k = 0; k < VALUES_LEN; k++) {
                d->deck[d->len++] = createCard(suits[j], values[k]);
            }
        }
    }

    return d;
}


struct Card* draw(struct Deck* d) {
    return d->deck[d->len--];
}


// Add shuffle


void freeDeck(struct Deck* d) {
    for (int i = 0; i < d->len; i++) {
        freeCard(d->deck[i]);
    }
    free(d);
}
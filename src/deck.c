#include <stdlib.h>
#include <time.h>

#include "deck.h"
#include "card.h"

struct Deck* createDeck() {
    struct Deck* d = (struct Deck*) malloc(sizeof(struct Deck));
    d->deck = (struct Card**) malloc(MAXCARDS * sizeof(struct Card*));
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
    if (d->len == 0) 
        return NULL;

    return d->deck[d->len--];
}


void shuffle(struct Deck* d) {
    srand(time(NULL)); // Seed the random number generator
    for (int i = d->len - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        // Swap cards at indices i and j
        struct Card* temp = d->deck[i];
        d->deck[i] = d->deck[j];
        d->deck[j] = temp;
    }
}


void freeDeck(struct Deck* d) {
    for (int i = 0; i < d->len; i++) {
        freeCard(d->deck[i]);
    }
    free(d);
}
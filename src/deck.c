#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "deck.h"
#include "card.h"

#define NUMCARDS 52

// Creates a deck filled with numDecks decks. Does not shuffle the deck
struct Deck* createDeck(int numDecks) {
    struct Deck* d = malloc(sizeof(struct Deck));
    d->deck = malloc(NUMCARDS * numDecks * sizeof(struct Card*));
    d->len = 0;
    
    for (int i = 0; i < numDecks; i++) {
        for (int j = 0; j < SUITS_LEN; j++) {
            for (int k = 0; k < VALUES_LEN; k++) {
                d->deck[(d->len)++] = createCard(suits[j], values[k]);
            }
        }
    }

    return d;
}

// Returns the first card of the deck. If the deck is empty, returns NULL.
// The card is removed from the deck when drawn, so it must be freed separately from freeDeck.
struct Card* draw(struct Deck* d) {
    if (d->len == 0) 
        return NULL;

    return d->deck[--(d->len)];
}


// Returns the first card of the deck. If the deck is empty, returns NULL.
// The card is NOT removed from the deck
struct Card* peekDeck(struct Deck* d) {
    if (d->len == 0) 
        return NULL;

    return d->deck[d->len - 1];
}


// Shuffles the deck
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

// Frees all cards in the deck, then the deck itself
void freeDeck(struct Deck* d) {
    if (d == NULL) return;
    for (int i = 0; i < d->len; i++) {
        freeCard(d->deck[i]);
    }
    free(d->deck);
    free(d);
}
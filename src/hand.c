#include <stdio.h>
#include <stdlib.h>

#include "hand.h"
#include "card.h"


// Creates a hand. The cards property is set to NULL
struct Hand* createHand() {
    struct Hand* h = malloc(sizeof(struct Hand));
    h->numCards = 0;
    h->cards = NULL;
    return h;
}

// Adds the card to the hand. 
// If hand->cards is NULL, handles accordingly.
void addCard(struct Hand* h, struct Card* c) {
    if (h->cards == NULL) {
        h->numCards++;
        h->cards = malloc(sizeof(struct Card*));
    } else {
        h->cards = realloc(h->cards, ++(h->numCards) * sizeof(struct Card*));
    }   
    h->cards[h->numCards - 1] = c;
}

// Removes the last card in the hand
// Returns NULL if empty
struct Card* removeLastCard(struct Hand* h) {
    if (h->numCards == 0) return NULL;

    struct Card* temp = h->cards[h->numCards - 1];
    h->cards = realloc(h->cards, --(h->numCards) * sizeof(struct Card*));

    return temp;
}

// Returns the total BlackJack value of the hand
int getHandValue(struct Hand* h) {

    // THIS NEEDS SORTING ACES AT END
    // Malloc an int array of numCards size. int len = 0
    // If ace is card, add index to array
    // After for loop finished, for loop over the array

    int total = 0;
    for (int i = 0; i < h->numCards; i++) {
        total += getBJValue(h->cards[i], total);
    }
    return total;
}

// Returns the first [0] card in the hand.
// If the hand is empty, returns NULL
struct Card* peekHand(struct Hand* h) {
    if (h->numCards == 0 || h->cards == NULL) 
        return NULL;
    return h->cards[0];
}

// Displays each card in the hand
void printHand(struct Hand* h) {
    for (int i = 0; i < h->numCards; i++) {
        printCard(h->cards[i]);
    }
}

// Returns true if there are 2 cards in the hand and they are equal in internal value
int canSplit(struct Hand* h) {
    return canDouble(h) && cardInternalValue(h->cards[0]) == cardInternalValue(h->cards[1]);
}

// Returns true if there are 2 cards in the hand
int canDouble(struct Hand* h) {
    return h->numCards == 2;
}

// Frees all memory of the hand
void freeHand(struct Hand* h) {
    for (int i = 0; i < h->numCards; i++) {
        free(h->cards[i]);
    }
    free(h->cards);
    free(h);
}
#include <stdio.h>
#include <stdlib.h>

#include "deck.h"
#include "dealer.h"

// Creates a dealer. dealer->hand is set to NULL
struct Dealer *createDealer(struct Deck *d) {
    struct Dealer *dealer = malloc(sizeof(struct Dealer));
    dealer->deck = d;
    dealer->hand = NULL;

    return dealer;
}

// Returns the dealer's top card
struct Card* getDealerTopCard(struct Dealer* dealer) {
    if (dealer->hand->numCards == 0 || dealer->hand == NULL) 
        return NULL;

    return peekHand(dealer->hand);
}

// Makes sure the dealer's hand has at least 2 cards
void checkForTwoDealer(struct Dealer *dealer) {
    dealer->hand = createHand();
    while (dealer->hand->numCards < 2) {
        addCard(dealer->hand, draw(dealer->deck));
    }
}

// Returns 1 if the dealer has busted, otherwise 2
int checkBustedDealer(struct Dealer *dealer) {
    if (getHandValue(dealer->hand) > 21) 
        return 1;
    return 0;
}

// Preps the dealer for the next deal
void nextRoundDealer(struct Dealer *dealer) {
    free(dealer->hand);
    dealer->hand = NULL;
}

// Frees all memory in the dealer
void freeDealer(struct Dealer *dealer) {
    if (dealer == NULL) return;
    freeDeck(dealer->deck);
    free(dealer);
}

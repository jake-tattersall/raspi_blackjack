#ifndef DEALER_H
#define DEALER_H

#include "hand.h"

struct Dealer {
    struct Hand* hand; // The dealer's hand
    struct Deck* deck; // Pointer to the deck currently being played on
};

struct Dealer* createDealer(struct Deck* d);
struct Card* getDealerTopCard(struct Dealer* dealer);
void checkForTwoDealer(struct Dealer* d);
int dealerBust(struct Dealer* d);
void nextDealDealer(struct Dealer* d);
void freeDealer(struct Dealer* d);


#endif
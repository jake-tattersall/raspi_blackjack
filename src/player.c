#include <stdlib.h>

#include "player.h"

#include "deck.h"
#include "hand.h"

// Creates a player.
// player->bets and player->hands are set to NULL
// p->money set to -1
struct Player* createPlayer(struct Deck* d) {
    struct Player *p = (struct Player*) malloc(sizeof(struct Player));
    p->bets = NULL;
    p->hands = NULL;
    p->deck = d;
    p->currentHand = 0;
    p->numHands = 0;
    p->money = -1;
    
    return p;
}

// Returns the current hand
struct Hand* getCurrentHand(struct Player *p) {
    if (p->currentHand >= p->numHands)
        return NULL;

    return p->hands[p->currentHand];
}

// Adds a hand to the player. Used for creating the first hand or splitting
// If hand->cards is NULL, handles accordingly.
void addHand(struct Player* p) {
    if (p->hands == NULL) { 
        p->numHands++;
        p->hands = malloc(sizeof(struct Hand*)); 
    } else {
        p->numHands++;
        p->hands = realloc(p->hands, p->numHands * sizeof(struct Hand*));
    }
    p->hands[p->numHands-1] = createHand();
}

// Adds the passed bet to the last hand.
// Also subtracts the bet from the player's money
void addBet(struct Player* p, int bet) {
    if (p->bets == NULL) {
        p->bets = malloc(sizeof(int));
    } else {
        p->bets = realloc(p->bets, p->numHands * sizeof(int));
    }
    p->bets[p->numHands - 1] = bet;
    p->money -= bet;
}

// Doubles the bet of the current hand (intended for Double Down)
void doubleBet(struct Player* p) {
    int temp = getCurrentBet(p);
    if (2 * temp > p->money + temp)
        return;

    p->bets[p->currentHand] = 2 * temp;
    p->money -= temp;
}

// Gets the bet of the current hand
int getCurrentBet(struct Player* p) {
    return p->bets[p->currentHand];
}

// Win the current bet
void payday(struct Player* p) {
    p->money += 2 * getCurrentBet(p);
}

// Win the current bet at BJ rate (1.5x)
void blackjackPayday(struct Player* p) {
    p->money += 2.5 * getCurrentBet(p);
}

// Get money back from current bet
void push(struct Player* p) {
    p->money += getCurrentBet(p);
}

// Makes sure the player's hand has at least 2 cards
void checkForTwoPlayer(struct Player* p) {
    while (getCurrentHand(p)->numCards < 2) {
        addCard(getCurrentHand(p), draw(p->deck));
    }
}

// Returns 1 if the current hand has busted, otherwise 2
int playerBust(struct Player* p) {
    if (getHandValue(getCurrentHand(p)) > 21)
        return 1;
    return 0;
}

// Preps the player for the next deal
void nextDealPlayer(struct Player* p) {
    for (int i = 0; i < p->numHands; i++) {
        freeHand(p->hands[i]);
    }
    free(p->hands);
    free(p->bets);
    p->hands = NULL;
    p->bets = NULL;
    p->numHands = 0;
    p->currentHand = 0;
}

// Frees all memory of player
void freePlayer(struct Player* p) {
    if (p == NULL) return;
    for (int i = 0; i < p->numHands; i++) {
        freeHand(p->hands[i]);
    }
    free(p->hands);
    free(p->bets);
    free(p);
}
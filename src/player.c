#include <stdlib.h>

#include "player.h"
#include "deck.h"
#include "hand.h"

// Creates a player.
// player->bets and player->hands are set to NULL
// p->money set to -1
struct Player* createPlayer(struct Deck* d) {
    struct Player *p = malloc(sizeof(struct Player));
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
    return p->hands[p->currentHand];
}

// Adds a hand to the player. Used for creating the first hand or splitting
// If hand->cards is NULL, handles accordingly.
void addHand(struct Player* p) {
    if (p->hands == NULL) { 
        p->numHands++;
        p->hands = malloc(sizeof(struct Hand*));
        p->bets = malloc(sizeof(int));
        p->currentHand = 0;
    } else {
        p->numHands++;
        p->hands = realloc(p->hands, p->numHands * sizeof(struct Hand*));
        p->bets = realloc(p->bets, p->numHands * sizeof(int));
    }
}

// Makes sure the player's hand has at least 2 cards
void checkForTwoPlayer(struct Player* p) {
    while (getCurrentHand(p)->numCards < 2) {
        addCard(getCurrentHand(p), draw(p->deck));
    }
}

// Returns 1 if the current hand has busted, otherwise 2
int checkBustedPlayer(struct Player* p) {
    if (getHandValue(getCurrentHand(p)) > 21)
        return 1;
    return 0;
}

// Preps the player for the next deal
void nextRoundPlayer(struct Player* p) {
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
    for (int i = 0; i < p->numHands; i++) {
        freeHand(p->hands[i]);
    }
    free(p->hands);
    free(p->bets);
    free(p);
}
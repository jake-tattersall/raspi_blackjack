#ifndef PLAYER_H
#define PLAYER_H

#include "hand.h"

struct Player {
    struct Hand** hands; // List of hands the player is playing on
    int numHands; // Number of hands in *hands
    int currentHand; // Index of the current hand
    int money; // Bank money to play with
    int *bets; // List of bets corresponding to each hand
    struct Deck* deck; // Pointer to the deck currently being played on
};

struct Player* createPlayer(struct Deck* d);
struct Hand* getCurrentHand(struct Player *p);
void addHand(struct Player* p);
void addBet(struct Player* p, int bet);
void doubleBet(struct Player* p);
int getCurrentBet(struct Player* p);
void payday(struct Player* p);
void blackjackPayday(struct Player* p);
void push(struct Player* p);
void checkForTwoPlayer(struct Player* p);
int playerBust(struct Player* p);
void nextDealPlayer(struct Player* p);
void freePlayer(struct Player* p);


#endif
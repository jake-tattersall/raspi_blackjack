#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "card.h"

char *values[VALUES_LEN] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
char *suits[SUITS_LEN] = {"H", "S", "D", "C"}; //{"♥", "♠", "♦", "♣"}; 


// Creates a card structure filled with the passed values
// Returns the filled card
struct Card* createCard(char* suit, char* value) {
    struct Card *c = (struct Card*) malloc(sizeof(struct Card));
    c->suit = malloc(3 * sizeof(char));
    c->value = malloc(3 * sizeof(char));
    strcpy(c->value, value);
    strcpy(c->suit, suit);
    return c;
}

// Displays the card in the way of "{Suit}{Value} "
void printCard(struct Card *c) {
    printf("%s%s ", c->suit, c->value);
}

// Gets the card's heirarchical value
// Numbers are number value, J = 11, Q = 12, K = 13, A = 14
int cardInternalValue(struct Card *c) {
    if (strcmp(c->value, "J") == 0)
        return 11;
    else if (strcmp(c->value, "Q") == 0)
        return 12;
    else if (strcmp(c->value, "K") == 0)
        return 13;
    else if (strcmp(c->value, "A") == 0)
        return 14;
    else if (strcmp(c->value, "10") == 0)
        return 10;
    else 
        return c->value[0] - 48;
}

// Gets the blackjack value of the card based on the hand total (Ace is 1 or 11)
// total and isLast help with detecting how to count the Aces
int getBJValue(struct Card *c, int total, int isLast) {
    if (strcmp(c->value, "10") == 0)
        return 10;
    else if (strcmp(c->value, "J") == 0)
        return 10;
    else if (strcmp(c->value, "Q") == 0)
        return 10;
    else if (strcmp(c->value, "K") == 0)
        return 10;
    else if (strcmp(c->value, "A") == 0) {
        if (isLast && total < 11) 
            return 11;
        else 
            return 1;
    }
    else 
        return c->value[0] - 48;
}

// Frees the memory of the card's properties and the card itself
void freeCard(struct Card* c) {
    if (c == NULL) return;
    free(c->value);
    free(c->suit);
    free(c);
}
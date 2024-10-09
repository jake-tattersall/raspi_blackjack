#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <locale.h>

#include "deck.h"
#include "player.h"
#include "dealer.h"

#define MAXDECKS 4
#define BET 100

char split_choices[4] = {'1', '2', '3', '4'};
char double_choices[3] = {'1', '2', '3'};
char default_choices[2] = {'1', '2'};

void reshuffle(struct Player* player, struct Dealer* dealer, struct Deck** deck);
void freeAll(struct Player* player, struct Dealer* dealer, struct Deck* deck);

int main() {
    //setlocale(LC_ALL, "");

    struct Deck* deck = createDeck(MAXDECKS);
    struct Player* player = createPlayer(deck);
    struct Dealer* dealer = createDealer(deck);

    player->money = 1000;

    printf("Starting...\n\n");
    Sleep(2000);

    while (player->money > 0) { // Each run is a new deal
        reshuffle(player, dealer, &deck);
        addHand(player);
        checkForTwoDealer(dealer);

        while (player->currentHand < player->numHands) { // Each run is a new decision (Hit, Stay, etc.)
            
            struct Hand* current = getCurrentHand(player);
            checkForTwoPlayer(player);

            int dealerValue = getHandValue(dealer->hand);
            int currentPlayerValue = getHandValue(current);
            if (dealerValue == 21 && currentPlayerValue == 21) { // Both got BJ
                // do bets and stuff
                // print
                printf("Press enter to continue...");
                scanf("");
                break;
            } else if (dealerValue == 21) { // Dealer got BJ
                // do bets and stuff
                // print
                printf("Press enter to continue...");
                scanf("");
                break;
            } else if (currentPlayerValue == 21) { // Player got BJ
                // do bets and stuff
                // print
                continue;
            }

            char *choices;
            int choices_len;
            if (canSplit(current)) {
                choices = split_choices;
                choices_len = 4;
            } else if (canDouble(current)) {
                choices = double_choices;
                choices_len = 3;
            } else {
                choices = default_choices;
                choices_len = 2;
            }


            char input;
            int invalid_input = 1;
            while (invalid_input) { // Each run is getting user's choice for this decision

                printf("Playing on hand %d of %d\n", player->currentHand + 1, player->numHands);
                printf("Dealer is showing a ");
                printCard(getDealerTopCard(dealer));

                printf("\nYou have: ");
                printHand(current);
                printf("for a total of %d\n", getHandValue(current));

                if (choices_len == 4) {
                    printf("1-Hit\t2-Stay\t3-Double\t4-Split\n");
                } else if (choices_len == 3) {
                    printf("1-Hit\t2-Stay\t3-Double\n");
                } else {
                    printf("1-Hit\t2-Stay\n");
                }                            

                scanf("%c", &input);

                int valid_choice = 0;
                for (int i = 0; i < choices_len; i++) {
                    if (input == choices[i]) {
                        valid_choice = 1;
                        break;
                    }
                }
                if (!valid_choice) {
                    system("cls");
                    printf("Invalid choice\n");
                    continue;
                }

                switch (input) {
                    case '4': // Split
                        invalid_input = 0;
                        addHand(player);
                        addCard(player->hands[player->currentHand + 1], removeLastCard(current));
                        checkForTwoPlayer(player);
                        break;
                    case '3': // Double down
                        invalid_input = 0;
                        addCard(current, draw(deck));
                        player->currentHand++;
                        current = getCurrentHand(player);
                        break;
                    case '2': // Stay
                        invalid_input = 0;
                        player->currentHand++;
                        current = getCurrentHand(player);
                        break;
                    case '1': // Hit
                        invalid_input = 0;
                        addCard(current, draw(deck));
                        break;
                }

            } // End of while (getting_input)
            
            system("cls");

            // Handle something here

        } // End of while (player->currentHand < player->numHands)
        
        // Handle something here

    } // End of while (player->money > 0)

    printf("Thank you for playing!\n");
    freeAll(player, dealer, deck);

    return 0;
}


// Preps the player, dealer, and deck for another deal
void reshuffle(struct Player* player, struct Dealer* dealer, struct Deck** deck) {
    nextRoundPlayer(player);
    nextRoundDealer(dealer);
    freeDeck(*deck);
    *deck = createDeck(MAXDECKS);
    shuffle(*deck);
    player->deck = *deck;
    dealer->deck = *deck;
}


// Frees all memory used
void freeAll(struct Player* player, struct Dealer* dealer, struct Deck* deck) {
    freePlayer(player);
    freeDealer(dealer);
    freeDeck(deck);
}
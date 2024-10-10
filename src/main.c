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

void pressEnterToContinue();
void reshuffle(struct Player* player, struct Dealer* dealer, struct Deck** deck);
void freeAll(struct Player* player, struct Dealer* dealer, struct Deck* deck);

int main() {
    //setlocale(LC_ALL, "");

    struct Deck* deck = NULL;// = createDeck(MAXDECKS);
    struct Player* player = createPlayer(deck);
    struct Dealer* dealer = createDealer(deck);

    player->money = 1000;

    printf("Starting...\n\n");
    //Sleep(2000);

    while (player->money > 0) { // Each run is a new deal
        reshuffle(player, dealer, &deck);
        addHand(player);
        addBet(player, BET);
        checkForTwoDealer(dealer);

        while (player->currentHand < player->numHands) { // Each run is a new decision (Hit, Stay, etc.)
            struct Hand* current = getCurrentHand(player);
            checkForTwoPlayer(player);

            int currentPlayerValue = getHandValue(current);
            int dealerValue = getHandValue(dealer->hand);
            
            if (isBlackjack(dealer->hand) && isBlackjack(current)) { // Both got BJ
                printf("The dealer got blackjack with ");
                printHand(dealer->hand);
                printf("\nBut so did you with ");
                printHand(current);
                printf("\nNo money payed.");
                push(player);
                player->currentHand++;
                break;
            } else if (isBlackjack(dealer->hand)) { // Dealer got BJ
                printf("The dealer got blackjack with ");
                printHand(dealer->hand);
                printf("\nYou lost %d\n", getCurrentBet(player));
                player->currentHand++;
                break;
            } else if (isBlackjack(current)) { // Player got BJ
                printf("Congratulations! You got blackjack with ");
                printHand(current);
                printf("\n");
                player->currentHand++;
                pressEnterToContinue();
                continue;
            } else if (getHandValue(current) == 21) {
                printf("You got 21 with ");
                printHand(current);
                printf("\n");
                player->currentHand++;
                pressEnterToContinue();
                continue;
            }

            // ALSO DETERMINE IF ENOUGH MONEY FOR SPLITTING AND DOUBLING
            
            char *choices;
            int choices_len;
            if (canSplit(current) && player->money >= getCurrentBet(player)) {
                choices = split_choices;
                choices_len = 4;
            } else if (canDouble(current) && player->money >= getCurrentBet(player)) {
                choices = double_choices;
                choices_len = 3;
            } else {
                choices = default_choices;
                choices_len = 2;
            }


            char input;
            int invalid_input = 1;
            while (invalid_input) { // Each run is getting user's choice for this decision

                int totalBets = 0;
                for (int i = 0; i < player->numHands; i++) {
                    totalBets += player->bets[i];
                }
                printf("Money - $%d\t Total Bets - $%d\n", player->money + totalBets, totalBets);
                printf("Playing on hand #%d of %d\n", player->currentHand + 1, player->numHands);
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
                        addBet(player, getCurrentBet(player));
                        addCard(player->hands[player->currentHand + 1], removeLastCard(current));
                        if (strcmp(peekHand(current)->value, "A") == 0) {
                            for (int i = 0; i < 2; i++) {
                                addCard(getCurrentHand(player), draw(deck));
                                player->currentHand++;
                            }
                        } else {
                            checkForTwoPlayer(player);
                        }
                        
                        break;
                    case '3': // Double down
                        invalid_input = 0;
                        addCard(current, draw(deck));
                        doubleBet(player);
                        if (playerBust(player)) {
                            printf("Sorry, you drew a ");
                            printCard(current->cards[current->numCards - 1]);
                            printf("giving you a total of %d. You bust!\nPress enter to continue...", getHandValue(current));
                            char c;
                            scanf("%c", &c);
                            while ((c = getchar()) != '\n' && c != EOF);
                        }
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
                        if (playerBust(player)) {
                            printf("Sorry, you drew a ");
                            printCard(current->cards[current->numCards - 1]);
                            printf("giving you a total of %d. You bust!\nPress enter to continue...", getHandValue(current));
                            char c;
                            scanf("%c", &c);
                            while ((c = getchar()) != '\n' && c != EOF);
                            player->currentHand++;
                            current = getCurrentHand(player);
                        }
                        break;
                }

            } // End of while (getting_input)
            
            system("cls");

        } // End of while (player->currentHand < player->numHands)
        

        if (isBlackjack(dealer->hand)) 
            continue;

        int dealerVal = getHandValue(dealer->hand);
        printf("The dealer has ");
        printHand(dealer->hand);
        printf("for a total of %d\n", dealerVal);
        Sleep(500);

        while (dealerVal < 17) {
            addCard(dealer->hand, draw(deck));

            dealerVal = getHandValue(dealer->hand);
            printf("The dealer has ");
            printHand(dealer->hand);
            printf("for a total of %d\n", dealerVal);
            Sleep(500);
        }

        
        for (int i = 0; i < player->numHands; i++) {
            player->currentHand = i;
            struct Hand* current = getCurrentHand(player);
            int currentVal = getHandValue(current); // current hand value
            printf("\nOn hand #%d, you had ", i+1);
            printHand(current);
            printf("for a total of %d\n", currentVal);
        
            if (playerBust(player)) {
                printf("This hand bust. You lost $%d\n", getCurrentBet(player));
            } else if (isBlackjack(current)) {
                printf("This hand had blackjack! You profit $%f\n", 1.5 * getCurrentBet(player));
                blackjackPayday(player);
            } else if (dealerBust(dealer) || (!playerBust(player) && currentVal > dealerVal)) { // Player's hand won
                printf("This hand won! You profit $%d\n", getCurrentBet(player));
                payday(player);
            } else if (playerBust(player) || (!dealerBust(dealer) && dealerVal > currentVal)) { // Dealer's hand won
                printf("The dealer won.. You lost $%d\n", getCurrentBet(player));
            } else { // Push
                printf("This hand tied. No money payed.\n");
                push(player);
            }
            Sleep(500);
        }

        printf("Your new balance is %d\n", player->money);
        pressEnterToContinue();
    } // End of while (player->money > 0)

    printf("Thank you for playing!\n");
    freeAll(player, dealer, deck);

    return 0;
}


void pressEnterToContinue() {
    printf("Press enter to continue...");
    char c;
    scanf("%c", &c);
    while ((c = getchar()) != '\n' && c != EOF);
    system("cls");
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
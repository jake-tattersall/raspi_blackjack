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

    struct Deck* deck = NULL;
    struct Player* player = createPlayer(deck); // The player
    struct Dealer* dealer = createDealer(deck); // The dealer

    player->money = 1000;

    printf("Starting...\n\n");
    //Sleep(2000);

    while (player->money > 0) { // Each run is a new deal

        // This is where the program would ask for a bet if I wanted to do it that way
        // If no code here, use the default BET everytime

        // Shuffle the deck, give the player a hand, and give the dealer 2 cards
        reshuffle(player, dealer, &deck);
        addHand(player);
        addBet(player, BET);
        checkForTwoDealer(dealer);

        int dealerValue = getHandValue(dealer->hand); // The dealer's hand value

        while (player->currentHand < player->numHands) { // Each run is a new decision (Hit, Stay, etc.)
            struct Hand* current = getCurrentHand(player); // The current hand
            
            // First, give the player 2 cards if they don't have 2 cards
            checkForTwoPlayer(player);

            int currentPlayerValue = getHandValue(current); // The current hand value
            
            // Check if 21 has been reached by either Dealer or Player
            if (isBlackjack(dealer->hand) && isBlackjack(current)) { // Both got BJ
                printf("The dealer got blackjack with ");
                printHand(dealer->hand);
                printf("\nBut so did you with ");
                printHand(current);
                printf("\nNo money payed.");

                // No money transfer for this round. Break to next deal
                push(player);
                player->currentHand++;
                break;
            } else if (isBlackjack(dealer->hand)) { // Dealer got BJ
                printf("The dealer got blackjack with ");
                printHand(dealer->hand);
                printf("\nYou lost %d.\n", getCurrentBet(player));

                // Break to next deal
                pressEnterToContinue();
                player->currentHand++;
                break;
            } else if (isBlackjack(current)) { // Player got BJ
                printf("Congratulations! You got blackjack with ");
                printHand(current);
                printf("\n");

                // Continue to next deal
                pressEnterToContinue();
                player->currentHand++;
                continue;
            } else if (getHandValue(current) == 21) { // 21 but not BJ
                printf("You got 21 with ");
                printHand(current);
                printf("\n");

                // Continue to next hand
                player->currentHand++;
                pressEnterToContinue();
                continue;
            }
            
            
            char *choices; // List of choices
            int choices_len; // Length of choices array

            // Determine what choices the player has 
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


            char input; // User input
            int invalid_input = 1; // 1 if needs new input, 0 if can move on
            while (invalid_input) { // Each run is getting user's choice for this decision
                // Get total money bet
                int totalBets = 0;
                for (int i = 0; i < player->numHands; i++) {
                    totalBets += player->bets[i];
                }
                // Display info to user
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

                // Get user's choice

                scanf("%c", &input); 
                char c;
                while ((c=getchar()) != '\n' && c != EOF);

                // Check if valid choice
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

                // If valid choice, determine choice
                switch (input) {
                    case '4': // Split 
                        // Move 2nd card to new hand
                        addHand(player);
                        addBet(player, getCurrentBet(player));
                        addCard(player->hands[player->currentHand + 1], removeLastCard(current));
                        // If splitting Aces, draw one card on each and end
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
                        // Draw one card and move to next hand
                        addCard(current, draw(deck));
                        doubleBet(player);
                        // If busted, tell player
                        if (playerBust(player)) {
                            printf("Sorry, you drew a ");
                            printCard(current->cards[current->numCards - 1]);
                            printf("giving you a total of %d. You bust!\n", getHandValue(current));
                            
                            pressEnterToContinue();
                        }
                        player->currentHand++;
                        current = getCurrentHand(player);
                        break;
                    case '2': // Stay
                        // Move to next hand
                        player->currentHand++;
                        current = getCurrentHand(player);
                        break;
                    case '1': // Hit
                        addCard(current, draw(deck));
                        // If busted, tell player
                        if (playerBust(player)) {
                            printf("Sorry, you drew a ");
                            printCard(current->cards[current->numCards - 1]);
                            printf("giving you a total of %d. You bust!\n", getHandValue(current));
                            pressEnterToContinue();
                            player->currentHand++;
                            current = getCurrentHand(player);
                        }
                        break;
                }

                // Exit while loop
                invalid_input = 0; 

            } // End of while (getting_input)
            
            // Clear console
            system("cls");

        } // End of while (player->currentHand < player->numHands)

        // If the dealer or player had BJ, don't process bets again
        if (isBlackjack(dealer->hand))
            continue;
        
        int dealerVal = getHandValue(dealer->hand);
        printf("The dealer has ");
        printHand(dealer->hand);
        printf("for a total of %d\n", dealerVal);
        Sleep(500);

        // Dealer stays on soft 17
        while (dealerVal < 17) {
            addCard(dealer->hand, draw(deck));

            dealerVal = getHandValue(dealer->hand);
            printf("The dealer has ");
            printHand(dealer->hand);
            printf("for a total of %d\n", dealerVal);
            Sleep(500);
        }

        // Process bets for each hand
        for (int i = 0; i < player->numHands; i++) {
            player->currentHand = i;
            struct Hand* current = getCurrentHand(player);
            int currentVal = getHandValue(current); // current hand value
            printf("\nOn hand #%d, you had ", i+1);
            printHand(current);
            printf("for a total of %d\n", currentVal);
        
            if (playerBust(player)) { // If hand busted
                printf("This hand bust. You lost $%d\n", getCurrentBet(player));
            } else if (isBlackjack(current)) { // If had was BJ
                printf("This hand had blackjack! You profit $%.0f\n", 1.5 * getCurrentBet(player));
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

        // Show player's new balance after all hands cleared
        printf("Your new balance is %d\n", player->money);
        pressEnterToContinue();
    } // End of while (player->money > 0)

    // Player has no money :(
    printf("Thank you for playing!\n");
    freeAll(player, dealer, deck);

    return 0;
}

// Asks the user to press enter, then clears the buffer
void pressEnterToContinue() {
    printf("Press enter to continue...");
    char c;
    scanf("%c", &c);
    while ((c = getchar()) != '\n' && c != EOF);
    system("cls");
}


// Preps the player, dealer, and deck for another deal
void reshuffle(struct Player* player, struct Dealer* dealer, struct Deck** deck) {
    nextDealPlayer(player);
    nextDealDealer(dealer);
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <locale.h>

#include "deck.h"
#include "player.h"
#include "dealer.h"
#include "lcd_display.h"

#define MAXDECKS 4 // Number of card decks in the shoe
#define BET 1 // Fixed Bet 1 credit per hand

void reshuffle(struct Player* player, struct Dealer* dealer, struct Deck** deck);
void freeAll(struct Player* player, struct Dealer* dealer, struct Deck* deck);
void int_to_str(int num);

char send[10];

int main() {
    // Setup hardware
    setup();

    //setlocale(LC_ALL, "");

    struct Deck* deck = NULL; // The shoe
    struct Player* player = createPlayer(deck); // The player
    struct Dealer* dealer = createDealer(deck); // The dealer

    player->money = BET * 10;

    lcd_string("Starting...");
    usleep(2e6);

    while (player->money > 0) { // Each run is a new deal

        // This is where the program would ask for a bet if I wanted to do it that way
        // Instead, uses the default BET everytime

        // Shuffle the deck, give the player a hand, and give the dealer 2 cards
        reshuffle(player, dealer, &deck);
        addHand(player);
        addBet(player, BET);
        checkForTwoDealer(dealer);

        while (player->currentHand < player->numHands) { // Each run is a new decision (Hit, Stay, etc.)
            struct Hand* current = getCurrentHand(player); // The current hand
            
            // First, give the player 2 cards if they don't have 2 cards
            checkForTwoPlayer(player);
            
            // Check if 21 has been reached by either Dealer or Player
            if (isBlackjack(dealer->hand) && isBlackjack(current)) { // Both got BJ
                /*
                printf("The dealer got blackjack with ");
                printHand(dealer->hand);
                printf("\nBut so did you with ");
                printHand(current);
                printf("\nNo money payed.");
                */

                lcd_clear();
                lcd_string("D-BJ & P-BJ");
                lcd_set_cursor(2, 0);
                lcd_string("Push");

                // No money transfer for this round. Break to next deal
                push(player);
                player->currentHand++;
                break;
            } else if (isBlackjack(dealer->hand)) { // Dealer got BJ
                /*
                printf("The dealer got blackjack with ");
                printHand(dealer->hand);
                printf("\nYou lost %d.\n", getCurrentBet(player));
                */
                lcd_clear();
                lcd_string("D-BJ");
                lcd_set_cursor(2, 0);
                int_to_str(getCurrentBet(player));
                lcd_string("-$");
                lcd_string(send);

                // Break to next deal
                lcd_press_enter();
                player->currentHand++;
                break;
            } else if (isBlackjack(current)) { // Player got BJ
                /*
                printf("Congratulations! You got blackjack with ");
                printHand(current);
                printf("\n");
                */
                lcd_clear();
                lcd_string("P-BJ ");
                lcd_print_hand(current);
                

                // Continue to next deal
                lcd_press_enter();
                player->currentHand++;
                continue;
            } else if (getHandValue(current) == 21) { // 21 but not BJ
                /*
                printf("You got 21 with ");
                printHand(current);
                printf("\n");
                */
                lcd_clear();
                lcd_string("21=>");
                lcd_print_hand(current);

                // Continue to next hand
                lcd_press_enter();
                player->currentHand++;
                continue;
            }
            
            int choices_len; // Length of choices array

            // Determine what choices the player has 
            if (canSplit(current) && player->money >= getCurrentBet(player)) {
                choices_len = 4;
            } else if (canDouble(current) && player->money >= getCurrentBet(player)) {
                choices_len = 3;
            } else {
                choices_len = 2;
            }


            int invalid_input = 1; // 1 if needs new input, 0 if can move on
            while (invalid_input) { // Each run is getting user's choice for this decision
                // Get total money bet
                int totalBets = 0;
                for (int i = 0; i < player->numHands; i++) {
                    totalBets += player->bets[i];
                }
                // Display info to user
                /*
                printf("Money - $%d\t Total Bets - $%d\n", player->money + totalBets, totalBets);
                printf("Playing on hand #%d of %d\n", player->currentHand + 1, player->numHands);
                printf("Dealer is showing a ");
                printCard(getDealerTopCard(dealer));
                */
                // Print money
                lcd_clear();
                lcd_string("M$");
                int_to_str(player->money + totalBets);
                lcd_string(send);
                // Print bets
                lcd_string(" T$");
                int_to_str(totalBets);
                lcd_string(send);
                // Print dealer's card
                lcd_string(" D=");
                lcd_print_card(getDealerTopCard(dealer));

                lcd_set_cursor(2, 0);

                /*
                printf("\nYou have: ");
                printHand(current);
                printf("for a total of %d\n", getHandValue(current));
                */
                lcd_print_hand(current);
                lcd_string("=");
                int_to_str(getHandValue(current));
                lcd_string(send);

                /*
                if (choices_len == 4) {
                    printf("1-Hit\t2-Stay\t3-Double\t4-Split\n");
                } else if (choices_len == 3) {
                    printf("1-Hit\t2-Stay\t3-Double\n");
                } else {
                    printf("1-Hit\t2-Stay\n");
                }           
                */                 

                // Get user's choice

                /*
                scanf("%c", &input); 
                char c;
                while ((c=getchar()) != '\n' && c != EOF);
                */

                // display pot val and wait for button in that func
                int input = wait_for_button(choices_len);

                // Check if valid choice
                /*
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
                */

                lcd_clear();
                // If valid choice, determine choice
                switch (input) {
                    case 4: // Split 
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
                    case 3: // Double down
                        // Draw one card and move to next hand
                        addCard(current, draw(deck));
                        doubleBet(player);
                        // If busted, tell player
                        if (playerBust(player)) {
                            /*
                            printf("Sorry, you drew a ");
                            printCard(current->cards[current->numCards - 1]);
                            printf("giving you a total of %d. You bust!\n", getHandValue(current));
                            */

                            lcd_string("Busted w/ ");
                            lcd_print_card(current->cards[current->numCards - 1]);
                            lcd_set_cursor(2, 0);
                            lcd_string("==>");
                            int_to_str(getHandValue(current));
                            lcd_string(send);
                            
                            lcd_press_enter();
                        }
                        player->currentHand++;
                        current = getCurrentHand(player);
                        break;
                    case 2: // Stay
                        // Move to next hand
                        player->currentHand++;
                        current = getCurrentHand(player);
                        break;
                    case 1: // Hit
                        addCard(current, draw(deck));
                        // If busted, tell player
                        if (playerBust(player)) {
                            /*
                            printf("Sorry, you drew a ");
                            printCard(current->cards[current->numCards - 1]);
                            printf("giving you a total of %d. You bust!\n", getHandValue(current));
                            */

                            lcd_string("Busted w/ ");
                            lcd_print_card(current->cards[current->numCards - 1]);
                            lcd_set_cursor(2, 0);
                            lcd_string("==>");
                            int_to_str(getHandValue(current));
                            lcd_string(send);
                            
                            lcd_press_enter();

                            player->currentHand++;
                            current = getCurrentHand(player);
                        }
                        break;
                }

                // Exit while loop
                invalid_input = 0; 

            } // End of while (getting_input)
            
            // Clear console
            //system("cls");
            lcd_clear();
            usleep(5e5);

        } // End of while (player->currentHand < player->numHands)

        // If the dealer or player had BJ, don't process bets again
        if (isBlackjack(dealer->hand))
            continue;
        
        int dealerVal = getHandValue(dealer->hand);
        /*
        printf("The dealer has ");
        printHand(dealer->hand);
        printf("for a total of %d\n", dealerVal);
        */
        //Sleep(500);

        lcd_clear();
        lcd_string("Dealer has");
        lcd_set_cursor(2, 0);
        lcd_print_hand(dealer->hand);
        usleep(5e5);

        // Dealer stays on soft 17
        while (dealerVal < 17) {
            addCard(dealer->hand, draw(deck));

            dealerVal = getHandValue(dealer->hand);
            /*
            printf("The dealer has ");
            printHand(dealer->hand);
            printf("for a total of %d\n", dealerVal);
            //Sleep(500);
            */
            lcd_clear();
            lcd_string("Dealer has");
            lcd_set_cursor(2, 0);
            lcd_print_hand(dealer->hand);
            usleep(5e5);
        }

        // Process bets for each hand
        for (int i = 0; i < player->numHands; i++) {
            lcd_clear();

            player->currentHand = i;
            struct Hand* current = getCurrentHand(player);
            int currentVal = getHandValue(current); // current hand value
            // printf("\nOn hand #%d, you had ", i+1);
            // printHand(current);
            // printf("for a total of %d\n", currentVal);
            
            int_to_str(i+1);
            lcd_string(send);
            lcd_string("=>");
            lcd_print_hand(current);
            lcd_set_cursor(2, 0);

        
            if (playerBust(player)) { // If hand busted
                //printf("This hand bust. You lost $%d\n", getCurrentBet(player));
                lcd_string(" bust -$");
                int_to_str(getCurrentBet(player));
                lcd_string(send);
            } else if (isBlackjack(current)) { // If had was BJ
                //printf("This hand had blackjack! You profit $%.0f\n", 1.5 * getCurrentBet(player));
                lcd_string(" P-BJ! +$");
                int_to_str(getCurrentBet(player));
                lcd_string(send);
                blackjackPayday(player);
            } else if (dealerBust(dealer) || (!playerBust(player) && currentVal > dealerVal)) { // Player's hand won
                //printf("This hand won! You profit $%d\n", getCurrentBet(player));
                lcd_string(" won! +$");
                int_to_str(getCurrentBet(player));
                lcd_string(send);
                payday(player);
            } else if (playerBust(player) || (!dealerBust(dealer) && dealerVal > currentVal)) { // Dealer's hand won
                //printf("The dealer won.. You lost $%d\n", getCurrentBet(player));
                lcd_string(" lost. -$");
                int_to_str(getCurrentBet(player));
                lcd_string(send);
            } else { // Push
                //printf("This hand tied. No money payed.\n");
                lcd_string(" pushed. +$0");
                push(player);
            }
            //Sleep(500);
            usleep(1e6);
        }

        // Show player's new balance after all hands cleared
        //printf("Your new balance is %d\n", player->money);
        lcd_clear();
        int_to_str(player->money);
        lcd_string("Bal: ");
        lcd_string(send);
        lcd_press_enter();
    } // End of while (player->money > 0)

    // Player has no money :(
    //printf("Thank you for playing!\n");
    lcd_clear();
    lcd_string("Thanks for");
    lcd_set_cursor(2, 0);
    lcd_string("Playing!");
    freeAll(player, dealer, deck);

    return 0;
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

void int_to_str(int num) {
    sprintf(send, "%d", num);
}

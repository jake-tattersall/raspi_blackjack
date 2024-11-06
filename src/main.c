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

const char* path = "/home/jaket/Desktop/raspi_blackjack-main/src/data.txt"; // Path to data file

void reshuffle(struct Player* player, struct Dealer* dealer, struct Deck** deck);
void freeAll(struct Player* player, struct Dealer* dealer, struct Deck* deck);
void int_to_str(int num);

char send[10]; // variable used when writing to screen

int main() {
    // Setup hardware. End whole program if failed
    if (setup() == -1) {
        return 1;
    }

    //setlocale(LC_ALL, "");

    struct Deck* deck = NULL; // The shoe
    struct Player* player = createPlayer(deck); // The player
    struct Dealer* dealer = createDealer(deck); // The dealer

    
    // Get data from data.txt
    FILE *fp = fopen(path, "r"); // The data file
    if (fp == NULL) { // File DNE, so make it and set money
        fp = fopen(path, "w");
        player->money = BET * 10;
        fprintf(fp, "%d\n", player->money);
    } else {
        fscanf(fp, "%d", &player->money);
        if (player->money <= 0) { // No money, so set money
            fclose(fp);
            fp = fopen(path, "w");
            player->money = BET * 10;
            fprintf(fp, "%d\n", player->money);
        }
    }
    fclose(fp);
    

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
                lcd_clear();
                lcd_string("D-BJ & P-BJ");
                lcd_set_cursor(2, 0);
                lcd_string("Push");

                // No money transfer for this round. Break to next deal
                push(player);
                player->currentHand++;
                break;
            } else if (isBlackjack(dealer->hand)) { // Dealer got BJ
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
                lcd_clear();
                lcd_string("P-BJ ");
                lcd_print_hand(current);
                

                // Continue to next deal
                lcd_press_enter();
                player->currentHand++;
                continue;
            } else if (getHandValue(current) == 21) { // 21 but not BJ
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

                lcd_print_hand(current);
                lcd_string("=");
                int_to_str(getHandValue(current));
                lcd_string(send);


                // Display current selection and wait for button press to confirm selection
                int input = wait_for_button(choices_len);

    
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
            
            lcd_clear();
            usleep(5e5);

        } // End of while (player->currentHand < player->numHands)

        // If the dealer or player had BJ, don't process bets again
        if (isBlackjack(dealer->hand))
            continue;
        
        int dealerVal = getHandValue(dealer->hand);

        lcd_clear();
        lcd_string("Dealer has");
        lcd_set_cursor(2, 0);
        lcd_print_hand(dealer->hand);
        usleep(5e5);

        // Dealer stays on soft 17
        while (dealerVal < 17) {
            addCard(dealer->hand, draw(deck));

            dealerVal = getHandValue(dealer->hand);

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
            
            int_to_str(i+1);
            lcd_string(send);
            lcd_string("=>");
            lcd_print_hand(current);
            lcd_set_cursor(2, 0);

        
            if (playerBust(player)) { // If hand busted
                lcd_string(" bust -$");
                int_to_str(getCurrentBet(player));
                lcd_string(send);
            } else if (isBlackjack(current)) { // If had was BJ
                lcd_string(" P-BJ! +$");
                int_to_str(getCurrentBet(player));
                lcd_string(send);
                blackjackPayday(player);
            } else if (dealerBust(dealer) || (!playerBust(player) && currentVal > dealerVal)) { // Player's hand won
                lcd_string(" won! +$");
                int_to_str(getCurrentBet(player));
                lcd_string(send);
                payday(player);
            } else if (playerBust(player) || (!dealerBust(dealer) && dealerVal > currentVal)) { // Dealer's hand won
                lcd_string(" lost. -$");
                int_to_str(getCurrentBet(player));
                lcd_string(send);
            } else { // Push
                lcd_string(" pushed. +$0");
                push(player);
            }
            usleep(1e6);
        }

        // Show player's new balance after all hands cleared
        fp = fopen(path, "w");
        fprintf(fp, "%d\n", player->money);
        fclose(fp);

        lcd_clear();
        int_to_str(player->money);
        lcd_string("Bal: ");
        lcd_string(send);
        lcd_press_enter();
    } // End of while (player->money > 0)

    // Player has no money :(
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

// Convert an integer to a lcd-sendable string
void int_to_str(int num) {
    sprintf(send, "%d", num);
}

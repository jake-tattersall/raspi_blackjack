#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "card.h"
#include "hand.h"

void setup();

void lcd_init();
void lcd_set_cursor(int line, int pos);
void lcd_string(const char *message);
void lcd_clear();

int read_adc();

void lcd_press_enter();
void lcd_print_card(struct Card *c);
void lcd_print_hand(struct Hand *h);

int wait_for_button(int choices_len);

#endif

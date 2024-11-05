#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#include "card.h"
#include "hand.h"

#define LCD_ADDR 0x3F
#define LCD_CHR 1
#define LCD_CMD 0
#define LCD_BACKLIGHT 0x08
#define ENABLE 0b00000100

#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_MAX_LINE_LEN 16

#define ADC_ADDR 0x4B
#define CHANNEL 0

void setup();

void lcd_init();
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);
void lcd_string(const char *message);
void lcd_set_cursor(int line, int pos);
void lcd_delay();
void lcd_clear();

int read_adc();

void lcd_press_enter();
void lcd_print_card(struct Card *c);
void lcd_print_hand(struct Hand *h);

#endif
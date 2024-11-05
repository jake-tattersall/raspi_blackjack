#include <stdio.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <unistd.h>

#include "lcd_display.h"

#include "card.h"
#include "dealer.h"
#include "deck.h"
#include "hand.h"
#include "player.h"


int lcd;
int adc;

int cursor_pos;
int current_line;



/*
int main() {
	
	 = wiringPiI2CSetup(LCD_ADDR);
	if (lcd == -1) {
		printf("bad lcd\n");
		return 1;
	}
	
	int adc = wiringPiI2CSetup(ADC_ADDR);
	if (adc == -1) {
		printf("bad adc\n");
		return 1;
	}
	
	lcd_init(lcd);
	
	printf("hello world\n");
	
	lcd_string(lcd, "Hello, World!");
	
	sleep(1);
	
	lcd_set_cursor(lcd, 2, 0);
	
	lcd_string(lcd, "test");
	
	while (1) {
		int pot_val = read_adc(adc);
		if (pot_val >= 0) {
			printf("%d\n", pot_val);
		}
	}
	
	
	return 0;
}
*/

void setup() {
    lcd = wiringPiI2CSetup(LCD_ADDR);
	if (lcd == -1) {
		printf("bad lcd\n");
		return 1;
	}
	
	adc = wiringPiI2CSetup(ADC_ADDR);
	if (adc == -1) {
		printf("bad adc\n");
		return 1;
	}
}


void lcd_init() {
    cursor_pos = 0;
    current_line = 1;

	lcd_byte(lcd, 0x33, LCD_CMD); // init
	lcd_byte(lcd, 0x32, LCD_CMD); // 4-bit mode
	lcd_byte(lcd, 0x06, LCD_CMD); // Cursor move direction
	lcd_byte(lcd, 0x0C, LCD_CMD); // Turn on display
	lcd_byte(lcd, 0x28, LCD_CMD); // 2 line display, 5x8 matrix
	lcd_byte(lcd, LCD_CLEAR, LCD_CMD); // Clear display
	lcd_byte(lcd, LCD_HOME, LCD_CMD); // Return home
	sleep(1);
}

void lcd_byte(int bits, int mode) {
	int bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT;
	int bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;
	
	wiringPiI2CWrite(lcd, bits_high);
	lcd_toggle_enable(lcd, bits_high);
	lcd_delay();
	
	wiringPiI2CWrite(lcd, bits_low);
	lcd_toggle_enable(lcd, bits_low);
	lcd_delay();
}


void lcd_toggle_enable(int bits) {
	delayMicroseconds(500);
	wiringPiI2CWrite(lcd, (bits | ENABLE));
	delayMicroseconds(500);
	wiringPiI2CWrite(lcd, (bits & ~ENABLE));
	delayMicroseconds(500);
}

void lcd_string(const char *message) {
	while (*message) {
	
		if (cursor_pos >= LCD_MAX_LINE_LEN) {
			if (current_line < 2) {
				current_line++;
				lcd_set_cursor(lcd, current_line, 0);
			} else 
				break;
		}
		
		lcd_byte(lcd, *message++, LCD_CHR);
		cursor_pos++;
	}
}

void lcd_set_cursor(int line, int pos) {
	int address;
	
	if (line == 1) {
		address = 0x80 + pos;
	} else if (line == 2) {
		address = 0xC0 + pos;
	} else {
		return;
	}
	
	lcd_byte(lcd, address, LCD_CMD);
	cursor_pos = pos;
	current_line = line;
}


void lcd_delay() {
	usleep(500);
}

void lcd_clear() {
    lcd_byte(lcd, LCD_CLEAR, LCD_CMD); 
    lcd_set_cursor(lcd, 1, 0);
}


int read_adc() {
	wiringPiI2CWrite(adc, CHANNEL);
	
	int adc_val = wiringPiI2CRead(adc);
	if (adc_val == -1) {
		printf("bad adc_val\n");
		return -1;
	}
	
	adc_val &= 0x0FFF;
	return adc_val;
}


void lcd_press_enter() {
    lcd_set_cursor(lcd, 2, 14);
    lcd_string(lcd, "PE");
}


void lcd_print_card(struct Card *c) {
    lcd_string(lcd, c->suit);
    lcd_string(lcd, c->value);
    lcd_string(lcd, " ");
}


void lcd_print_hand(struct Hand *h) {
    for (int i = 0; i < h->numCards; i++)
        lcd_print_card(lcd, h->cards[i]);
}



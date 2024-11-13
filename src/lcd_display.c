#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <unistd.h>

#include "lcd_display.h"

#include "card.h"
#include "dealer.h"
#include "deck.h"
#include "hand.h"
#include "player.h"

#define BUTTON 25                           // Button GPIO pin
#define DEBOUNCE_DELAY 50                   // Delay for debouncing

#define LCD_ADDR 0x3F                       // I2C Address of LCD
#define LCD_CHR 1                           // Command to print char
#define LCD_CMD 0                           // Value used to tell to perform command
#define LCD_BACKLIGHT 0x08                  // Turns on backlight
#define ENABLE 0b00000100                   // Used to turn on required bits for each char

#define LCD_CLEAR 0x01                      // CMD to clear the screen
//#define LCD_HOME 0x02                     // CMD to return home
#define LCD_MAX_LINE_LEN 16                 // Max chars per line

#define ADC_ADDR 0x4B                       // I2C Address of ADS7830
#define CHANNEL 0                           // Used to write to ADC

#define HEART_LOC 0                         // LCD storage of heart symbol
#define DIAMOND_LOC 1                       // LCD storage of diamond symbol
#define CLUB_LOC 2                          // LCD storage of club symbol
#define SPADE_LOC 3                         // LCD storage of spade symbol

void lcd_init();
void lcd_byte(int bits, int mode);
void lcd_toggle_enable(int bits);
void lcd_delay();
void lcd_display_action(char* action);
int read_adc();
int debounce_button();
void lcd_load_custom_char(int location, const unsigned char *char_map);

const unsigned char heart[8] = {            // LCD grid for the heart
    0b00000,
    0b01010,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000
};

const unsigned char diamond[8] = {          // LCD grid for the diamond
    0b00000,
    0b00100,
    0b01110,
    0b11111,
    0b01110,
    0b00100,
    0b00000,
    0b00000
};

const unsigned char club[8] = {             // LCD grid for the club
    0b00100,
    0b01110,
    0b00100,
    0b01110,
    0b11111,
    0b00100,
    0b00100,
    0b00000
};

const unsigned char spade[8] = {            // LCD grid for the spade
    0b00100,
    0b01110,
    0b11111,
    0b11111,
    0b01110,
    0b00100,
    0b11111,
    0b00000
};


int lcd;                                    // The LCD
int adc;                                    // The ADS7830

int cursor_pos;                             // Current cursor position (0-15)
int current_line;                           // Current line # (1 or 2)
char* previous_action;                      // Previous H, S, D, or V


// Setup all hardware.
// Button, LCD, and ADC
// Return -1 if bad, 0 otherwise
int setup() {
    wiringPiSetup();
    pinMode(BUTTON, INPUT);
    pullUpDnControl(BUTTON, PUD_DOWN);

    lcd = wiringPiI2CSetup(LCD_ADDR);
	if (lcd == -1) {
		printf("bad lcd\n");
		return -1;
	}
	
	adc = wiringPiI2CSetup(ADC_ADDR);
	if (adc == -1) {
		printf("bad adc\n");
		return -1;
	}

	lcd_init();

    previous_action = "";
    return 0;
}

// Initializes the lcd
void lcd_init() {
    cursor_pos = 0;
    current_line = 1;

	lcd_byte(0x33, LCD_CMD); // init
	lcd_byte(0x32, LCD_CMD); // 4-bit mode
	lcd_byte(0x06, LCD_CMD); // Cursor move direction
	lcd_byte(0x0C, LCD_CMD); // Turn on display
	lcd_byte(0x28, LCD_CMD); // 2 line display, 5x8 matrix
	//lcd_byte(LCD_CLEAR, LCD_CMD); // Clear display
	//lcd_byte(LCD_HOME, LCD_CMD); // Return home
	lcd_clear();

    // Load all custom chars
	lcd_load_custom_char(HEART_LOC, heart);
    lcd_load_custom_char(DIAMOND_LOC, diamond);
    lcd_load_custom_char(CLUB_LOC, club);
    lcd_load_custom_char(SPADE_LOC, spade);

	sleep(1);
}

// Write byte to LCD
void lcd_byte(int bits, int mode) {
	int bits_high = mode | (bits & 0xF0) | LCD_BACKLIGHT;
	int bits_low = mode | ((bits << 4) & 0xF0) | LCD_BACKLIGHT;
	
	wiringPiI2CWrite(lcd, bits_high);
	lcd_toggle_enable(bits_high);
	lcd_delay();
	
	wiringPiI2CWrite(lcd, bits_low);
	lcd_toggle_enable(bits_low);
	lcd_delay();
}

// Enable selected bits on LCD
void lcd_toggle_enable(int bits) {
	delayMicroseconds(500);
	wiringPiI2CWrite(lcd, (bits | ENABLE));
	delayMicroseconds(500);
	wiringPiI2CWrite(lcd, (bits & ~ENABLE));
	delayMicroseconds(500);
}

// Sends the string to the LCD screen
void lcd_string(const char *message) {
	while (*message) {
	
		if (cursor_pos >= LCD_MAX_LINE_LEN) {
			if (current_line < 2) {
				current_line++;
				lcd_set_cursor(current_line, 0);
			} else 
				break;
		}
		
		lcd_byte(*message++, LCD_CHR);
		cursor_pos++;
	}
}

// Sets the cursor at the specified line and pos
// Line is from 1-2
// Pos is from 0-15
void lcd_set_cursor(int line, int pos) {
	int address;
	
	if (line == 1) {
		address = 0x80 + pos;
	} else if (line == 2) {
		address = 0xC0 + pos;
	} else {
		return;
	}
	
	lcd_byte(address, LCD_CMD);
	cursor_pos = pos;
	current_line = line;
}

// Slight delay for the LCD
void lcd_delay() {
	usleep(500);
}

// Clear LCD screen and reset position
void lcd_clear() {
    lcd_byte(LCD_CLEAR, LCD_CMD); 
    lcd_set_cursor(1, 0);
    usleep(2000);
}

// Read the value of the ADC
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

// Waits for the user to press the button
void lcd_press_enter() {
    lcd_set_cursor(2, 14);
    lcd_string("PE");
    wait_for_button(0);
}

// Prints the specified card
void lcd_print_card(struct Card *c) {
	switch (c->suit[0]) {
		case 'H':
			lcd_byte(HEART_LOC, LCD_CHR);
			break;
		case 'D':
			lcd_byte(DIAMOND_LOC, LCD_CHR);
			break;
		case 'C':
			lcd_byte(CLUB_LOC, LCD_CHR);
			break;
		case 'S':
			lcd_byte(SPADE_LOC, LCD_CHR);
			break;
	}
    lcd_string(c->value);
    lcd_string(" ");
}

// Prints the specified hand, each card at a time
void lcd_print_hand(struct Hand *h) {
    for (int i = 0; i < h->numCards; i++)
        lcd_print_card(h->cards[i]);
}

// Displays the user's current action selection
void lcd_display_action(char* action) {
    // Useless to continue if already displaying
    if (strcmp(action, previous_action) == 0) return;

    lcd_set_cursor(2, 15);
    lcd_string(action);
    previous_action = action;
}

// Debounces the button.
// Returns HIGH or LOW if proper press
// Returns -1 if invalid press
int debounce_button() {
    int stable_state = digitalRead(BUTTON); // Initial reading
    delay(DEBOUNCE_DELAY); // Wait for debounce period
    int current_state = digitalRead(BUTTON); // Read again

    // If the state is stable, return it
    if (current_state == stable_state) {
        return stable_state;
    }
    return -1; 
}

// Waits for the button to be pressed
// In the meantime, shows the user their current selection of H, S, D, or V
int wait_for_button(int choices_len) {
    while (1) {
        int val = read_adc();
        if (choices_len <= 0) {
			val = -1;
        } else if (choices_len == 4) {
            val /= 64;
        } else if (choices_len == 3) {
            val /= 86;
        } else {
            val /= 128;
        }
        val++;

        switch (val) {
			case 0:
				break;
            case 1:
                lcd_display_action("H");
                break;
            case 2:
                lcd_display_action("S");
                break;
            case 3:
                lcd_display_action("D");
                break;
            case 4:
                lcd_display_action("V");
                break;
        }

        int button_state = debounce_button();
        if (button_state == HIGH) {
			previous_action = "";
            return val;
        }
    }
}

// Loads a custom map to the LCD at the specified location
void lcd_load_custom_char(int location, const unsigned char *char_map) {
    // Limit location to 0-7
    location &= 0x07;

    // Set CGRAM address
    lcd_byte(0x40 | (location << 3), LCD_CMD);

    // Write 8 bytes to define the character
    for (int i = 0; i < 8; i++) {
        lcd_byte(char_map[i], LCD_CHR);
    }
}

#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <pcf8574.h>
#include <lcd.h>

#include "lcd_display.h"

#include "card.h"
#include "dealer.h"
#include "deck.h"
#include "hand.h"
#include "player.h"


unsigned char command = 0x84; // Read channel 0

int lcdAdsSetup() {
    lcd = lcdInit(2, 16, 4, I2C_ADDRESS, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    ads = wiringPiI2CSetup(ADS7830_ADDRESS);

    // If either was unsuccessful at being initialized, end
    if (lcd == -1 || ads == -1)
        return -1;

    pinMode(BUTTON, INPUT);

    lcdClear(lcd);
}


void lcdDisplay(struct Player* p, struct Dealer* dealer) {
    struct Card* top = getDealerTopCard(dealer);
    lcdPrintf(lcd, "$%d   %s%s   %d", p->money, top->suit, top->value, wiringPiI2CReadReg8(ads, command));


    
}

// See tutorial.pdf for debounce logic using millis

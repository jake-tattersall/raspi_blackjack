#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H

#define I2C_ADDRESS 0x27
#define ADS7830_ADDRESS 0x48 // or 0x4b
#define BUTTON 26

int lcd;
int ads;

int lcdAdsSetup();
void lcdDisplay(struct Player* p, struct Dealer* dealer);

#endif
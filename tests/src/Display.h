#ifndef DISPLAY_H
#define DISPLAY_H

#include <U8g2lib.h>

// Display constants
#define OLED_RESET -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define LINE_HEIGHT 8
#define H_PADDING 2

// U8G2 display object
extern U8G2_SSD1306_128X64_NONAME_F_SW_I2C display;

// Functions for display handling
void initializeDisplay();
void splitTextIntoLines(String *texts, int numTexts, int charPerLine, String *lines, int &lineCount);
void scrollAllText(String *texts, int numTexts, int delayMs);

#endif // DISPLAY_H

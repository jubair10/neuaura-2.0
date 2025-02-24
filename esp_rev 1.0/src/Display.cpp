#include "Display.h"

// Initialize the display object
U8G2_SSD1306_128X64_NONAME_F_SW_I2C display(U8G2_R0, /* clock=*/SCL, /* data=*/SDA, OLED_RESET);

void initializeDisplay()
{
    display.begin();                         // Initialize display library
    display.setFont(u8g2_font_profont12_tr); // Set font size
    display.clearBuffer();
    display.println("Initializing...");
    display.sendBuffer(); // Send data to OLED screen
    delay(1000);
}

void splitTextIntoLines(String *texts, int numTexts, int charPerLine, String *lines, int &lineCount)
{
    lineCount = 0;

    for (int i = 0; i < numTexts; i++)
    {
        String currentText = texts[i];
        int textLength = currentText.length();

        for (int j = 0; j < textLength; j += charPerLine)
        {
            lines[lineCount++] = currentText.substring(j, min(j + charPerLine, textLength));
        }

        lines[lineCount++] = ""; // Add an empty line for spacing
    }
}

void scrollAllText(String *texts, int numTexts, int delayMs)
{
    int charPerLine = (SCREEN_WIDTH - 2 * H_PADDING) / 6; // Approx characters per line (accounting for padding)
    String lines[200];                                    // Array to store lines (adjust size if needed)
    int totalLines = 0;
    splitTextIntoLines(texts, numTexts, charPerLine, lines, totalLines);

    int visibleLines = SCREEN_HEIGHT / LINE_HEIGHT; // Lines that fit on screen

    // Scroll through all lines
    for (int startLine = 0; startLine < totalLines + visibleLines; startLine++)
    {
        display.clearBuffer();

        // Display visible lines
        for (int line = 0; line < visibleLines; line++)
        {
            int currentLine = startLine + line - visibleLines;
            if (currentLine >= 0 && currentLine < totalLines)
            {
                display.setCursor(H_PADDING, LINE_HEIGHT * (line));
                display.print(lines[currentLine]);
            }
        }
        display.sendBuffer(); 
        delay(delayMs);       
    }
}

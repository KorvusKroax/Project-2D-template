#pragma once

#include "lodepng.h"

#include <iostream> // cout

struct Font
{
    unsigned int charWidth, charHeight, charCount = 0;
    int **charset = nullptr;

    Font() { }

    Font(const char *fileName, int fontSheetGridColumns, int fontSheetGridRows, int charWidth, int charHeight)
    {
        loadFont(fileName, fontSheetGridColumns, fontSheetGridRows, charWidth, charHeight);
    }

    ~Font()
    {
        if (charset != nullptr) {
            for (int i = 0; i < this->charCount; i++) {
                delete[] charset[i];
            }
            delete[] charset;
        }
    }

    bool loadFont(const char *fileName, int fontSheetGridColumns, int fontSheetGridRows, int charWidth, int charHeight)
    {
        unsigned fontSheetWidth, fontSheetHeight, channelCount = 4;
        unsigned char *fontSheet;
        unsigned error = lodepng_decode32_file(&fontSheet, &fontSheetWidth, &fontSheetHeight, fileName);
        if (error) {
            std::cout << "loadFont(): decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            return false;
        }

        if (fontSheetWidth != charWidth * fontSheetGridColumns || fontSheetHeight != charHeight * fontSheetGridRows ) {
            std::cout << "loadFont(): fontSheetWidth and/or fontSheetHeight not match" << std::endl;
            return false;
        }

        this->charWidth = charWidth;
        this->charHeight = charHeight;
        this->charCount = fontSheetGridColumns * fontSheetGridRows;

        this->charset = new int*[this->charCount];

        for (int row = 0; row < fontSheetGridRows; row++) {
            for (int col = 0; col < fontSheetGridColumns; col++) {
                int charIndex = col + row * fontSheetGridColumns;
                this->charset[charIndex] = new int[this->charWidth * this->charHeight];
                int charPos = (col * this->charWidth * channelCount) + row * this->charHeight * (fontSheetWidth * channelCount);
                for (int y = 0; y < this->charHeight; y++) {
                    for (int x = 0; x < this->charWidth; x++) {
                        this->charset[charIndex][x + (this->charHeight - 1 - y) * this->charWidth] =
                            fontSheet[charPos + (x * channelCount + 0) + y * (fontSheetWidth * channelCount)] |
                            fontSheet[charPos + (x * channelCount + 1) + y * (fontSheetWidth * channelCount)] << 8 |
                            fontSheet[charPos + (x * channelCount + 2) + y * (fontSheetWidth * channelCount)] << 16 |
                            fontSheet[charPos + (x * channelCount + 3) + y * (fontSheetWidth * channelCount)] << 24;
                    }
                }
            }
        }

        return true;
    }
};

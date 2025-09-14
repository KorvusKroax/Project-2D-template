#pragma once

#include "lodepng.h"

#include <iostream> // cout
#include <vector>

enum FontType {
    MONOSPACED,
    PROPORTIONAL
};

struct FontChar
{
    int width = 0;
    int* pixels = nullptr;

    FontChar() { }

    FontChar(int width, int height)
    {
        this->width = width;
        this->pixels = new int[this->width * height];
    }

    ~FontChar()
    {
        if (pixels != nullptr) {
            delete[] pixels;
        }
    }
};

struct Font
{
    int charHeight, charCount = 0;
    FontChar** charset;
    FontType fontType;

    Font() { }

    Font(const char *fileName) // proportional
    {
        loadFont(fileName);
        fontType = PROPORTIONAL;
    }

    Font(const char *fileName, int charWidth) // monospaced
    {
        loadFont(fileName, charWidth);
        fontType = MONOSPACED;
    }

    ~Font()
    {
        if (charset != nullptr) {
            delete[] charset;
        }
    }



    bool loadFont(const char *fileName, int charWidth = -1)
    {
        unsigned int fontSheetWidth, fontSheetHeight, channelCount = 4;
        unsigned char *fontSheet;
        unsigned error = lodepng_decode32_file(&fontSheet, &fontSheetWidth, &fontSheetHeight, fileName);
        if (error) {
            std::cout << "loadFont(): decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            return false;
        }

        this->charHeight = fontSheetHeight;

        if (charWidth == -1) {
            this->charCount = 0;
            std::vector<int> charWidths;
            int width = 0;
            for (int i = 0; i < fontSheetWidth; i++) {
                int r = fontSheet[i * channelCount + 0];
                int g = fontSheet[i * channelCount + 1];
                int b = fontSheet[i * channelCount + 2];
                int a = fontSheet[i * channelCount + 3];
                width++;
                if (r == 0 && g == 0 && b == 0 && a != 0) {
                    this->charCount++;
                    charWidths.push_back(width - 1);
                    width = 0;
                }
            }

            if (this->charCount == 0) {
                std::cout << "loadFont(): no proportional character found" << std::endl;
                return false;
            }

            this->charset = new FontChar*[this->charCount];

            int xPos = 0;
            for (int i = 0; i < this->charCount; i++) {
                this->charset[i] = new FontChar(charWidths[i], this->charHeight);
                for (int y = 0; y < this->charHeight; y++) {
                    for (int x = 0; x < charWidths[i]; x++) {
                        this->charset[i]->pixels[x + (this->charHeight - 1 - y) * charWidths[i]] =
                            fontSheet[((xPos + x) * channelCount + 0) + y * (fontSheetWidth * channelCount)] |
                            fontSheet[((xPos + x) * channelCount + 1) + y * (fontSheetWidth * channelCount)] << 8 |
                            fontSheet[((xPos + x) * channelCount + 2) + y * (fontSheetWidth * channelCount)] << 16 |
                            fontSheet[((xPos + x) * channelCount + 3) + y * (fontSheetWidth * channelCount)] << 24;
                    }
                }
                xPos += charWidths[i] + 1;
            }
        } else {
            this->charCount = fontSheetWidth / charWidth;

            if (this->charCount == 0) {
                std::cout << "loadFont(): no monospaced character found" << std::endl;
                return false;
            }

            this->charset = new FontChar*[this->charCount];

            int xPos = 0;
            for (int i = 0; i < this->charCount; i++) {
                this->charset[i] = new FontChar(charWidth, this->charHeight);
                for (int y = 0; y < this->charHeight; y++) {
                    for (int x = 0; x < charWidth; x++) {
                        this->charset[i]->pixels[x + (this->charHeight - 1 - y) * charWidth] =
                            fontSheet[((xPos + x) * channelCount + 0) + y * (fontSheetWidth * channelCount)] |
                            fontSheet[((xPos + x) * channelCount + 1) + y * (fontSheetWidth * channelCount)] << 8 |
                            fontSheet[((xPos + x) * channelCount + 2) + y * (fontSheetWidth * channelCount)] << 16 |
                            fontSheet[((xPos + x) * channelCount + 3) + y * (fontSheetWidth * channelCount)] << 24;
                    }
                }
                xPos += charWidth;
            }
        }

        return true;
    }
};

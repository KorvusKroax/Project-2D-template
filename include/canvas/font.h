/*
    USAGE:
        Font(fileName, charWidth)
            - fileName: a .PNG file which contains the pixels of the charset
                the first character must be the <space>
            - charWidth: one monospaced character width in the .PNG file (optional)
                if no passed charWidth the charset will be proportional

    FUNCTIONS:
        loadFont(*fileName, charWidth)
            - loading charset from the .PNG file
                the charset must be in one row ordered by ascii codes
                - valid pixelcolor = 255, 255, 255, 255
                - separator pixelcolor = 0, 0, 0, 255
*/
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
    unsigned int charHeight, charCount = 0;
    FontChar** charset;
    FontType type;

    Font() { }

    Font(const char *fileName, unsigned int charWidth = 0)
    {
        loadFont(fileName, charWidth);
    }

    ~Font()
    {
        if (charset != nullptr) {
            delete[] charset;
        }
    }

    bool loadFont(const char *fileName, unsigned int charWidth = 0)
    {
        unsigned int fontSheetWidth, channelCount = 4;
        unsigned char* fontSheet;
        unsigned error = lodepng_decode32_file(&fontSheet, &fontSheetWidth, &this->charHeight, fileName);
        if (error) {
            std::cout << "loadFont(): decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            return false;
        }

        if (charWidth > 0) {
            return loadMonospaced(fontSheet, fontSheetWidth, channelCount, charWidth);
        }
        return loadProportional(fontSheet, fontSheetWidth, channelCount);
    }

private:
    bool loadMonospaced(unsigned char *fontSheet, unsigned int fontSheetWidth, unsigned int channelCount, int charWidth)
    {
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

        this->type = MONOSPACED;
        return true;
    }

    bool loadProportional(unsigned char *fontSheet, unsigned int fontSheetWidth, int channelCount)
    {
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

        this->type = PROPORTIONAL;
        return true;
    }
};

/*
    draw_char(x, y, asciiCode, colors, font)
        - draw a charater of the given font to the passed coords, font are optional
            if no given font it will uses a simple 4x6 pixel charaterset

    draw_line(x, y, text, colors, font)
        - write the given text using the given font to the passed coords, font are optional
            if no given font it will uses a simple 4x6 pixel charaterset
            recognize and apply the '\n' (newLine) character

    private calcTextField(text, &colCount, &rowCount)
        - give back row and max column count in charcount (not pixelcount)
*/
#pragma once

#include "canvas.h"
#include "color.h"
#include "font.h"

struct TextColor
{
    Color text, shadow, background;

    TextColor(Color text = CLEAR, Color shadow = CLEAR, Color background = CLEAR)
    {
        this->text = text;
        this->shadow = shadow;
        this->background = background;
    }
};

struct Text
{
    // https://hackaday.io/project/6309-vga-graphics-over-spi-and-serial-vgatonic/log/20759-a-tiny-4x6-pixel-font-that-will-fit-on-almost-any-microcontroller-license-mit
    static bool draw_char(Canvas* canvas, int x, int y, unsigned char asciiCode, TextColor colors)
    {
        if (x + 4 < 0 || x >= canvas->width || y + 6 < 0 || y >= canvas->height) return false;

        if (colors.background.getAlpha() > 0) {
            for (int j = 0; j < 6; j++) {
                for (int i = 0; i < 4; i++) {
                    canvas->setPixel(x + i,  y + j, colors.background);
                }
            }
        }

        int charIndex = asciiCode - 32;
        if (charIndex < 0 || charIndex > 96) {
            canvas->setPixel(x + 1, y + 1, colors.text);
            return false;
        }

        const unsigned short font4x6 [96] {
            0x0000,   /*SPACE*/
            0x4908,   /*'!'*/
            0xb400,   /*'"'*/
            0xbef6,   /*'#'*/
            0x7b7a,   /*'$'*/
            0xa594,   /*'%'*/
            0x55b8,   /*'&'*/
            0x4800,   /*'''*/
            0x2944,   /*'('*/
            0x442a,   /*')'*/
            0x15a0,   /*'*'*/
            0x0b42,   /*'+'*/
            0x0050,   /*','*/
            0x0302,   /*'-'*/
            0x0008,   /*'.'*/
            0x2590,   /*'/'*/
            0x76ba,   /*'0'*/
            0x595c,   /*'1'*/
            0xc59e,   /*'2'*/
            0xc538,   /*'3'*/
            0x92e6,   /*'4'*/
            0xf33a,   /*'5'*/
            0x73ba,   /*'6'*/
            0xe590,   /*'7'*/
            0x77ba,   /*'8'*/
            0x773a,   /*'9'*/
            0x0840,   /*':'*/
            0x0850,   /*';'*/
            0x2a44,   /*'<'*/
            0x1ce0,   /*'='*/
            0x8852,   /*'>'*/
            0xe508,   /*'?'*/
            0x568e,   /*'@'*/
            0x77b6,   /*'A'*/
            0x77b8,   /*'B'*/
            0x728c,   /*'C'*/
            0xd6ba,   /*'D'*/
            0x739e,   /*'E'*/
            0x7392,   /*'F'*/
            0x72ae,   /*'G'*/
            0xb7b6,   /*'H'*/
            0xe95c,   /*'I'*/
            0x64aa,   /*'J'*/
            0xb7b4,   /*'K'*/
            0x929c,   /*'L'*/
            0xbeb6,   /*'M'*/
            0xd6b6,   /*'N'*/
            0x56aa,   /*'O'*/
            0xd792,   /*'P'*/
            0x76ee,   /*'Q'*/
            0x77b4,   /*'R'*/
            0x7138,   /*'S'*/
            0xe948,   /*'T'*/
            0xb6ae,   /*'U'*/
            0xb6aa,   /*'V'*/
            0xb6f6,   /*'W'*/
            0xb5b4,   /*'X'*/
            0xb548,   /*'Y'*/
            0xe59c,   /*'Z'*/
            0x694c,   /*'['*/
            0x9124,   /*'\'*/
            0x642e,   /*']'*/
            0x5400,   /*'^'*/
            0x001c,   /*'_'*/
            0x4400,   /*'`'*/
            0x0eae,   /*'a'*/
            0x9aba,   /*'b'*/
            0x0e8c,   /*'c'*/
            0x2eae,   /*'d'*/
            0x0ece,   /*'e'*/
            0x56d0,   /*'f'*/
            0x553B,   /*'g'*/
            0x93b4,   /*'h'*/
            0x4144,   /*'i'*/
            0x4151,   /*'j'*/
            0x97b4,   /*'k'*/
            0x4944,   /*'l'*/
            0x17b6,   /*'m'*/
            0x1ab6,   /*'n'*/
            0x0aaa,   /*'o'*/
            0xd6d3,   /*'p'*/
            0x7667,   /*'q'*/
            0x1790,   /*'r'*/
            0x0f38,   /*'s'*/
            0x9a8c,   /*'t'*/
            0x16ae,   /*'u'*/
            0x16ba,   /*'v'*/
            0x16f6,   /*'w'*/
            0x15b4,   /*'x'*/
            0xb52b,   /*'y'*/
            0x1c5e,   /*'z'*/
            0x6b4c,   /*'{'*/
            0x4948,   /*'|'*/
            0xc95a,   /*'}'*/
            0x5400,   /*'~'*/
            0x56e2    /*''*/
        };

        const unsigned char pixelValues[6] = {
            (unsigned char)((font4x6[charIndex] >> 12) & 0xe),
            (unsigned char)((font4x6[charIndex] >> 9) & 0xe),
            (unsigned char)(((font4x6[charIndex] & 0x0300) >> 6) | (font4x6[charIndex] & 0x02)),
            (unsigned char)((font4x6[charIndex] >> 4) & 0xe),
            (unsigned char)((font4x6[charIndex] >> 1) & 0xe),
            0
        };

        for (int j = 0; j < 6; j++) {
            unsigned char pixel = pixelValues[j];
            bool setted = false;
            for (int i = 0; i < 4; i++) {
                if ((pixel >> i) & 1) {
                    canvas->setPixel(x + 3 - i, y + 5 - j, colors.text);
                    if (colors.shadow.getAlpha() > 0) {
                        canvas->setPixel(x + 2 - i, y + 4 - j, colors.shadow);
                        if (!setted) canvas->setPixel(x + 3 - i, y + 4 - j, colors.shadow);
                    }
                    setted = true;
                } else setted = false;
            }
        }
        return true;
    }

    static bool draw_line(Canvas* canvas, int x, int y, const char* text, TextColor colors)
    {
        int colCount, rowCount;
        calcTextFieldInChar(text, &colCount, &rowCount);

        if (x + colCount * 4 < 0 || x >= canvas->width || y + rowCount * 6  < 0 || y >= canvas->height) return false;

        int xPos = 0;
        int yPos = rowCount - 1;
        int i = 0;
        while (text[i] != '\0') {
            if (text[i] == '\n') {
                xPos = 0;
                yPos--;
            } else {
                draw_char(canvas, x + xPos * 4, y + yPos * 6, text[i], colors);
                xPos++;
            }
            i++;
        }
        return true;
    }

    static bool draw_char(Canvas* canvas, int x, int y, unsigned char asciiCode, TextColor colors, Font* font, int charSpacing = 0, int lineSpacing = 0)
    {
        if (font->charCount == 0) {
            draw_char(canvas, x, y, asciiCode, colors);
        }

        int charIndex = asciiCode - 32;
        if (charIndex < 0 || charIndex >= font->charCount) {
            canvas->setPixel(x + (font->charset[0]->width >> 1), y + 1, colors.text);
            return false;
        }

        int charWidth = font->charset[charIndex]->width;

        if (x + charWidth < 0 || x >= canvas->width || y + font->charHeight < 0 || y >= canvas->height) return false;

        if (colors.background.getAlpha() > 0) {
            for (int j = 0; j < font->charHeight + lineSpacing; j++) {
                for (int i = 0; i < charWidth + charSpacing; i++) {
                    canvas->setPixel(x + i,  y + j, colors.background);
                }
            }
        }

        int lineSpacingOffset = lineSpacing >> 1;
        int charSpacingOffset = charSpacing >> 1;
        for (int j = font->charHeight - 1; j >= 0; j--) {
            bool setted = false;
            for (int i = 0; i < charWidth; i++) {
                if (font->charset[charIndex]->pixels[i + j * charWidth] & 0xff000000) {
                    canvas->setPixel(x + i + charSpacingOffset,  y + j + lineSpacingOffset, colors.text);
                    if (colors.shadow.getAlpha() > 0) {
                        canvas->setPixel(x + i + charSpacingOffset, y + j - 1 + lineSpacingOffset, colors.shadow);
                        if (!setted) canvas->setPixel(x + i - 1 + charSpacingOffset, y + j - 1 + lineSpacingOffset, colors.shadow);
                    }
                    setted = true;
                } else setted = false;
            }
        }
        return true;
    }

    static bool draw_line(Canvas* canvas, int x, int y, const char* text, TextColor colors, Font* font, int charSpacing = 1, int lineSpacing = 0)
    {
        if (font->charCount == 0) {
            draw_line(canvas, x, y, text, colors);
        }

        if (font->type == MONOSPACED) charSpacing = 0;

        int maxWidth, maxHeight;
        calcTextFieldInPixel(text, &maxWidth, &maxHeight, font, charSpacing, lineSpacing);

        if (x + maxWidth < 0 || x >= canvas->width || y + maxHeight  < 0 || y >= canvas->height) return false;

        int xPos = 0;
        int yPos = maxHeight - (font->charHeight + lineSpacing);
        int i = 0;
        while (text[i] != '\0') {
            if (text[i] == '\n') {
                xPos = 0;
                yPos -= font->charHeight + lineSpacing;
            } else {
                draw_char(canvas, x + xPos, y + yPos, text[i], colors, font, charSpacing, lineSpacing);
                xPos += font->charset[text[i] - 32]->width + charSpacing;
            }
            i++;
        }
        return true;
    }

private:
    static void calcTextFieldInChar(const char* text, int* colCount, int* rowCount)
    {
        *colCount = 0;
        *rowCount = 1;

        int count = 0;
        int i = 0;
        while (text[i] != '\0') {
            if (text[i] == '\n') {
                *colCount = std::max(*colCount, count);
                (*rowCount)++;
                count = 0;
            } else count++;
            i++;
        }
        *colCount = std::max(*colCount, count);
    }

    static void calcTextFieldInPixel(const char* text, int* width, int* height, Font* font, int charSpacing, int lineSpacing)
    {
        *width = 0;
        *height = font->charHeight + lineSpacing;

        int pixelCount = 0;
        int i = 0;
        while (text[i] != '\0') {
            if (text[i] == '\n') {
                *width = std::max(*width, pixelCount);
                (*height) += font->charHeight + lineSpacing;
                pixelCount = 0;
            } else {
                pixelCount += font->charset[text[i] - 32]->width + charSpacing;
            }
            i++;
        }
        *width = std::max(*width, pixelCount);
    }
};

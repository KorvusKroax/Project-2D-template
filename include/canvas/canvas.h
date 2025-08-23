/*
    USAGE:
        Canvas(width, height)
            - width: width in pixels
            - height: height in pixels

        Canvas(fileName)
            - fileName of a .PNG file that will be loaded, the canvas will be sized by the picture size

    FUNCTIONS:
        init(width, height)
            - create or resize the canvas

        clearPixelBuffer()
            - fill all bytes of pixelBuffer with 0 (means all pixel will be Color(0, 0, 0, 0))

        setPixel(x, y, color)
            - sets a pixel as the passed color, if the color's alpha is not 255, this color will be added to the pixel's current color below
            - returns
                true: the pixel is created
                false: the coords are out of canvas or the color is full transparent

        setPixels(x, y, &canvas)
            - replace pixels with the pixels of the given canvas from passed coords which will be start (0,0) of the given canvas,
                alpha has no effect
            - returns
                true: pixels replaced by the given canvas (even partially)
                false: the area is out canvas

        getPixel(x, y, &color)
            - it sets the given color with the pixel's color at the passed coords
            - returns
                true: has pixel at the given coords
                false has no pixel at the given coords

        getPixels(x, y, width, height, &canvas)
            - it inits the given canvas as passed width and height, and sets its pixels with the pixels from the coords to passed width and height,
                where the area is offscreen, there will be garbage on the given canvas
            - returns
                true: pixels getted from the canvas (even also partially)
                false: the area is out canvas

        drawChar(x, y, asciiCode, charColor, shadowColor, backgroundColor)
            - draw a simple 4x6 pixel charater to the passed coords, shadowColor and backgroundColor are optional

        drawText(x, y, text, textColor, shadowColor, backgroundColor)
            - write the given text with simple 4x6 pixel charaters to the passed coords, shadowColor and backgroundColor are optional

        loadImage_PNG(fileName)
            - load a .PNG image named as fileName, it will resize the canvas
            -return
                true: canvas resized and image loaded succesfully
                false: error occoured while trying to load .PNG image, canvas not changed
*/
#pragma once

#include "color.h"
#include "lodepng.h"

#include <iostream> // cout
#include <cstring> // memset

struct Canvas
{
    int width, height;
    unsigned int* pixelBuffer;

    Canvas() { }

    Canvas(int width, int height) { init(width, height); }

    Canvas(const char* fileName) { loadImage_PNG(fileName); }

    ~Canvas() { delete[] this->pixelBuffer; }

    void init(int width, int height)
    {
        this->width = width;
        this->height = height;
        this->pixelBuffer = new unsigned int[this->width * this->height];
    }

    void clearPixelBuffer() { memset(this->pixelBuffer, 0, this->width * this->height * sizeof(unsigned int)); }

    bool setPixel(int x, int y, Color color)
    {
        if (x < 0 || x >= this->width || y < 0 || y >= this->height) return false;

        if (color.getAlpha() < 255) {
            if (color.getAlpha() == 0) return false;
            color = Color(this->pixelBuffer[x + y * width]).blend(color);
        }

        this->pixelBuffer[x + y * this->width] = color.value;
        return true;
    }

    bool setPixels(int x, int y, Canvas* canvas)
    {
        if (x + canvas->width < 0 || x >= this->width || y + canvas->height < 0 || y >= this->height) return false;

        for (int j = 0; j < canvas->height; j++)  {
            if (y + j < 0) continue;
            if (y + j >= this->height) break;
            for (int i = 0; i < canvas->width; i++) {
                if (x + i < 0) continue;
                if (x + i >= this->width) break;
                this->pixelBuffer[(x + i) + (y + j) * this->width] = canvas->pixelBuffer[i + j * canvas->width];
            }
        }
        return true;
    }

    bool getPixel(int x, int y, Color* color)
    {
        if (x < 0 || x >= this->width || y < 0 || y >= this->height) return false;

        *color = Color(this->pixelBuffer[x + y * width]);
        return true;
    }

    bool getPixels(int x, int y, int width, int height, Canvas* canvas)
    {
        if (x + width < 0 || x >= this->width || y + height < 0 || y >= this->height) return false;

        canvas->init(width, height);
        for (int j = 0; j < canvas->height; j++)  {
            if (y + j < 0) continue;
            if (y + j >= this->height) break;
            for (int i = 0; i < canvas->width; i++) {
                if (x + i < 0) continue;
                if (x + i >= this->width) break;
                canvas->pixelBuffer[i + j * canvas->width] = this->pixelBuffer[(x + i) + (y + j) * this->width];
            }
        }
        return true;
    }

    bool drawChar(int x, int y, unsigned char asciiCode, Color charColor, Color shadowColor = CLEAR, Color backgroundColor = CLEAR)
    {
        if (x + 4 < 0 || x >= this->width || y + 6 < 0 || y >= this->height) return false;

        if (backgroundColor.getAlpha() > 0) {
            for (int j = 0; j < 6; j++) {
                for (int i = 0; i < 4; i++) {
                    setPixel(x + i,  y + j, backgroundColor);
                }
            }
        }

        int charIndex = asciiCode - 32;
        for (int j = 0; j < 6; j++) {
            unsigned char pixel = 0;
            switch (j - (this->font4x6[charIndex] & 1)) {
                case 0: pixel = (this->font4x6[charIndex] >> 12) & 0xe; break;
                case 1: pixel = (this->font4x6[charIndex] >> 9) & 0xe; break;
                case 2: pixel = ((this->font4x6[charIndex] & 0x0300) >> 6) | (this->font4x6[charIndex] & 0x02); break;
                case 3: pixel = (this->font4x6[charIndex] >> 4) & 0xe; break;
                case 4: pixel = (this->font4x6[charIndex] >> 1) & 0xe; break;
            }

            bool setted = false;
            for (int i = 0; i < 4; i++) {
                if ((pixel >> i) & 1) {
                    setPixel(x + 3 - i,  y + 5 - j, charColor);
                    if (shadowColor.getAlpha() > 0) {
                        setPixel(x + 2 - i, y + 4 - j, shadowColor);
                        if (!setted) setPixel(x + 3 - i, y + 4 - j, shadowColor);
                        setted = true;
                    }
                } else setted = false;
            }
        }
        return true;
    }

    bool drawText(int x, int y, const char* text, Color textColor, Color shadowColor = CLEAR, Color backgroundColor = CLEAR)
    {
        if (x + 4 * strlen(text) < 0 || x >= this->width || y + 6 < 0 || y >= this->height) return false;

        int index = 0;
        while (text[index] != '\0') {
            drawChar(x + index * 4, y, text[index], textColor, shadowColor, backgroundColor);
            index++;
        }
        return true;
    }

    bool loadImage_PNG(const char* fileName)
    {
        unsigned imageWidth, imageHeight, channelCount = 4;
        unsigned char *image;
        unsigned error = lodepng_decode32_file(&image, &imageWidth, &imageHeight, fileName);
        if (error) {
            std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
            return false;
        }

        init(imageWidth, imageHeight);
        for (int i = 0; i < this->width; i++) {
            for (int j = 0; j < this->height; j++) {
                this->pixelBuffer[i + j * this->width] =
                    image[(i * channelCount + 0) + (this->height - 1 - j) * (this->width * channelCount)] |
                    image[(i * channelCount + 1) + (this->height - 1 - j) * (this->width * channelCount)] << 8 |
                    image[(i * channelCount + 2) + (this->height - 1 - j) * (this->width * channelCount)] << 16 |
                    image[(i * channelCount + 3) + (this->height - 1 - j) * (this->width * channelCount)] << 24;
            }
        }
        return true;
    }

private:
    const unsigned short font4x6 [96] { // https://hackaday.io/project/6309-vga-graphics-over-spi-and-serial-vgatonic/log/20759-a-tiny-4x6-pixel-font-that-will-fit-on-almost-any-microcontroller-license-mit
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
};

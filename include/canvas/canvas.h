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

        fillPixelBuffer(color)
            - fill all integer of pixelBuffer with passed color's value (slower than clearPixelBuffer())

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

    void fillPixelBuffer(Color color)
    {
        for (int i = 0; i < this->width * this->height; i++) {
            this->pixelBuffer[i] = color.value;
        }
    }

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

    bool loadImage_PNG(const char* fileName)
    {
        unsigned int imageWidth, imageHeight, channelCount = 4;
        unsigned char *image;
        unsigned error = lodepng_decode32_file(&image, &imageWidth, &imageHeight, fileName);
        if (error) {
            std::cout << "loadImage_PNG(): decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
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
};

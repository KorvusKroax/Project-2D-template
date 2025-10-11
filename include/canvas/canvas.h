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

#include <cstring> // memset

struct Canvas
{
    int width, height;
    std::vector<unsigned char> pixelBuffer; // RGBA

    Canvas() { }

    Canvas(int w, int h): width(w), height(h), pixelBuffer(w * h * 4, 0) { }

    Canvas(const char* fileName)
    {
        loadImage_PNG(fileName);
    }

    void resize(int newWidth, int newHeight, unsigned char fill = 0)
    {
        this->width = newWidth;
        this->height = newHeight;
        this->pixelBuffer.assign(this->width * this->height * 4, fill);
    }

    void clear()
    {
        memset(this->pixelBuffer.data(), 0, this->pixelBuffer.size());
    }

    void fill(Color color)
    {
        for (int i = 0; i < this->pixelBuffer.size(); i += 4) {
            pixelBuffer[i + 0] = color.r;
            pixelBuffer[i + 1] = color.g;
            pixelBuffer[i + 2] = color.b;
            pixelBuffer[i + 3] = color.a;
        }
    }

    void fastFill(Color color)
    {
        const size_t pixelCount = this->width * this->height;
        unsigned char c[4] = { color.r, color.g, color.b, color.a };

        std::memcpy(this->pixelBuffer.data(), c, 4);

        size_t filled = 1;
        while (filled < pixelCount) {
            size_t copyCount = std::min(filled, pixelCount - filled);
            std::memcpy(this->pixelBuffer.data() + filled * 4, this->pixelBuffer.data(), copyCount * 4);
            filled += copyCount;
        }
    }

    bool setPixel(int x, int y, Color color)
    {
        if (color.a == 0 || x < 0 || x >= this->width || y < 0 || y >= this->height) return false;

        int index = (x + y * this->width) * 4;
        if (color.a < 255) {
            color = Color(
                this->pixelBuffer[index + 0],
                this->pixelBuffer[index + 1],
                this->pixelBuffer[index + 2],
                this->pixelBuffer[index + 3]).blend(color);
        }
        this->pixelBuffer[index + 0] = color.r;
        this->pixelBuffer[index + 1] = color.g;
        this->pixelBuffer[index + 2] = color.b;
        this->pixelBuffer[index + 3] = color.a;

        return true;
    }

    bool getPixel(int x, int y, Color* color)
    {
        if (x < 0 || x >= this->width || y < 0 || y >= this->height) return false;

        int index = (x + y * width) * 4;
        *color = Color(
            this->pixelBuffer[index + 0],
            this->pixelBuffer[index + 1],
            this->pixelBuffer[index + 2],
            this->pixelBuffer[index + 3]
        );

        return true;
    }

    bool setPixels(int x, int y, const Canvas* canvas)
    {
        int destStartX = std::max(0, x);
        int destStartY = std::max(0, y);
        int destEndX = std::min(this->width, x + canvas->width);
        int destEndY = std::min(this->height, y + canvas->height);

        if (destStartX >= destEndX || destStartY >= destEndY) return false;

        int srcOffsetX = destStartX - x;
        int srcOffsetY = destStartY - y;

        for (int i = 0; i < destEndY - destStartY; i++) {
            const int destIndex = (destStartX + (destStartY + i) * this->width) * 4;
            const int srcIndex  = (srcOffsetX + (srcOffsetY + i) * canvas->width) * 4;
            std::memcpy(&this->pixelBuffer[destIndex], &canvas->pixelBuffer[srcIndex], (destEndX - destStartX) * 4);
        }

        return true;
    }

    bool getPixels(int x, int y, int width, int height, Canvas* canvas)
    {
        canvas->resize(width, height);

        int srcStartX = std::max(0, x);
        int srcStartY = std::max(0, y);
        int srcEndX   = std::min(this->width,  x + width);
        int srcEndY   = std::min(this->height, y + height);

        if (srcStartX >= srcEndX || srcStartY >= srcEndY)
            return false;

        int destOffsetX = srcStartX - x;
        int destOffsetY = srcStartY - y;

        for (int j = 0; j < srcEndY - srcStartY; ++j) {
            const int srcIndex  = (srcStartX + (srcStartY + j) * this->width) * 4;
            const int destIndex = (destOffsetX + (destOffsetY + j) * width) * 4;
            std::memcpy(&canvas->pixelBuffer[destIndex], &this->pixelBuffer[srcIndex], (srcEndX - srcStartX) * 4);
        }

        return true;
    }

    bool loadImage_PNG(const char* fileName)
    {
        unsigned int imageWidth, imageHeight, channelCount = 4;
        unsigned char* image;
        unsigned int error = lodepng_decode32_file(&image, &imageWidth, &imageHeight, fileName);
        if (error) {
            printf("Canvas::loadImage_PNG ERROR: %i\n", error);
            return false;
        }

        resize(imageWidth, imageHeight);

        for (int y = 0; y < this->height; y++) {
            std::memcpy(
                this->pixelBuffer.data() + y * this->width * channelCount,
                image + (this->height - 1 - y) * this->width * channelCount,
                this->width * channelCount
            );
        }
        free(image);

        return true;
    }
};

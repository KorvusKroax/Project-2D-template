#pragma once

#include "color.h"
#include "lodepng/lodepng.h"

#include <iostream> // cout
#include <cstring> // memset

struct Canvas
{
    size_t width, height;
    unsigned int* pixelBuffer;

    Canvas() { }

    Canvas(size_t width, size_t height) { init(width, height); }

    Canvas(const char* fileName) { loadImage_PNG(fileName); }

    ~Canvas() { delete[] this->pixelBuffer; }

    void init(size_t width, size_t height)
    {
        this->width = width;
        this->height = height;
        this->pixelBuffer = new unsigned int[this->width * this->height];
    }

    void clearPixelBuffer() { memset(this->pixelBuffer, 0, this->width * this->height * sizeof(int)); }

    bool setPixel(int x, int y, Color color)
    {
        if (x < 0 || x >= this->width || y < 0 || y >= this->height) return false;

        if (color.getAlpha() < 255) {
            if (color.getAlpha() == 0) return false;
            color = Color(this->pixelBuffer[x + y * width]).add(color);
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
};

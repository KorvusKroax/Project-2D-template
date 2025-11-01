#pragma once

#include "color/color.h"

#include <vector>

class Canvas
{
    public:
        int width, height;
        std::vector<unsigned char> pixelBuffer; // RGBA

        Canvas();
        Canvas(int width, int height);
        Canvas(const char* fileName);

        void resize(int newWidth, int newHeight, unsigned char fill = 0);
        void clear();
        void fill(Color color);
        void fastFill(Color color);
        bool setPixel(int x, int y, Color color);
        bool getPixel(int x, int y, Color* color);
        bool setPixels(int x, int y, const Canvas* canvas);
        bool getPixels(int x, int y, int width, int height, Canvas* canvas);
        bool loadImage_PNG(const char* fileName);
};

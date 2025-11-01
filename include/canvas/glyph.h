#pragma once

#include <vector>

struct Glyph
{
    int x0, y0;
    int width, height;
    int advanceWidth;
    std::vector<unsigned char> bitmap;

    Glyph()
    {}

    Glyph(int x0, int y0, int widht, int height, int advanceWidth)
        : x0(x0), y0(y0), width(widht), height(height), advanceWidth(advanceWidth), bitmap(width * height, 0)
    {}
};

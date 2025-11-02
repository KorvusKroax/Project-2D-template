#pragma once

#include "color/palettes/base.h"
#include "color/palettes/ega.h"
#include "color/palettes/c64.h"

struct Color
{
    unsigned char r, g, b, a;

    Color()
    {}

    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255)
        : r(r), g(g), b(b), a(a)
    {}

    Color grayscale()
    {
        int v = this->r * 0.299f + this->g * 0.587f + this->b * 0.114f;
        return Color(v, v, v, this->a);
    }

    Color blend(const Color& foreground) const // this = background
    {
        return Color(
            (unsigned char)(int(foreground.r * foreground.a + this->r * (255 - foreground.a)) >> 8),
            (unsigned char)(int(foreground.g * foreground.a + this->g * (255 - foreground.a)) >> 8),
            (unsigned char)(int(foreground.b * foreground.a + this->b * (255 - foreground.a)) >> 8),
            foreground.a + this->a * (255 - foreground.a)
        );
    }

    bool operator == (const Color& other) const
    {
        return this->r == other.r && this->g == other.g && this->b == other.b && this->a == other.a;
    }

    bool operator != (const Color& other) const
    {
        return !(*this == other);
    }
};

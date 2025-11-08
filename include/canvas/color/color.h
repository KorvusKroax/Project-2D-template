#pragma once

#include "color/palettes/base.h"

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

    Color blend(const Color& other) const
    {
        return Color(
            (unsigned char)(int(other.r * other.a + this->r * (255 - other.a)) >> 8),
            (unsigned char)(int(other.g * other.a + this->g * (255 - other.a)) >> 8),
            (unsigned char)(int(other.b * other.a + this->b * (255 - other.a)) >> 8),
            (unsigned char)(int(other.a * 255 + this->a * (255 - other.a)) >> 8)
        );
    }

    bool operator == (const Color& other) const
    {
        return this->r == other.r && this->g == other.g && this->b == other.b;// && this->a == other.a;
    }

    bool operator != (const Color& other) const
    {
        return !(*this == other);
    }
};

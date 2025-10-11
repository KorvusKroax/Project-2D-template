#pragma once

#include "palette_base.h"
#include "palette_ega.h"
#include "palette_c64.h"

struct Color
{
    unsigned char r, g, b, a;

    Color() { }

    Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255): r(r), g(g), b(b), a(a) { }

    Color grayscale()
    {
        int v = this->r * 0.299f + this->g * 0.587f + this->b * 0.114f;
        return Color(v, v, v, this->a);
    }

    Color blend(const Color& fg) const
    {
        return Color(
            (unsigned char)(int(fg.r * fg.a + this->r * (255 - fg.a)) >> 8),
            (unsigned char)(int(fg.g * fg.a + this->g * (255 - fg.a)) >> 8),
            (unsigned char)(int(fg.b * fg.a + this->b * (255 - fg.a)) >> 8)
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

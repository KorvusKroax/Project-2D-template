#pragma once

#include "palette_base.h"
#include "palette_ega.h"
#include "palette_c64.h"

#include <algorithm> // min

struct Color
{
    unsigned int value;

    Color(unsigned int r, unsigned int g, unsigned int b, unsigned int a = 255)
    {
        this->value = r | (g << 8) | (b << 16) | (a << 24);
    }

    Color(unsigned int value = 0, unsigned int a = 255)
    {
        this->value = (value & 0x00ffffff) | (a << 24);
    }

    inline int getRed() { return this->value & 0x000000ff; }
    inline int getGreen() { return (this->value >> 8) & 0x000000ff; }
    inline int getBlue() { return (this->value >> 16) & 0x000000ff; }
    inline int getAlpha() { return (this->value >> 24) & 0x000000ff; }

    void setRed(unsigned int v) { this->value = this->value & 0xffffff00 | v; }
    void setGreen(unsigned int v) { this->value = this->value & 0xffff00ff | (v << 8); }
    void setBlue(unsigned int v) { this->value = this->value & 0xff00ffff | (v << 16); }
    void setAlpha(unsigned int v) { this->value = this->value & 0x00ffffff | (v << 24); }

    int grayscale()
    {
        int v = getRed() * 0.299f + getGreen() * 0.587f + getBlue() * 0.114f;
        return v | (v << 8) | (v << 16) | (getAlpha() << 24);
    }

    Color add(Color color)
    {
        return Color(
            std::min(255, int(getRed()   + (color.getRed()   * color.getAlpha()) / 255.0f)),
            std::min(255, int(getGreen() + (color.getGreen() * color.getAlpha()) / 255.0f)),
            std::min(255, int(getBlue()  + (color.getBlue()  * color.getAlpha()) / 255.0f)),
            std::min(255, getAlpha() + color.getAlpha())
        );
    }

    Color blend(Color color)
    {
        int r1 = color.getRed();
        int g1 = color.getGreen();
        int b1 = color.getBlue();
        int a1 = color.getAlpha();

        int r2 = getRed();
        int g2 = getGreen();
        int b2 = getBlue();
        int a2 = getAlpha();

        int outA = a1 + (a2 * (255 - a1) >> 8);
        if (outA == 0) return Color(0, 0, 0, 0);

        return Color(
            (r1 * a1 + (r2 * a2 * (255 - a1) >> 8) + (outA >> 1)) / outA,
            (g1 * a1 + (g2 * a2 * (255 - a1) >> 8) + (outA >> 1)) / outA,
            (b1 * a1 + (b2 * a2 * (255 - a1) >> 8) + (outA >> 1)) / outA
        );
    }
};

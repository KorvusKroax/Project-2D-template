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
        int v = this->getRed() * 0.299f + this->getGreen() * 0.587f + this->getBlue() * 0.114f;
        return v | (v << 8) | (v << 16) | (this->getAlpha() << 24);
    }

    Color blend(Color fg) // "this" is bg color
    {
        int aOut = fg.getAlpha() + (this->getAlpha() * (255 - fg.getAlpha())) >> 8;
        if (aOut == 0) return Color(0, 0, 0, 0);

        return Color(
            std::min((fg.getRed()   * fg.getAlpha() + (this->getRed()   * this->getAlpha() * (255 - fg.getAlpha()) >> 8) + (aOut >> 1)) / aOut, 255),
            std::min((fg.getGreen() * fg.getAlpha() + (this->getGreen() * this->getAlpha() * (255 - fg.getAlpha()) >> 8) + (aOut >> 1)) / aOut, 255),
            std::min((fg.getBlue()  * fg.getAlpha() + (this->getBlue()  * this->getAlpha() * (255 - fg.getAlpha()) >> 8) + (aOut >> 1)) / aOut, 255)
        );
    }
};

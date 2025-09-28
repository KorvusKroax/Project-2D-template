#pragma once

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <vector>
#include <map>

struct Glyph
{
    int width, height;
    int xOffset, yOffset;
    int advance;
    std::vector<unsigned char> bitmap;

    Glyph() { }

    Glyph(int width, int height, int x, int y, int advance)
    {
        this->width = width;
        this->height = height;
        this->xOffset = x;
        this->yOffset = y;
        this->advance = advance;

        this->bitmap.resize(this->width * this->height);
    }
};

struct Font
{
    stbtt_fontinfo font;
    std::vector<unsigned char> buffer;
    float scale;
    int ascent, descent, lineGap;
    std::map<int, Glyph> glyphCache;

    Font() { }

    Font(const char* filename, int pixelHeight)
    {
        loadFont(filename, pixelHeight);
    }

    bool loadFont(const char* filename, int pixelHeight)
    {
        FILE* file = fopen(filename, "rb");
        if (!file) return false;

        fseek(file, 0, SEEK_END);
        size_t size = ftell(file);
        fseek(file, 0, SEEK_SET);
        this->buffer.resize(size);
        fread(this->buffer.data(), 1, size, file);
        fclose(file);

        if (!stbtt_InitFont(&this->font, this->buffer.data(), stbtt_GetFontOffsetForIndex(this->buffer.data(), 0))) return false;

        this->scale = stbtt_ScaleForPixelHeight(&this->font, pixelHeight);
        stbtt_GetFontVMetrics(&this->font, &this->ascent, &this->descent, &this->lineGap);

        this->glyphCache.clear();

        return true;
    }

    Glyph* getGlyph(int codepoint)
    {
        std::map<int, Glyph>::iterator iterator = this->glyphCache.find(codepoint);
        if (iterator != this->glyphCache.end()) return &iterator->second;

        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(&this->font, codepoint, &advanceWidth, &leftSideBearing);

        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&this->font, codepoint, this->scale, this->scale, &x0, &y0, &x1, &y1);

        Glyph glyph(x1 - x0, y1 - y0, x0, y0, int(advanceWidth * this->scale));

        stbtt_MakeCodepointBitmap(&this->font, glyph.bitmap.data(), glyph.width, glyph.height, glyph.width, this->scale, this->scale, codepoint);

        this->glyphCache[codepoint] = std::move(glyph);

        return &this->glyphCache[codepoint];
    }
};

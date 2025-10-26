#pragma once

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <vector>
#include <map>
#include <string>
#include <fstream>

struct Glyph
{
    int x0, y0;
    int width, height;
    int advanceWidth;
    std::vector<unsigned char> bitmap;

    Glyph() { }

    Glyph(int x0, int y0, int widht, int height, int advanceWidth):
        x0(x0), y0(y0), width(widht), height(height), advanceWidth(advanceWidth)
    {
        this->bitmap.resize(this->width * this->height);
    }
};

struct Font
{
    stbtt_fontinfo info;
    std::vector<unsigned char> buffer;
    float scale;
    int ascent, descent, lineGap;
    std::map<int, Glyph> glyphCache;

    explicit Font(const std::string &filename, int pixelHeight = 16)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            printf("ERROR: Font::Font() -- Failed to load font, file not found\n");
            return;
        }

        this->buffer = std::vector<unsigned char>(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );

        if (!stbtt_InitFont(&this->info, this->buffer.data(), stbtt_GetFontOffsetForIndex(this->buffer.data(), 0))) {
            printf("ERROR: Font::Font() -- Failed to init font\n");
        }

        this->scale = stbtt_ScaleForPixelHeight(&this->info, pixelHeight);

        stbtt_GetFontVMetrics(&this->info, &this->ascent, &this->descent, &this->lineGap);
    }

    static int utf8ToCodepoint(const char* utf8char, int* bytesUsed = nullptr)
    {
        const unsigned char ch = (unsigned char)utf8char[0];
        int codepoint = 0;
        int length = 0;

        if (ch < 0x80) {
            codepoint = ch;
            length = 1;
        } else if ((ch >> 5) == 0x6) {
            codepoint = ((ch & 0x1f) << 6) | ((unsigned char)utf8char[1] & 0x3f);
            length = 2;
        } else if ((ch >> 4) == 0xe) {
            codepoint = ((ch & 0x0f) << 12) |
                (((unsigned char)utf8char[1] & 0x3f) << 6) |
                ((unsigned char)utf8char[2] & 0x3f);
            length = 3;
        } else if ((ch >> 3) == 0x1e) {
            codepoint = ((ch & 0x07) << 18) |
                (((unsigned char)utf8char[1] & 0x3f) << 12) |
                (((unsigned char)utf8char[2] & 0x3f) << 6) |
                ((unsigned char)utf8char[3] & 0x3f);
            length = 4;
        } else {
            codepoint = 0xfffd;
            length = 1;
        }

        if (bytesUsed) *bytesUsed = length;
        return codepoint;
    }

    static std::vector<int> utf8ToCodepoints(const std::string &str)
    {
        std::vector<int> result;
        int i = 0;

        while (i < str.size()) {
            int used = 0;
            int cp = utf8ToCodepoint(&str[i], &used);

            if (used == 0) break;

            result.push_back(cp);
            i += used;
        }

        return result;
    }

    Glyph* getGlyph(int codepoint)
    {
        std::map<int, Glyph>::iterator iterator = this->glyphCache.find(codepoint);
        if (iterator != this->glyphCache.end()) return &iterator->second;

        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(&this->info, codepoint, &advanceWidth, &leftSideBearing);

        int x0, y0, x1, y1;
        stbtt_GetCodepointBitmapBox(&this->info, codepoint, this->scale, this->scale, &x0, &y0, &x1, &y1);

        Glyph glyph(x0, y0, x1 - x0, y1 - y0, advanceWidth);
        stbtt_MakeCodepointBitmap(&this->info, glyph.bitmap.data(), glyph.width, glyph.height, glyph.width, this->scale, this->scale, codepoint);

        this->glyphCache[codepoint] = std::move(glyph);

        return &this->glyphCache[codepoint];
    }
};

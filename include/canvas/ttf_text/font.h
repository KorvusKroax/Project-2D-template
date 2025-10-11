#pragma once

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream> // cerr

struct Glyph
{
    int codepoint;
    int x0, y0;
    int width, height;
    int advanceWidth;
    std::vector<unsigned char> bitmap;

    Glyph() { }

    Glyph(int codepoint, int x0, int y0, int widht, int height, int advanceWidth): codepoint(codepoint), width(widht), height(height), x0(x0), y0(y0), advanceWidth(advanceWidth)
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

    Font(const std::string &filename, int pixelHeight)
    {
        std::ifstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Failed to load font!\n";
            return;
        }

        this->buffer = std::vector<unsigned char>(
            std::istreambuf_iterator<char>(file),
            std::istreambuf_iterator<char>()
        );

        if (!stbtt_InitFont(&this->info, this->buffer.data(), stbtt_GetFontOffsetForIndex(this->buffer.data(), 0))) {
            std::cerr << "Failed to load font!\n";
        }

        this->scale = stbtt_ScaleForPixelHeight(&this->info, pixelHeight);

        stbtt_GetFontVMetrics(&this->info, &this->ascent, &this->descent, &this->lineGap);
    }

    static std::vector<int> utf8_to_codepoints(const std::string text)
    {
        std::vector<int> result;

        int i = 0;
        while (i < text.size()) {
            unsigned char asciiCode = text[i];
            int codepoint = 0;
            int extra = 0;

            if (asciiCode < 0x80) {
                codepoint = asciiCode;
                extra = 0;
            } else if ((asciiCode >> 5) == 0x6) {
                codepoint = asciiCode & 0x1F;
                extra = 1;
            } else if ((asciiCode >> 4) == 0xe) {
                codepoint = asciiCode & 0x0f;
                extra = 2;
            } else if ((asciiCode >> 3) == 0x1e) {
                codepoint = asciiCode & 0x07;
                extra = 3;
            } else {
                i++;
                continue;
            }

            if (i + extra >= text.size()) break;

            for (int j = 1; j <= extra; j++) {
                unsigned char asciiCode = text[i + j];
                if ((asciiCode >> 6) != 0x2) {
                    codepoint = '?';
                    break;
                }
                codepoint = (codepoint << 6) | (asciiCode & 0x3F);
            }

            result.push_back(codepoint);
            i += extra + 1;
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

        Glyph glyph(codepoint, x0, y0, x1 - x0, y1 - y0, advanceWidth);
        stbtt_MakeCodepointBitmap(&this->info, glyph.bitmap.data(), glyph.width, glyph.height, glyph.width, this->scale, this->scale, glyph.codepoint);

        this->glyphCache[codepoint] = std::move(glyph);

        return &this->glyphCache[codepoint];
    }
};

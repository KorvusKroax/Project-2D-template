#include "font.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <fstream>

Font::Font(const std::string& filename, int pixelHeight)
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

Glyph* Font::getGlyph(int codepoint)
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

float Font::calcFontHeight()
{
    return (this->ascent - this->descent + this->lineGap) * this->scale;
}

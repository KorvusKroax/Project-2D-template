#pragma once

#include "stb_truetype.h"

#include "ui/glyph.h"

#include <vector>
#include <map>
#include <string>

class Font
{
public:
    stbtt_fontinfo info;
    std::vector<unsigned char> buffer;
    float scale;
    int ascent, descent, lineGap;
    std::map<int, Glyph> glyphCache;

    Font(const std::string& filename, int pixelHeight = 16);

    Glyph* getGlyph(int codepoint);
    float calcFontHeight();
};

#include "draw/text.h"

#include "glyph.h"

#include <vector>
#include <cmath> // lround

void Text::draw(Canvas* canvas, float x, float y, const std::string& text, const Options& opts)
{
    float lineHeight = calcLineHeight(opts);

    int start = 0;
    float yPos = y;
    while (start < text.size()) {
        int end = text.find('\n', start);
        draw_line(canvas, x, yPos, utf8ToCodepoints(text.substr(start, end - start)), opts);
        yPos -= lineHeight;
        start = (end == std::string::npos) ? text.size() : end + 1;
    }
}

void Text::draw_line(Canvas* canvas, float x, float y, const std::vector<int>& codepoints, const Text::Options& opts)
{
    float advanceWidth;
    for (int i = 0; i < codepoints.size(); i++) {
        draw_char(canvas, x, y, codepoints[i], opts, &advanceWidth);
        x += advanceWidth;
        if (i + 1 < codepoints.size()) {
            x += stbtt_GetCodepointKernAdvance(&opts.font->info, codepoints[i], codepoints[i + 1]) * opts.font->scale;
        }
    }
}

void Text::draw_char(Canvas* canvas, float x, float y, int codepoint, const Text::Options& opts, float* advanceWidth)
{
    if (codepoint == '\t') {
        Glyph* glyph = opts.font->getGlyph(' ');
        if (advanceWidth) *advanceWidth = (glyph->advanceWidth * 4 + stbtt_GetCodepointKernAdvance(&opts.font->info, ' ', ' ') * 3) * opts.font->scale;
        return;
    }

    Glyph* glyph = opts.font->getGlyph(codepoint);

    Color textColor = opts.textColor;
    if (glyph->width > 0 && glyph->height > 0) {
        for (int j = 0; j < glyph->height; j++) {
            bool setted = false;
            for (int i = 0; i < glyph->width; i++) {
                textColor.a = glyph->bitmap[j * glyph->width + i];
                if (textColor.a == 0) {
                    setted = false;
                    continue;
                }

                int px = (int)std::lround(x + glyph->x0 + i);
                int py = (int)std::lround((y  - glyph->y0) - j);
                canvas->setPixel(px, py, textColor);

                if (opts.shadowColor.a > 0) {
                    canvas->setPixel(px + 1, py - 1, opts.shadowColor);
                    if (!setted) canvas->setPixel(px, py - 1, opts.shadowColor);
                    setted = true;
                }
            }
        }
    }

    if (advanceWidth) *advanceWidth = glyph->advanceWidth * opts.font->scale;
    return;
}

int Text::utf8ToCodepoint(const char* utf8char, int* bytesUsed)
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

std::vector<int> Text::utf8ToCodepoints(const std::string& str)
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

// it can't handle tabs and line breaks
float Text::calcLineWidth(const std::vector<int>& codepoints, const Options& opts)
{
    float textWidth = 0;
    for (int i = 0; i < codepoints.size(); i++) {
        textWidth +=
            (i ? stbtt_GetCodepointKernAdvance(&opts.font->info, codepoints[i - 1], codepoints[i]) : 0) +
            opts.font->getGlyph(codepoints[i])->advanceWidth;
    }

    return textWidth * opts.font->scale;
}

float Text::calcLineHeight(const Options& opts)
{
    return opts.font->calcFontHeight() * opts.lineHeightScale;
}
